/* This file is Copyright 2005 Level Control Systems.  See the included LICENSE.txt file for details. */  

#include <stdio.h>

#include "dataio/TCPSocketDataIO.h"
#include "dataio/RS232DataIO.h"
#include "dataio/UDPSocketDataIO.h"

#include "system/SetupSystem.h"
#include "util/NetworkUtilityFunctions.h"
#include "util/StringTokenizer.h"

USING_NAMESPACE(muscle);

static const int DEFAULT_PORT = 2980;  // What LCS's lxtcpcomd daemon uses

static void DoSession(DataIO & io)
{
   SetSocketBlockingEnabled(STDIN_FILENO, false);
   int ss = io.GetSelectSocket();

   while(true)
   {
      fd_set readSet, writeSet;

      FD_ZERO(&readSet);
      FD_SET(ss, &readSet);
      FD_SET(STDIN_FILENO, &readSet);

      char buf[2048];
      if (select(muscleMax(STDIN_FILENO, ss)+1, &readSet, NULL, NULL, NULL) >= 0)
      {
         if (FD_ISSET(ss, &readSet))
         {
            int32 ret = io.Read(buf, sizeof(buf));
            if (ret > 0)
            {
               printf("Received: [");
               for (int32 i=0; i<ret; i++) printf("%s%02x", (i>0)?" ":"", ((uint32)buf[i])&0xFF);
               printf("]\n");
            }
            else if (ret < 0) break;
         }
         if (FD_ISSET(STDIN_FILENO, &readSet))
         {
            fgets(buf, sizeof(buf), stdin);
            if (feof(stdin)) break;

            uint32 count = 0;
            StringTokenizer tok(false, buf, ", \t\r\n");
            const char * next;
            while((next = tok()) != NULL) 
            {
               if (strlen(next) > 0) buf[count++] = (uint8) strtol(next, NULL, 16);
            }

            if (count>0)
            {
               if (io.Write(buf, count) == count)
               {
                  printf("Sent: [");
                  for (uint32 i=0; i<count; i++) printf("%s%02x", (i>0)?" ":"", ((uint32)buf[i])&0xFF); printf("]\n");
               }
               else break;
            }
         }
      }
      else break;
   }
}

static void PrintUsage()
{
   printf("Usage:  hexterm tcp=<port>              (listen for incoming TCP connections on the given port)\n");
   printf("   or:  hexterm tcp=<host>:<port>       (make an outgoing TCP connection to the given host/port)\n");
   printf("   or:  hexterm udp=<host>:<port>       (send outgoing UDP packets to the given host/port)\n");
   printf("   or:  hexterm udp=<port>              (listen for incoming UDP packets on the given port)\n");
   printf("   or:  hexterm serial=<devname>:<baud> (send/receive via a serial device, e.g. /dev/ttyS0)\n");
   exit(5);
}

