/**
 * @file ftp.h
 * @brief Defines the FTP class and related structures for Drexel Protocol.
 *
 * This file contains the definition of the FTP class, enums, and structures
 * used in the Drexel Protocol.
 *
 * @date June 12, 2024
 * @authoe Satwik Shresth <ss5278@drexel.edu>
 */

#pragma once

#include <drexelprotocol/pdu.h>

#include <cstdint>
#include <string>

namespace DrexelProtocol
{

using connection = DrexelProtocol::Connection<PDU>;

/**
 * @enum Error
 * @brief Defines error codes for FTP operations.
 */
typedef enum
{
   ACCESS_DENIED = -2, /**< Access to the requested resource is denied. */
   FILE_NOT_FOUND,     /**< The requested file was not found. */
   NONE,               /**< No error. */
   UNKOWN = 99         /**< An unknown error occurred. */
} Error;

/**
 * @enum Status
 * @brief Defines status codes for FTP operations.
 */
typedef enum
{
   NEW = 0, /**< The operation is new. */
   APPEND   /**< The operation is an append. */
} Status;

/**
 * @struct FTP_PDU
 * @brief Defines the protocol data unit for FTP operations.
 */
struct FTP_PDU
{
   char           fileName[100]; /**< The name of the file. */
   const uint32_t proto_ver = 1; /**< The protocol version. */
   int            status;        /**< The status of the operation. */
   int            err;           /**< The error code, if any. */
};

/**
 * @class FTP
 * @brief A base class for FTP operations in Drexel Protocol.
 *
 * The FTP class provides a base for FTP operations, including validation
 * and start methods.
 */
class FTP
{
public:
   static constexpr int BUFF_SZ = 512; /**< The buffer size for FTP operations. */

   connection* dpc; /**< The connection for FTP operations. */

   std::string filePath;                           /**< The file path for FTP operations. */
   char        sbuffer[BUFF_SZ + sizeof(FTP_PDU)]; /**< The send buffer. */
   char        rbuffer[BUFF_SZ + sizeof(FTP_PDU)]; /**< The receive buffer. */

   /**
    * @brief Constructs an FTP object with the specified file path.
    *
    * @param filePath The file path for FTP operations.
    */
   FTP(std::string filePath) : filePath(filePath){};

   /**
    * @brief Constructs an FTP object with the specified file path and connection.
    *
    * @param filePath The file path for FTP operations.
    * @param dpc The connection for FTP operations.
    */
   FTP(std::string filePath, connection* dpc) : dpc(dpc), filePath(filePath){};

   /**
    * @brief Destroys the FTP object.
    */
   ~FTP()
   {}

   /**
    * @brief Validates the FTP connection.
    *
    * @return bool Returns true if the connection is valid, false otherwise.
    */
   virtual bool validate()
   {
      return dpc != nullptr;
   }

   /**
    * @brief Starts the FTP operation.
    */
   virtual void start()
   {}

   /**
    * @brief Starts the FTP operation with an index.
    * @param idx The index to start the FTP operation with.
    */
   virtual void start(int idx) {}
};

}  // namespace DrexelProtocol
