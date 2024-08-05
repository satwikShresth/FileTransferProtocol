/**
 * @file connection.h
 * @brief Defines the Connection class for Drexel Protocol.
 *
 * This file contains the definition of the Connection class template used in the Drexel Protocol.
 *
 * @date June 12, 2024
 * @authoe Satwik Shresth <ss5278@drexel.edu>
 */

#pragma once

#include <arpa/inet.h>
#include <drexelprotocol/msgtype.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <unordered_map>

namespace DrexelProtocol
{
/**
 * @struct Sock
 * @brief Represents a socket address structure.
 *
 * This structure contains information about a socket address, including its length,
 * initialization status, and the sockaddr_in structure.
 */
typedef struct
{
   socklen_t          len;        /**< The length of the socket address. */
   bool               isAddrInit; /**< Indicates if the address is initialized. */
   struct sockaddr_in addr;       /**< The socket address. */
} Sock;

/**
 * @class Connection
 * @brief A class template for managing connections in Drexel Protocol.
 *
 * The Connection class template manages UDP connections, including sending and receiving
 * datagrams, handling connection states, and managing sequence numbers.
 */
template <typename PDU>
class Connection
{
public:
   static constexpr int MAX_BUFF_SZ       = 512;                       /**< Maximum buffer size. */
   static constexpr int MAX_DGRAM_SZ      = MAX_BUFF_SZ + sizeof(PDU); /**< Maximum datagram size. */
   static constexpr int NO_ERROR          = 0;                         /**< No error. */
   static constexpr int ERROR_GENERAL     = -1;                        /**< General error. */
   static constexpr int ERROR_PROTOCOL    = -2;                        /**< Protocol error. */
   static constexpr int ERROR_BAD_DGRAM   = -32;                       /**< Bad datagram error. */
   static constexpr int BUFF_UNDERSIZED   = -4;                        /**< Buffer undersized error. */
   static constexpr int BUFF_OVERSIZED    = -8;                        /**< Buffer oversized error. */
   static constexpr int CONNECTION_CLOSED = -16;                       /**< Connection closed error. */

private:
   int          udpSock;     /**< UDP socket. */
   unsigned int seqNum;      /**< Sequence number. */
   bool         connected;   /**< Connection status. */
   int          dbgMode;     /**< Debug mode flag. */
   Sock         outSockAddr; /**< Outgoing socket address. */
   Sock         inSockAddr;  /**< Incoming socket address. */

public:
   std::unordered_map<std::string, unsigned int> seqNums; /**< Sequence numbers map. */

   char _buffer[MAX_DGRAM_SZ]; /**< Buffer for datagrams. */

   /**
    * @brief Constructs a Connection object.
    */
   Connection();

   /**
    * @brief Destroys the Connection object.
    */
   ~Connection();

   /**
    * @brief Closes the connection.
    */
   void close();

   /**
    * @brief Gets the incoming socket address.
    *
    * @return Sock* Pointer to the incoming socket address.
    */
   Sock* getInSockAddr();

   /**
    * @brief Gets the outgoing socket address.
    *
    * @return Sock* Pointer to the outgoing socket address.
    */
   Sock* getOutSockAddr();

   /**
    * @brief Gets the UDP socket.
    *
    * @return int* Pointer to the UDP socket.
    */
   int* getUdpSock();

   /**
    * @brief Gets the maximum datagram size.
    *
    * @return int The maximum datagram size.
    */
   int maxDgram() const;

   /**
    * @brief Receives data into a buffer.
    *
    * @param buff The buffer to receive data into.
    * @param buff_sz The size of the buffer.
    * @return int The number of bytes received, or an error code.
    */
   int recv(void* buff, int buff_sz);

   /**
    * @brief Sends data from a buffer.
    *
    * @param sbuff The buffer to send data from.
    * @param sbuff_sz The size of the buffer.
    * @return int The number of bytes sent, or an error code.
    */
   int send(void* sbuff, int sbuff_sz);

   /**
    * @brief Sends a datagram from a buffer.
    *
    * @param sbuff The buffer to send data from.
    * @param sbuff_sz The size of the buffer.
    * @return int The number of bytes sent, or an error code.
    */
   int sendDgram(void* sbuff, int sbuff_sz);

   /**
    * @brief Receives a raw datagram into a buffer.
    *
    * @param buff The buffer to receive data into.
    * @param buff_sz The size of the buffer.
    * @return int The number of bytes received, or an error code.
    */
   int recvRaw(void* buff, int buff_sz);

