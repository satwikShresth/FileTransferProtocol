#include <drexelprotocol/msgtype.h>

using DrexelProtocol::MsgType;
const char* DrexelProtocol::msgToString(int mtype)
{
   switch (mtype)
   {
      case MsgType::ACK:
         return "ACK";
      case MsgType::SND:
         return "SEND";
      case MsgType::CONNECT:
         return "CONNECT";
      case MsgType::CLOSE:
         return "CLOSE";
      case MsgType::NACK:
         return "NACK";
      case MsgType::SNDACK:
         return "SEND/ACK";
      case MsgType::CNTACK:
         return "CONNECT/ACK";
      case MsgType::CLOSEACK:
         return "CLOSE/ACK";
      case MsgType::SENDFRAGMENT:
         return "SEND FRAGMENT";
      case MsgType::SENDFRAGMENTACK:
         return "SEND FRAGMENT/ACK";
      default:
         return "***UNKNOWN***";
   }
}
