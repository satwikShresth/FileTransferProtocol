#include "drexelprotocol/client.h"

#include <drexelprotocol/ftp.h>

#include <cstring>
#include <filesystem>
#include <iostream>

using Client = DrexelProtocol::FTPClient;

Client::FTPClient(const std::string filePath, const char* addr, int port) : FTP(filePath, new connection())
{
   struct sockaddr_in* servaddr = &(dpc->getOutSockAddr()->addr);
   int*                sock     = dpc->getUdpSock();

   if ((*sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
   {
      perror("socket creation failed");
      delete dpc;
   }

   servaddr->sin_family              = AF_INET;
   servaddr->sin_port                = htons(port);
   servaddr->sin_addr.s_addr         = inet_addr(addr);
   dpc->getOutSockAddr()->len        = sizeof(struct sockaddr_in);
   dpc->getOutSockAddr()->isAddrInit = true;

   memcpy(dpc->getInSockAddr(), dpc->getOutSockAddr(), sizeof(*dpc->getOutSockAddr()));
}
Client::~FTPClient()
{
   delete dpc;
}

int Client::connect()
{
   return dpc->connect();
}

void Client::start()
{
   static char sBuff[sizeof(FTP_PDU) + 500];

   if (!dpc->isConnected())
   {
      std::cout << "Client not connected" << std::endl;
      return;
   }

   FILE* f = fopen(filePath.c_str(), "rb");
   if (f == nullptr)
   {
      std::cerr << "ERROR:  Cannot open file " << filePath << std::endl;
      exit(-1);
   }
   if (!dpc->isConnected())
   {
      perror("Expecting the protocol to be in connect state, but it's not");
      exit(-1);
   }

   int bytes   = 0;
   int pduSize = sizeof(FTP_PDU);

   FTP_PDU pdu;
   strcpy(pdu.fileName, std::filesystem::path{filePath.c_str()}.filename().c_str());
   pdu.status = Status::NEW;
   pdu.err    = Error::NONE;

   while ((bytes = fread(sBuff + pduSize, 1, 500, f)) > 0)
   {
      size_t remainingBytes = bytes;

      char* dataPtr = sBuff + pduSize;

      while (remainingBytes > 0)
      {
         std::memcpy(sBuff, &pdu, pduSize);

         size_t sendSize = remainingBytes + pduSize;

         int sndSz = dpc->sendDgram(sBuff, sendSize);

         if (sndSz < pduSize)
         {
            break;
         }

         remainingBytes -= (sndSz - pduSize);
         dataPtr += (sndSz - pduSize);

         if (remainingBytes > 0)
         {
            pdu.status = Status::APPEND;
            std::memmove(sBuff + pduSize, dataPtr, remainingBytes);
         }
      }
   }

   fclose(f);
   dpc->disconnect();
}
