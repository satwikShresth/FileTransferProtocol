/**
 * @file msgtype.h
 * @brief Defines the message types for Drexel Protocol.
 * 
 * This file contains the definition of the MsgType enum and the msgToString function
 * for converting message types to their string representations.
 * 
 * @date June 12, 2024
 * @authoe Satwik Shresth <ss5278@drexel.edu>
 */

#pragma once

namespace DrexelProtocol
{

/**
 * @enum MsgType
 * @brief Defines message types for Drexel Protocol.
 * 
 * This enum defines various message types used in the Drexel Protocol, including
 * initial, acknowledgment, send, connect, close, negative acknowledgment, fragment,
 * and error messages. It also defines combinations of these message types.
 */
typedef enum
{
   INI      = 0,   /**< Initial message */
   ACK      = 1,   /**< Acknowledgment message */
   SND      = 2,   /**< Send message */
   CONNECT  = 4,   /**< Connect message */
   CLOSE    = 8,   /**< Close message */
   NACK     = 16,  /**< Negative acknowledgment */
   FRAGMENT = 32,  /**< Datagram is a fragment */
   ERROR    = 64,  /**< Simulate error */

   SNDACK          = (SND | ACK),             /**< Send acknowledgment message */
   CNTACK          = (CONNECT | ACK),         /**< Connect acknowledgment message */
   CLOSEACK        = (CLOSE | ACK),           /**< Close acknowledgment message */
   SENDFRAGMENT    = (FRAGMENT | SND),        /**< Send fragment message */
   SENDFRAGMENTACK = (FRAGMENT | SNDACK),     /**< Send fragment acknowledgment message */
} MsgType;

/**
 * @brief Converts a message type to its string representation.
 * 
 * @param mtype The message type to convert.
 * @return const char* The string representation of the message type.
 */
const char* msgToString(int mtype);

}  // namespace DrexelProtocol

