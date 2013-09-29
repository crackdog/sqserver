#ifndef _HANDLE_CLIENT_H_
#define _HANDLE_CLIENT_H_

#include "sqserver.h"

extern int handle_client(int clientSocket, int ts3Socket);
extern void clientLogin(int sock);
extern boolean isAllowedMsg(const char * msg);

#endif
