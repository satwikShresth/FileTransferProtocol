/**
 * @file pdu.h
 * @brief Defines the PDU structure for Drexel Protocol.
 *
 * This file contains the definition of the PDU structure used in the Drexel Protocol,
 * along with methods to print the details of the PDU.
 *
 * @date June 12, 2024
 * @authoe Satwik Shresth <ss5278@drexel.edu>
 */

#pragma once

#include <drexelprotocol/msgtype.h>
#include <drexelprotocol/connection.h>

#include <iostream>

namespace DrexelProtocol
{

/**
 * @struct PDU
 * @brief Defines the protocol data unit for Drexel Protocol.
 *
 * The PDU structure represents the protocol data unit used in Drexel Protocol,
 * containing information such as protocol version, message type, sequence number,
 * datagram size, and error number. It also includes methods to print the details
 * of the PDU.
 */
struct PDU
{
   const int proto_ver = 1; /**< The protocol version. */
   int       mtype;         /**< The message type. */
   int       seqnum;        /**< The sequence number. */
   int       dgram_sz;      /**< The datagram size. */
   int       err_num;       /**< The error number. */

   /**
    * @brief Prints the PDU details when sending, if debug mode is enabled.
    *
    * @param dbgMode Indicates whether debug mode is enabled.
    */
   void printOut(bool dbgMode) const
   {
      if (!dbgMode)
         return;
      std::cout << "PDU DETAILS ===>  [OUT]" << std::endl;
      printDetails();
   }

   /**
    * @brief Prints the PDU details when receiving, if debug mode is enabled.
    *
    * @param dbgMode Indicates whether debug mode is enabled.
    */
   void printIn(bool dbgMode) const
   {
      if (!dbgMode)
         return;
      std::cout << "===> PDU DETAILS  [IN]" << std::endl;
      printDetails();
   }

   /**
    * @brief Prints the details of the PDU.
    */
   void printDetails() const
   {
      std::cout << "\tVersion:  " << proto_ver << std::endl;
      std::cout << "\tMsg Type: " << msgToString(mtype) << std::endl;
      std::cout << "\tMsg Size: " << dgram_sz << std::endl;
      std::cout << "\tSeq Numb: " << seqnum << std::endl;
      std::cout << std::endl;
   }
};

}  // namespace DrexelProtocol

