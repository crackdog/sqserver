#include "sqserver.h"
#include "server.h"
#include "log.h"
#include "encryption.h"

int serverSocket, clientSocket, ts3Socket;

int start_server()
{
  boolean running;
  socklen_t client_addr_len;
  struct sockaddr_in client_addr;
  pid_t pid;
  int retvalue;
  
  retvalue = 0;

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
        
        ts3Socket = connectToTS3Server(TS3_SERVER_PORT);
        if(ts3Socket == -1)
        {
          serverlog("couldn't connect to ts3 server");
          return 2;
        }
        
        retvalue = handle_client(clientSocket, ts3Socket);
        
        close(clientSocket);
        
        terminate_log();
        //exit(retvalue);
        running = FALSE;
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
  return retvalue;
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
  close(ts3Socket);
  serverlog("server stopped");
}

int connectToTS3Server(unsigned int port)
{
    int s;
    struct hostent *host;
    struct sockaddr_in addr;

    if (!inet_aton("localhost", &addr.sin_addr))
    {
        host = gethostbyname("localhost");
        if (!host)
        {
            return -1;
        }
        addr.sin_addr = *(struct in_addr*)host->h_addr;
    }

    s = socket(PF_INET, SOCK_STREAM, 0);
    if (s == -1)
    {
        return -1;
    }

    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;

    if (connect(s, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        return -1;
    }

    return s;
}


