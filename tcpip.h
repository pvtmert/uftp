
#ifndef _TCPIP_H
#define _TCPIP_H

#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define DEBUG
#define PADDING 10
#define DEBUG_PREFIX "[ %*u ] "
#define PORT 9090
#define BUFSIZE 1024
#define MAX_CONNS 99
#define CLI_HEADER "!PUT+"
#define SRV_HEADER "!_OK+"
#define TIME_DIFF "!_QT+"
#define HELO_PROT "!_HI+"

#endif

/*
> HELO_PROT
< SRV_HEADER
> CLI_HEADER
*/
