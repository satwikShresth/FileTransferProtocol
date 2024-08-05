/**
 * @file main.cpp
 * @brief Main entry point for the Drexel Protocol FTP application.
 *
 * This application implements a concurrent UDP-based file transfer protocol
 * that uses channels and thread pools. It can operate in both client and server modes.
 *
 * @date June 12, 2024
 * @author Satwik Shresth <ss5278@drexel.edu>
 *
 * @mainpage Drexel Protocol FTP Application
 *
 * @section build_sec Build Instructions
 * To build the application, run `make`.
 *
 * @section usage_sec Usage
 * USAGE: ./bin/du-ftp [-p port] [-f fname] [-a svr_addr] [-s] [-c] [-h]
 *
 * WHERE:
 * - [-c] runs in client mode, [-s] runs in server mode; DEFAULT= client_mode
 * - [-a svr_addr] specifies the server's IP address as a string; DEFAULT = 127.0.0.1
 * - [-p portnum] specifies the port number; DEFAULT = 2080
 * - [-f fname] specifies the filename to send or receive; DEFAULT = test.c
 * - [-h] displays what you are looking at now - the help
 *
 *
 * @section Reference
 * This program was developed with insights and techniques from Dr. Brian Mitchell Class472 Assignment4-UDP:
 * https://github.com/ArchitectingSoftware/CS472-Class-Files/tree/main/hw4-udp
 */

#include <getopt.h>

#include <cstdlib>
#include <iostream>

#include "drexelprotocol/client.h"
#include "drexelprotocol/server.h"

namespace DPv1 = DrexelProtocol;

constexpr int PROG_MD_CLI = 0;
constexpr int PROG_MD_SVR = 1;
constexpr int DEF_PORT_NO = 2080;
// constexpr int FNAME_SZ    = 150;

constexpr const char* PROG_DEF_FNAME    = "test.c";
constexpr const char* PROG_DEF_SVR_ADDR = "127.0.0.1";

typedef struct ProgConfig
{
   int  progMode;
   int  portNumber;
   char svrIpAddr[16];
   char fileName[128];
} ProgConfig;

static int initParams(int argc, char* argv[], ProgConfig& cfg);

int main(int argc, char* argv[])
{
   ProgConfig cfg;
   int        cmd;
   int        rc;

   cmd = initParams(argc, argv, cfg);

   std::cout << "MODE " << cfg.progMode << std::endl;
   std::cout << "PORT " << cfg.portNumber << std::endl;
   std::cout << "FILE NAME: " << cfg.fileName << std::endl;

   switch (cmd)
   {
      case PROG_MD_CLI: {
         DPv1::FTPClient client{std::string(cfg.fileName), cfg.svrIpAddr, cfg.portNumber};

         if (!client.validate())
         {
            perror("Error initilizing client: ");
            exit(-1);
         }

         rc = client.connect();
         if (rc < 0)
         {
            perror("Error establishing connection");
            exit(-1);
         }

         client.start();
         break;
      }
      case PROG_MD_SVR: {
         DPv1::FTPServer server{std::string(cfg.fileName), cfg.portNumber};

         if (!server.validate())
         {
            perror("Error initilizing server: ");
            exit(-1);
         }
         while (true)
         {
            server.listen();
         }
         break;
      }
      default: {
         std::cerr << "ERROR: Unknown Program Mode.  Mode set is " << cmd << std::endl;
         break;
      }
   }

   return 0;
}

int initParams(int argc, char* argv[], ProgConfig& cfg)
{
   int         option;
   static char cmdBuffer[64] = {0};

   cfg.progMode   = PROG_MD_CLI;
   cfg.portNumber = DEF_PORT_NO;
   strcpy(cfg.fileName, PROG_DEF_FNAME);
   strcpy(cfg.svrIpAddr, PROG_DEF_SVR_ADDR);

   while ((option = getopt(argc, argv, ":p:f:a:csh")) != -1)
   {
      switch (option)
      {
         case 'p':
            strncpy(cmdBuffer, optarg, sizeof(cmdBuffer));
            cfg.portNumber = std::atoi(cmdBuffer);
            break;
         case 'f':
            strncpy(cfg.fileName, optarg, sizeof(cfg.fileName));
            break;
         case 'a':
            strncpy(cfg.svrIpAddr, optarg, sizeof(cfg.svrIpAddr));
            break;
         case 'c':
            cfg.progMode = PROG_MD_CLI;
            break;
         case 's':
            cfg.progMode = PROG_MD_SVR;
            break;
         case 'h':
            std::cout << "USAGE: " << argv[0] << " [-p port] [-f fname] [-a svr_addr] [-s] [-c] [-h]\n";
            std::cout << "WHERE:\n\t[-c] runs in client mode, [-s] runs in server mode; DEFAULT= client_mode\n";
            std::cout << "\t[-a svr_addr] specifies the server's IP address as a string; DEFAULT = " << cfg.svrIpAddr << "\n";
            std::cout << "\t[-p portnum] specifies the port number; DEFAULT = " << cfg.portNumber << "\n";
            std::cout << "\t[-f fname] specifies the filename to send or recv; DEFAULT = " << cfg.fileName << "\n";
            std::cout << "\t[-h] displays what you are looking at now - the help\n\n";
            exit(0);
         case ':':
            perror("Option missing value");
            exit(-1);
         default:
         case '?':
            perror("Unknown option");
            exit(-1);
      }
   }
   return cfg.progMode;
}
