/**
 * @file FTPServer.h
 * @brief Defines the FTPServer and FTPFileWriter classes for Drexel Protocol.
 *
 * This file contains the definition of the FTPServer and FTPFileWriter classes,
 * which provide functionality for an FTP server and file writing operations.
 *
 * @date June 12, 2024
 * @authoe Satwik Shresth <ss5278@drexel.edu>
 */

#pragma once
#include <drexelprotocol/ftp.h>

#include <cstring>
#include <ctime>
#include <string>
#include <unordered_map>
#include <vector>

#include "channel/channel.h"
#include "threadpool/threadpool.h"

namespace DrexelProtocol
{

/**
 * @class FTPFileWriter
 * @brief A class for handling file writing operations in an FTP server.
 *
 * The FTPFileWriter class manages file writing operations, including pushing data
 * to a channel and running a server loop.
 */
class FTPFileWriter
{
private:
   bool closed{false}; /**< Indicates if the file writer is closed. */

   channel<std::string>* stream; /**< The channel for data communication. */

public:
   std::string address; /**< The address of the file writer. */

   /**
    * @brief Constructs an FTPFileWriter object.
    *
    * @param address The address of the file writer.
    */
   FTPFileWriter(std::string address);

   /**
    * @brief Gets the channel for data communication.
    *
    * @return channel<std::string>* Pointer to the channel.
    */
   ::channel<std::string>* getChannel();

   /**
    * @brief Pushes data to the channel.
    *
    * @param buff The buffer containing data.
    * @param buffSz The size of the buffer.
    */
   void pushToChannel(char* buff, int buffSz);

   /**
    * @brief Runs the server loop for the file writer.
    */
   void serverLoop();
};

/**
 * @class FTPServer
 * @brief A class for managing an FTP server.
 *
 * The FTPServer class manages FTP server operations, including listening for connections,
 * handling new connections, and managing file writers.
 */
class FTPServer : public FTP
{
private:
   int                         connected{0}; /**< Indicates if the server is connected. */
   ThreadPool*                 pool;         /**< The thread pool for handling tasks. */
   std::vector<FTPFileWriter*> fw;           /**< Vector of file writers. */

   std::unordered_map<std::string, FTPFileWriter*> ftpWriters; /**< Map of file writers by address. */

public:
   /**
    * @brief Constructs an FTPServer object.
    *
    * @param filePath The file path for the FTP server.
    * @param port The port number for the FTP server.
    */
   FTPServer(const std::string filePath, int port);

   /**
    * @brief Listens for incoming connections.
    */
   void listen();

   /**
    * @brief Destroys the FTPServer object.
    */
   ~FTPServer();

   /**
    * @brief Creates a new connection.
    *
    * @return connection* Pointer to the new connection.
    */
   connection* newConnection();
};

}  // namespace DrexelProtocol
