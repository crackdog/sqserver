#include "sqserver.h"
#include "daemon.h"
#include "encryption.h"
#include "param.h"
#include "server.h"
#include "log.h"

int main(int argc, const char * argv[])
{
  int returnvalue;
  
  settimestamp(TRUE);
  set_sigchld_handler();  
  
  returnvalue = 0;
  
  switch(getParam(argc, argv))
  {
    case ENCRYPT:
    {
      //encode
      char buffer[BUF_SIZE];
      char * result;
      unsigned int i;
      
      setKey(loadKey("test.key"));
      
      while(!feof(stdin))
      {
        if(fgets(buffer, BUF_SIZE, stdin) == 0)
          break;
        buffer[strlen(buffer)] = '\0';
        printf("%s\n", encrypt_msg(buffer));
      }
      
      break;
    }
    case DECRYPT:
    {
      char buffer[BUF_SIZE];
      
      setKey(loadKey("test.key"));
      
      while(!feof(stdin))
      {
        if(fgets(buffer, BUF_SIZE, stdin) == 0)
          break;
        printf("%s\n", decrypt_msg(buffer));
      }
      
      break;
    }  
    case GENERATE_KEY:
    {
      //generate key
      unsigned int len;
      
      if(argc == 3)
        sscanf(argv[2], "%i", &len);
      else
        len=64;
      
      printf("%s\n", generateKey(len));
      
      break;
    }
    case BASE64ENCODE:
    {
      char buffer[BUF_SIZE];
      
      while(!feof(stdin))
      {
        if(fgets(buffer, BUF_SIZE, stdin) == 0)
          break;
        printf("%s\n", base64encode(buffer, strlen(buffer)));
      }
      
      break;
    }
    case BASE64DECODE:
    {
      char buffer[BUF_SIZE];
      
      while(!feof(stdin))
      {
        if(fgets(buffer, BUF_SIZE, stdin) == 0)
          break;
        printf("%s\n", base64decode(buffer).data);
      }
      
      break;
    }
    case SERVER:
    {
      daemonize();
    
      if(argc > 2)
      {
        initserverlog(argv[2]);
      }
      else
      {
        initserverlog("~/logs");
      }
      
      serverlog("server started");
      //sleep(60); //debug...
      //returnvalue = start_server();
      serverlog("server stopped");
      //terminate_log();
      break;
    }
    case NOPARAM:
    default:
    {
      //print usage:
      printf("usage: %s <option>\noptions: [ -g <length> | -d | -e ]\n", argv[0]);
      break;
    }
  }
  
  return 0;
}

