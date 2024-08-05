
#include "drexelprotocol/server.h"

#include <cstdio>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>

#include "channel/channel.h"

using writer = DrexelProtocol::FTPFileWriter;
using server = DrexelProtocol::FTPServer;

writer::FTPFileWriter::FTPFileWriter(std::string address) : stream(makeChannel<std::string>(20)), address(address)
{}

::channel<std::string>* writer::getChannel()
{
   return stream;
}

void writer::pushToChannel(char* buff, int buffSz)
{
   std::cout << "========================> \n"
             << "Size :" << buffSz << std::endl
             << "========================> \n";
   stream->send(std::string(buff, buffSz));
}

void writer::serverLoop()
{
   while (!stream->isClosed())
   {
      std::string buff;

      try
      {
         buff = stream->receive();
      }
      catch (std::runtime_error e)
      {
         break;
      }
      FTP_PDU* pdu = reinterpret_cast<FTP_PDU*>(buff.data());
      std::cout << "filename: " << pdu->fileName << std::endl;

      auto mode = (pdu->status == Status::NEW) ? std::ios::trunc : std::ios::app;

      std::ofstream outFile{pdu->fileName, std::ios::out | mode};

      if (!outFile.is_open())
      {
         std::cerr << "ERROR:  Cannot open file " << pdu->fileName << std::endl;
         exit(-1);
      }
      buff.erase(0, sizeof(FTP_PDU));
      outFile << buff;

      // fwrite(dataPtr, 1, dataSz, f);
      std::cout << "========================> \n" << buff << "\n========================> \n";
   }
   delete stream;
   closed = true;
}

server::FTPServer(const std::string filePath, int port) : FTP(filePath, new connection()), pool(new ThreadPool())
{
   struct sockaddr_in* servaddr = &(dpc->getInSockAddr()->addr);
   int*                sock     = dpc->getUdpSock();

   if ((*sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
   {
      perror("socket creation failed");
      delete dpc;
   }

   memset(servaddr, 0, sizeof(*servaddr));  // Initialize servaddr
   servaddr->sin_family      = AF_INET;
   servaddr->sin_addr.s_addr = INADDR_ANY;
   servaddr->sin_port        = htons(port);

   int val = 1;
   if (setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)) < 0)
   {
      perror("setsockopt(SO_REUSEADDR) failed");
      ::close(*sock);
      delete dpc;
   }

   if (bind(*sock, (const struct sockaddr*) servaddr, sizeof(*servaddr)) < 0)
   {
      perror("bind failed");
      ::close(*sock);
      delete dpc;
   }

   dpc->getInSockAddr()->isAddrInit = true;
   dpc->getOutSockAddr()->len       = sizeof(struct sockaddr_in);
}

void server::listen()
{
   int sndSz, rcvSz;

   if (!dpc->getInSockAddr()->isAddrInit)
   {
      perror("listen: connection not setup properly - cli struct not init");
   }

   std::cout << "Waiting for a new connection..." << std::endl;

   memset(dpc->_buffer, 0, sizeof(dpc->_buffer));

   rcvSz = dpc->recvRaw(dpc->_buffer, sizeof(dpc->_buffer));

   std::string address = inet_ntoa(dpc->getOutSockAddr()->addr.sin_addr);

   if (rcvSz == sizeof(PDU))
   {
      PDU pdu;
      pdu.seqnum = 0;
      pdu.mtype  = MsgType::CNTACK;

      dpc->seqNums[inet_ntoa(dpc->getOutSockAddr()->addr.sin_addr)] = pdu.seqnum + 1;

      pdu.seqnum = dpc->seqNums[inet_ntoa(dpc->getOutSockAddr()->addr.sin_addr)];

      sndSz = dpc->sendRaw(&pdu, sizeof(PDU));

      if (sndSz != sizeof(PDU))
      {
         perror("listen: The wrong number of bytes were sent");
      }

      connected++;
      fw.push_back(new FTPFileWriter{address});

      pool->submit([&] {
         FTPFileWriter* writer = fw.back();
         fw.pop_back();
         writer->serverLoop();
         delete writer;
         ftpWriters.erase(writer->address);
      });

      ftpWriters[address] = fw.back();
      std::cout << "Connection established OK!" << std::endl;
   }
   else
   {
      int errCode = dpc->NO_ERROR;
      int buffSz  = sizeof(dpc->_buffer);

      if (rcvSz < sizeof(PDU))
         errCode = dpc->ERROR_BAD_DGRAM;

      PDU inPdu;
      memcpy(&inPdu, dpc->_buffer, sizeof(PDU));

      if (inPdu.dgram_sz > buffSz)
         errCode = dpc->BUFF_UNDERSIZED;

      if (errCode == dpc->NO_ERROR)
      {
         if (inPdu.dgram_sz == 0)
            dpc->seqNums[inet_ntoa(dpc->getOutSockAddr()->addr.sin_addr)]++;
         else
            dpc->seqNums[inet_ntoa(dpc->getOutSockAddr()->addr.sin_addr)] += inPdu.dgram_sz;
      }
      else
      {
         dpc->seqNums[inet_ntoa(dpc->getOutSockAddr()->addr.sin_addr)]++;
      }

      PDU outPdu;
      outPdu.dgram_sz = 0;
      outPdu.seqnum   = dpc->seqNums[inet_ntoa(dpc->getOutSockAddr()->addr.sin_addr)];
      outPdu.err_num  = errCode;

      int actSndSz = 0;
      if (errCode != dpc->NO_ERROR)
      {
         outPdu.mtype = MsgType::ERROR;
         actSndSz     = dpc->sendRaw(&outPdu, sizeof(PDU));
         if (actSndSz != sizeof(PDU))
            std::cerr << "ERROR: not no error" << inPdu.mtype << std::endl;
      }
      else if ((inPdu.mtype & MsgType::FRAGMENT) == MsgType::FRAGMENT)
      {
         outPdu.mtype = MsgType::SENDFRAGMENTACK;
         actSndSz     = dpc->sendRaw(&outPdu, sizeof(PDU));
         if (actSndSz != sizeof(PDU))
            std::cerr << "ERROR: in frag " << inPdu.mtype << std::endl;
      }
      else
      {
         switch (inPdu.mtype)
         {
            case MsgType::SND:
               outPdu.mtype = MsgType::SNDACK;
               actSndSz     = dpc->sendRaw(&outPdu, sizeof(PDU));
               if (actSndSz != sizeof(PDU))
                  std::cerr << "ERROR: Unexpected or bad mtype in header " << inPdu.mtype << std::endl;
               // return dpc->ERROR_PROTOCOL;
               break;
            case MsgType::CLOSE:
               outPdu.mtype = MsgType::CLOSEACK;
               actSndSz     = dpc->sendRaw(&outPdu, sizeof(PDU));
               if (actSndSz != sizeof(PDU))
                  std::cerr << "ERROR: Unexpected or bad mtype in header " << inPdu.mtype << std::endl;
               ftpWriters[address]->getChannel()->close();
            default:
               std::cerr << "ERROR: Unexpected or bad mtype in header " << inPdu.mtype << std::endl;
         }
      }
      ftpWriters[address]->pushToChannel(dpc->_buffer + sizeof(PDU), rcvSz - sizeof(PDU));
   }

   std::this_thread::sleep_for(std::chrono::seconds(3));
}

server::~FTPServer()
{}

DrexelProtocol::connection* server::newConnection()
{
   return dpc;
}