   /**
    * @brief Receives a datagram into a buffer.
    *
    * This function receives a datagram into the specified buffer and processes it according to
    * the protocol rules. It handles various error conditions and manages sequence numbers.
    *
    * @param buff The buffer to receive the datagram into.
    * @param buffSz The size of the buffer.
    * @return int The number of bytes received, or an error code.
    */
   int recvDgram(void* buff, int buffSz);

   /**
    * @brief Sends a raw datagram from a buffer.
    *
    * @param sbuff The buffer to send data from.
    * @param sbuff_sz The size of the buffer.
    * @return int The number of bytes sent, or an error code.
    */
   int sendRaw(void* sbuff, int sbuff_sz);

   /**
    * @brief Listens for incoming connections.
    *
    * @return int The status of the listen operation.
    */
   int listen();

   /**
    * @brief Establishes a connection.
    *
    * @return int The status of the connect operation.
    */
   int connect();

   /**
    * @brief Disconnects the connection.
    *
    * @return int The status of the disconnect operation.
    */
   int disconnect();

   /**
    * @brief Generates a random number with a threshold.
    *
    * @param threshold The threshold for generating the random number.
    * @return int The generated random number.
    */
   int rand(int threshold);

   /**
    * @brief Checks if the connection is established.
    *
    * @return bool True if the connection is established, false otherwise.
    */
   bool isConnected();

