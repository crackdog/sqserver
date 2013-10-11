#include "sqserver.h"
#include "handle_client.h"
#include "log.h"
#include "encryption.h"

int handle_client(int clientSocket, int ts3Socket)
{
  fd_set fds;
  int max, retvalue, bytes;
  boolean running;
  char * msgbuffer;
  struct timeval timer;
  
  msgbuffer = (char *) malloc(BUF_SIZE);
  
  //ts3Socket initialisieren...
  clientLogin(ts3Socket);
  
  running = TRUE;
  retvalue = 0;
  
  serverlogwi("running = ", running, "");
  
  while(running)
  {   
    //init buffer
    memset(msgbuffer, '\0', BUF_SIZE);
    
    //init select...  
    FD_ZERO(&fds);
    FD_SET(clientSocket, &fds);
    FD_SET(ts3Socket, &fds);
    max = clientSocket > ts3Socket ? clientSocket : ts3Socket;
    
    timer.tv_sec = 10;
    timer.tv_usec = 0;
    
    //select...
    if(select(max + 1, &fds, NULL, NULL, &timer) == -1)
    {
      //error
      serverlog("selecterror...quitting process");
      retvalue = 1;
      running = FALSE;
      break;
    }
    
    //handle current msg...
    if(FD_ISSET(clientSocket, &fds))
    {
      //handle client msg
      bytes = read(clientSocket, msgbuffer, BUF_SIZE);
      msgbuffer[bytes] = '\0';
      //if msg is empty the connection is closed
      if(bytes == 0)
      {
        serverlog("connection closed by client");
        running = FALSE;
      }
      else
      {
        //decrypt msg
/*        strncpy(msgbuffer, decrypt_msg(msgbuffer), BUF_SIZE);*/
        //check if msg is a allowed one
        if(isAllowedMsg(msgbuffer))
        {
          //send to ts3 server
          write(ts3Socket, msgbuffer, strlen(msgbuffer));
        }
        strncat(msgbuffer, "'", BUF_SIZE);
        write(clientSocket, msgbuffer, strlen(msgbuffer));
      }
    }
    else if(FD_ISSET(ts3Socket, &fds))
    {
      //handle ts3server msg
      //read msg
      bytes = read(ts3Socket, msgbuffer, BUF_SIZE);
      msgbuffer[bytes] = '\0';
      //if msg is empty the connnection is closed
      if(bytes == 0)
      {
        //serverlog("connection to ts3 server is closed");
        //running = FALSE;
        continue;
      }
      //encrypt msg
/*      strncpy(msgbuffer, encrypt_msg(msgbuffer), BUF_SIZE);*/
      //send msg
      write(clientSocket, msgbuffer, strlen(msgbuffer));
    }
    else
    {
      //handle timer
      strncpy(msgbuffer, "clientlist", BUF_SIZE);
      write(ts3Socket, msgbuffer, strlen(msgbuffer));
    }
  }
  
  close(ts3Socket);

  return 0;
}

void clientLogin(int sock)
{
  FILE * clfile;
  char * buffer;
  
  buffer = (char *) malloc(BUF_SIZE);
  
  clfile = fopen(CLOGINFILENAME, "r");
  if(clfile == NULL)
  {
    if(getlogdir() != NULL)
    {
      strncpy(buffer, getlogdir(), BUF_SIZE);
      strncat(buffer, "../conf/", BUF_SIZE);
    }
    else
    {
      strncpy(buffer, "~/sqserver/conf/", BUF_SIZE);
    }
    strncpy(buffer, "~/programming/sqserver/conf/", BUF_SIZE);
    strncat(buffer, CLOGINFILENAME, BUF_SIZE);
    clfile = fopen(buffer, "r");
    if(clfile == NULL)
    {
      //error
      //exit(666);
      return;
    }
  }
  
  while(!feof(clfile))
  {
    if(fgets(buffer, BUF_SIZE, clfile) != NULL)
    {
      write(sock, buffer, strlen(buffer)); //maybe without '+1'
    }
  }
  
  fclose(clfile);
  
  return;
}

boolean isAllowedMsg(const char * msg)
{
  char * allowedmsg[] = 
    {"clientlist",
    "channellist", 
    "sendtextmessage",
    "pokeclient",
    NULL};
  int i;
  boolean ret;
  
  i = 0;
  ret = FALSE;
  while(allowedmsg[i] != NULL)
  {
    if(strncmp(msg, allowedmsg[i], strlen(allowedmsg[i])) == 0)
    {
      ret = TRUE;
    }
    i++;
  }
  
  return TRUE;//ret;
}

