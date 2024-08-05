/**
 * @file client.h
 * @brief Defines the FTPClient class for Drexel Protocol.
 *
 * This file contains the definition of the FTPClient class, which is a derived class
 * from FTP and provides functionality to connect and start FTP operations.
 *
 * @author  Satwik Shresth <ss5278@drexel.edu>
 * @date June 12, 2024
 */

#pragma once

#include <drexelprotocol/ftp.h>
#include <drexelprotocol/connection.h>

#include <cstring>

namespace DrexelProtocol
{

/**
 * @class FTPClient
 * @brief A class for FTP client operations in Drexel Protocol.
 *
 * FTPClient inherits from the FTP class and provides methods to connect to an FTP server
 * and start FTP operations.
 */
class FTPClient : public FTP
{
public:
   /**
    * @brief Constructs an FTPClient object.
    *
    * Initializes the FTP client with the specified file path, address, and port.
    *
    * @param filePath The file path for FTP operations.
    * @param addr The address of the FTP server.
    * @param port The port number of the FTP server.
    */
   FTPClient(const std::string filePath, const char* addr, int port);

   /**
    * @brief Destroys the FTPClient object.
    *
    * Cleans up any resources used by the FTP client.
    */
   ~FTPClient();

   /**
    * @brief Connects to the FTP server.
    *
    * Establishes a connection to the FTP server using the provided address and port.
    *
    * @return int Returns 0 on success, or an error code on failure.
    */
   int connect();

   /**
    * @brief Starts the FTP operation.
    *
    * Overrides the start method from the FTP base class to begin FTP operations.
    */
   void start() override;
};

}  // namespace DrexelProtocol