   /**
    * @brief Prepares data for sending.
    *
    * @param pdu_ptr Pointer to the PDU.
    * @param buff The buffer to prepare data in.
    * @param buff_sz The size of the buffer.
    * @return void* Pointer to the prepared buffer.
    */
   void* prepareSend(PDU* pdu_ptr, void* buff, int buff_sz);
};

template <typename PDU>
int Connection<PDU>::rand(int threshold)
{
   if (threshold < 1)
      return 0;
   if (threshold > 99)
      return 1;
   srand(time(0));

   int rndInRange = (std::rand() % (100 - 1 + 1)) + 1;
   return (threshold < rndInRange) ? 1 : 0;
}

template <typename PDU>
Connection<PDU>::Connection() : udpSock(0), seqNum(0), connected(false), dbgMode(1)
{}

template <typename PDU>
Connection<PDU>::~Connection()
{
   close();
}

template <typename PDU>
void Connection<PDU>::close()
{
   ::close(udpSock);
}

template <typename PDU>
int Connection<PDU>::maxDgram() const
{
   return MAX_BUFF_SZ;
}

template <typename PDU>
int Connection<PDU>::recv(void* buff, int buffSz)
{
   int   total      = 0;
   char* rPtr       = (char*) buff;
   int   isFragment = true;

   while (isFragment)
   {
      memset(_buffer, 0, sizeof(_buffer));
      int rcvLen = recvDgram(_buffer, sizeof(_buffer));

      if (rcvLen == CONNECTION_CLOSED)
         return CONNECTION_CLOSED;

      int copied = (total + (rcvLen - sizeof(PDU)) > buffSz) ? (buffSz - total) : rcvLen - sizeof(PDU);
      memcpy(rPtr, _buffer + sizeof(PDU), copied);
      rPtr += copied;
      total += copied;

      isFragment = (((PDU*) _buffer)->mtype & MsgType::FRAGMENT) == MsgType::FRAGMENT;
   }

   return total;
}

template <typename PDU>
int Connection<PDU>::recvDgram(void* buff, int buffSz)
{
   int bytesIn = 0;
   int errCode = NO_ERROR;

   if (buffSz > MAX_DGRAM_SZ)
      return BUFF_OVERSIZED;

   bytesIn = recvRaw(buff, buffSz);

   if (bytesIn < sizeof(PDU))
      errCode = ERROR_BAD_DGRAM;

   PDU inPdu;
   memcpy(&inPdu, buff, sizeof(PDU));
   if (inPdu.dgram_sz > buffSz)
      errCode = BUFF_UNDERSIZED;

   if (errCode == NO_ERROR)
   {
      if (inPdu.dgram_sz == 0)
         seqNum++;
      else
         seqNum += inPdu.dgram_sz;
   }
   else
   {
      seqNum++;
   }

   PDU outPdu;
   outPdu.dgram_sz = 0;
   outPdu.seqnum   = seqNum;
   outPdu.err_num  = errCode;

   int actSndSz = 0;
   if (errCode != NO_ERROR)
   {
      outPdu.mtype = MsgType::ERROR;
      actSndSz     = sendRaw(&outPdu, sizeof(PDU));
      if (actSndSz != sizeof(PDU))
         return ERROR_PROTOCOL;
   }

   else if ((inPdu.mtype & MsgType::FRAGMENT) == MsgType::FRAGMENT)
   {
      outPdu.mtype = MsgType::SENDFRAGMENTACK;
      actSndSz     = sendRaw(&outPdu, sizeof(PDU));
      if (actSndSz != sizeof(PDU))
         return ERROR_PROTOCOL;
      return bytesIn;
   }

   switch (inPdu.mtype)
   {
      case MsgType::SND:
         outPdu.mtype = MsgType::SNDACK;
         actSndSz     = sendRaw(&outPdu, sizeof(PDU));
         if (actSndSz != sizeof(PDU))
            return ERROR_PROTOCOL;
         break;
      case MsgType::CLOSE:
         outPdu.mtype = MsgType::CLOSEACK;
         actSndSz     = sendRaw(&outPdu, sizeof(PDU));
         if (actSndSz != sizeof(PDU))
            return ERROR_PROTOCOL;
         close();
         return CONNECTION_CLOSED;
      default:
         std::cerr << "ERROR: Unexpected or bad mtype in header " << inPdu.mtype << std::endl;
         return ERROR_PROTOCOL;
   }

   return bytesIn;
}

template <typename PDU>
int Connection<PDU>::recvRaw(void* buff, int buffSz)
{
   int bytes = 0;

   if (!inSockAddr.isAddrInit)
   {
      perror("recv: connection not setup properly - cli struct not init");
      return -1;
   }

   bytes = recvfrom(udpSock, (char*) buff, buffSz, MSG_WAITALL, (struct sockaddr*) &(outSockAddr.addr), &(outSockAddr.len));

   if (bytes < 0)
   {
      perror("recv: received error from recvfrom()");
      return -1;
   }
   outSockAddr.isAddrInit = true;

   if (bytes > sizeof(PDU))
   {
      if (false)
      {
         PDU*  inPdu   = (PDU*) buff;
         char* payload = (char*) buff + sizeof(PDU);
         printf("DATA : %.*s\n", inPdu->dgram_sz, payload);
      }
   }

   PDU* inPdu = (PDU*) buff;
   inPdu->printIn(dbgMode);

   return bytes;
}

template <typename PDU>
int Connection<PDU>::send(void* sbuff, int sbuff_sz)
{
   char* sPtr  = (char*) sbuff;
   int   total = 0;

   while (sbuff_sz > 0)
   {
      int sndSz = sendDgram(sPtr, sbuff_sz);

      if (sndSz <= 0)
         return -1;

      sPtr += sndSz;
      sbuff_sz -= sndSz;
      total += sndSz;
   }

   return total;
}

template <typename PDU>
int Connection<PDU>::sendDgram(void* sbuff, int sbuff_sz)
{
   int bytesOut = 0;

   if (!outSockAddr.isAddrInit)
   {
      perror("send: connection not setup properly");
      return ERROR_GENERAL;
   }

   // if (sbuff_sz > MAX_BUFF_SZ)
   //    return ERROR_GENERAL;

   PDU* outPdu      = (PDU*) _buffer;
   outPdu->seqnum   = seqNum;
   outPdu->mtype    = (sbuff_sz > MAX_BUFF_SZ) ? MsgType::SENDFRAGMENT : MsgType::SND;
   outPdu->dgram_sz = sbuff_sz > MAX_BUFF_SZ ? MAX_BUFF_SZ : sbuff_sz;

   memcpy((_buffer + sizeof(PDU)), sbuff, outPdu->dgram_sz);

   int totalSendSz = outPdu->dgram_sz + sizeof(PDU);
   bytesOut        = sendRaw(_buffer, totalSendSz);

   if (bytesOut != totalSendSz)
   {
      std::cerr << "Warning send " << bytesOut << ", but expected " << totalSendSz << "!" << std::endl;
   }

   if (outPdu->dgram_sz == 0)
      seqNum++;
   else
      seqNum += outPdu->dgram_sz;

   PDU inPdu   = {0};
   int bytesIn = recvRaw(&inPdu, sizeof(PDU));
   if ((bytesIn < sizeof(PDU)) && (inPdu.mtype != MsgType::SNDACK))
   {
      std::cerr << "Expected SND/ACK but got a different mtype " << inPdu.mtype << std::endl;
   }

   return bytesOut - sizeof(PDU);
}

template <typename PDU>
int Connection<PDU>::sendRaw(void* sbuff, int sbuff_sz)
{
   int bytesOut = 0;

   if (!outSockAddr.isAddrInit)
   {
      perror("sendRaw: connection not setup properly");
      return -1;
   }

   PDU* outPdu = (PDU*) sbuff;
   bytesOut    = sendto(udpSock, (const char*) sbuff, sbuff_sz, 0, (const struct sockaddr*) &(outSockAddr.addr), outSockAddr.len);

   outPdu->printOut(dbgMode);

   return bytesOut;
}

template <typename PDU>
int Connection<PDU>::listen()
{
   int sndSz, rcvSz;

   if (!inSockAddr.isAddrInit)
   {
      perror("listen: connection not setup properly - cli struct not init");
      return ERROR_GENERAL;
   }

   PDU pdu = {0};

   std::cout << "Waiting for a connection..." << std::endl;
   rcvSz = recvRaw(&pdu, sizeof(pdu));
   if (rcvSz != sizeof(pdu))
   {
      perror("listen: The wrong number of bytes were received");
      return ERROR_GENERAL;
   }

   pdu.mtype  = MsgType::CNTACK;
   seqNum     = pdu.seqnum + 1;
   pdu.seqnum = seqNum;

   sndSz = sendRaw(&pdu, sizeof(pdu));

   if (sndSz != sizeof(pdu))
   {
      perror("listen: The wrong number of bytes were sent");
      return ERROR_GENERAL;
   }

   connected = true;
   std::cout << "Connection established OK!" << std::endl;

   return true;
}

template <typename PDU>
bool Connection<PDU>::isConnected()
{
   return connected;
}

template <typename PDU>
int Connection<PDU>::connect()
{
   int sndSz, rcvSz;

   if (!outSockAddr.isAddrInit)
   {
      perror("connect: connection not setup properly - svr struct not init");
      return ERROR_GENERAL;
   }

   PDU pdu      = {0};
   pdu.mtype    = MsgType::CONNECT;
   pdu.seqnum   = seqNum;
   pdu.dgram_sz = 0;

   sndSz = sendRaw(&pdu, sizeof(pdu));
   if (sndSz != sizeof(PDU))
   {
      perror("connect: Wrong amount of connection data sent");
      return -1;
   }

   rcvSz = recvRaw(&pdu, sizeof(pdu));
   if (rcvSz != sizeof(PDU))
   {
      perror("connect: Wrong amount of connection data received");
      return -1;
   }
   if (pdu.mtype != MsgType::CNTACK)
   {
      perror("connect: Expected CNTACT Message but didn't get it");
      return -1;
   }

   seqNum++;
   connected = true;
   std::cout << "Connection established OK!" << std::endl;

   return true;
}

template <typename PDU>
int Connection<PDU>::disconnect()
{
   int sndSz, rcvSz;

   PDU pdu      = {};
   pdu.mtype    = MsgType::CLOSE;
   pdu.seqnum   = seqNum;
   pdu.dgram_sz = 0;

   sndSz = sendRaw(&pdu, sizeof(pdu));
   if (sndSz != sizeof(PDU))
   {
      perror("disconnect: Wrong amount of connection data sent");
      return ERROR_GENERAL;
   }

   rcvSz = recvRaw(&pdu, sizeof(pdu));
   if (rcvSz != sizeof(PDU))
   {
      perror("disconnect: Wrong amount of connection data received");
      return ERROR_GENERAL;
   }
   if (pdu.mtype != MsgType::CLOSEACK)
   {
      perror("disconnect: Expected CNTACT Message but didn't get it");
      return ERROR_GENERAL;
   }
   close();

   return CONNECTION_CLOSED;
}

template <typename PDU>
void* Connection<PDU>::prepareSend(PDU* pdu_ptr, void* buff, int buffSz)
{
   if (buffSz < sizeof(PDU))
   {
      perror("Expected CNTACT Message but didn't get it");
      return nullptr;
   }
   std::memset(buff, 0, buffSz);
   std::memcpy(buff, pdu_ptr, sizeof(PDU));

   return (char*) buff + sizeof(PDU);
}

template <typename PDU>
Sock* Connection<PDU>::getInSockAddr()
{
   return &inSockAddr;
}

template <typename PDU>
Sock* Connection<PDU>::getOutSockAddr()
{
   return &outSockAddr;
}

template <typename PDU>
int* Connection<PDU>::getUdpSock()
{
   return &udpSock;
}

}  // namespace DrexelProtocol
