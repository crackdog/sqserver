#ifndef _SERVER_H_
#define _SERVER_H_

//private
extern int init_server_socket(int local_port);
extern int handle_client();
extern int connectToTS3Server(unsigned int port);
extern void clientLogin(int sock);

//public
extern int start_server(void);
extern void kill_server(void);
extern void terminate_server(void);

#endif
