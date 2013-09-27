#include "sqserver.h"
#include "server.h"
#include "log.h"

int serverSocket, clientSocket;;

int start_server()
{
  boolean running;
  socklen_t client_addr_len;
  struct sockaddr_in client_addr;
  pid_t pid;

  serverlog("starting server...");
  
  serverSocket = init_server_socket(SERVER_PORT);
  if(serverSocket == -1)
  {
    serverlog("problems creating server socket");
    return 1;
  }
  
  serverlog("server running...");
  
  running = TRUE;
  while(running)
  {
    //wait for incoming connection
    clientSocket = accept(serverSocket, (struct sockaddr *) &client_addr, &client_addr_len);
    if(clientSocket == -1)
    {
      serverlog("client connection problem...");
      //do nothing and wait for the next client.
    }
    else
    {
      pid = fork();
      
      if(pid == 0)
      {
        close(serverSocket);
        //handle client...
        
        serverlog("handle_client();");
        
        //if(write(clientSocket, "test\n", strlen("test\n")) == 0)
        
        int retvalue;
        retvalue = handle_client();
        
        close(clientSocket);
        
        terminate_log();
        exit(retvalue);
      }
      else
      {
        close(clientSocket);
        serverlogwi("handle client with pid: ", (long) pid, "");
      }
    }
    
    //running = FALSE;
    //sleep(1);
  }
  
  terminate_server();
  return 0;
}

int init_server_socket(int local_port)
{
  serverlogwi("initialize server socket on port '", local_port, "'");
  
  int s;
  struct sockaddr_in addr;

  s = socket(PF_INET, SOCK_STREAM, 0);
  if (s == -1)
  {
    serverlog("socket() failed");
    return -1;
  }

  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(local_port);
  addr.sin_family = AF_INET;

  if(bind(s, (struct sockaddr*)&addr, sizeof(addr)) == -1)
  {
    serverlog("bind() failed");
    return -1;
  }

  if(listen(s, 3) == -1)
  {
    serverlog("listen() failed");
    return -1;
  }

  return s;
}

void terminate_server()
{
  close(serverSocket);
  close(clientSocket);
  serverlog("server stopped");
}

int handle_client()
{
  return 0;
}

