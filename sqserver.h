#ifndef _SQSERVER_H_
#define _SQSERVER_H_

//------------------------
//includes:
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <time.h>
//------------------------

//************************
//defines:
#define TRUE            1
#define FALSE           0
#define BUF_SIZE        4096
#define TS3_SERVER_PORT 10011
#define SERVER_PORT     6666
//************************

//++++++++++++++++++++++++
//typedefs
typedef int boolean;
//++++++++++++++++++++++++

#endif