// This program can send and receive raw hexadecimal bytes over TCP, UDP, or serial.
// Hex bytes are displayed and entered in ASCII format.
// Good for interactive debugging of low-level protocols like MIDI.
int main(int argc, char ** argv) 
{
   CompleteSetupSystem css;

   char * arg   = (argc>1)?argv[1]:NULL;
   char * equal = arg?strchr(arg,'='):NULL;
   char * colon = arg?strchr(arg,':'):NULL;
   if (equal == NULL) PrintUsage();

   int port = colon ? atoi(colon+1) : 0;
   if (port <= 0) port = DEFAULT_PORT;
   if (colon) *colon = '\0';  // so that the pre-colon arg can be read without trailing stuff

   if (strncmp(arg, "serial=", 7) == 0)
   {
      uint32 baudRate = colon ? port : 38400;
      String devName(arg+7);
      Queue<String> devs;
      if (RS232DataIO::GetAvailableSerialPortNames(devs) == B_NO_ERROR)
      {
         String serName;
         for (int32 i=devs.GetNumItems()-1; i>=0; i--)
         {
            if (devs[i] == devName) 
            {
               serName = devs[i];
               break;
            }
         }
         if (serName.Length() > 0)
         {
            RS232DataIO io(devName(), baudRate, false);
            if (io.IsPortAvailable())
            {
               LogTime(MUSCLE_LOG_INFO, "Communicating with serial port %s (baud rate %lu)\n", serName(), baudRate);
               DoSession(io);
               LogTime(MUSCLE_LOG_INFO, "Serial session aborted, exiting.\n");
            }
            else LogTime(MUSCLE_LOG_CRITICALERROR, "Unable to open serial device %s (baud rate %lu).\n", serName(), baudRate);
         }
         else 
         {
            LogTime(MUSCLE_LOG_CRITICALERROR, "Serial device %s not found.\n", devName());
            LogTime(MUSCLE_LOG_CRITICALERROR, "Available serial devices are:\n");
            for (uint32 i=0; i<devs.GetNumItems(); i++) LogTime(MUSCLE_LOG_CRITICALERROR, "   %s\n", devs[i]());
         }
      }
      else LogTime(MUSCLE_LOG_CRITICALERROR, "Couldn't get list of serial device names!\n");
   }
   else if (strncmp(arg, "tcp=", 4) == 0)
   {
      if (colon)
      {
         int ss = Connect(arg+4, port, "hexterm", false);
         if (ss >= 0) 
         {
            LogTime(MUSCLE_LOG_INFO, "Connected to [%s:%i]\n", arg+4, port);
            TCPSocketDataIO io(ss, false);
            DoSession(io);
            LogTime(MUSCLE_LOG_INFO, "Session socket disconnected, exiting.\n");
         }
         else LogTime(MUSCLE_LOG_CRITICALERROR, "Unable to connect to [%s:%i]\n", arg+4, port);
      }
      else
      {
         port = atoi(arg+4); if (port <= 0) port = DEFAULT_PORT;  // reinterpret arg as port
         int as = CreateAcceptingSocket(port);
         if (as >= 0)
         {
            LogTime(MUSCLE_LOG_INFO, "Listening for incoming TCP connections on port %i\n", port);
            while(true) 
            {
               int ss = Accept(as);
               if (ss >= 0)
               {
                  char buf[64]; Inet_NtoA(GetPeerIPAddress(ss), buf);
                  LogTime(MUSCLE_LOG_INFO, "Accepted TCP connection from %s, awaiting data...\n", buf);
                  TCPSocketDataIO io(ss, false);
                  DoSession(io);
                  LogTime(MUSCLE_LOG_ERROR, "Session socket disconnected, awaiting next connection.\n");
               }
            }
            CloseSocket(as);
         }
         else LogTime(MUSCLE_LOG_CRITICALERROR, "Couldn't bind to port %i\n", port);
      }
   }
   else if (strncmp(arg, "udp=", 4) == 0)
   {
      int ss = CreateUDPSocket();
      if (ss >= 0)
      {
         UDPSocketDataIO udpIO(ss, false);

         if (colon)
         {
            uint32 ip = GetHostByName(arg+4);
            if (ip > 0)
            {
               char buf[64]; Inet_NtoA(ip, buf);
               if (SetUDPSocketTarget(ss, ip, port) == B_NO_ERROR)
               {
                  char buf[64]; Inet_NtoA(ip, buf);
                  LogTime(MUSCLE_LOG_INFO, "Ready to send UDP packets to %s:%i\n", buf, port);
                  DoSession(udpIO);
               }
               else LogTime(MUSCLE_LOG_ERROR, "Couldn't set socket target to %s:%i!\n", buf, port);
            }
            else LogTime(MUSCLE_LOG_ERROR, "Couldn't look up target hostname [%s]\n", arg+4);
         }
         else 
         {
            port = atoi(arg+4); if (port <= 0) port = DEFAULT_PORT;  // reinterpret arg as port
            if (BindUDPSocket(ss, port) == B_NO_ERROR)
            {
               LogTime(MUSCLE_LOG_INFO, "Listening for incoming UDP packets on port %i\n", port);
               DoSession(udpIO);
            }
            else LogTime(MUSCLE_LOG_ERROR, "Couldn't bind UDP socket to port %i\n", port);
         }
      }
      else LogTime(MUSCLE_LOG_ERROR, "Error creating UDP socket!\n");
   }
   else PrintUsage();

   return 0;
}
