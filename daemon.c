#include "sqserver.h"
#include "daemon.h"
#include "log.h"

int daemonize(void)
{
  //make to daemon...
  //printf("daemonize();\n");
  
  pid_t pid;
  int i;
  
  pid = fork();
  if(pid < 0)
  {
    perror("forkerror");
    exit(0);
  }
  
  if(pid != 0)
  {
    printf("%i\n", (int) pid);
    exit(0);
  }
  
  if(setsid() == -1)
  {
    exit(-1);
  }
  
  if(chdir("/") == -1)
  {
    exit(-1);
  }
  
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  
  open("/dev/null", O_RDWR);
  i = dup(STDIN_FILENO);  //"i = " to delete the warning for unused result
  i = dup(STDIN_FILENO);
    
  return 0;
}

void set_sigchld_handler()
{
  struct sigaction sa;
    
  //signal handler
  sa.sa_handler = sigchld_handler;
  sa.sa_flags = SA_RESTART;
  sigaction ( SIGCHLD , & sa , NULL );
  
  sa.sa_handler = sigterm_handler;
  sa.sa_flags = SA_RESTART;
  sigaction(SIGTERM, &sa, NULL);
  
  sa.sa_handler = sigterm_handler;
  sa.sa_flags = SA_RESTART;
  sigaction(SIGINT, &sa, NULL);
}

void sigterm_handler(int sig)
{
  //serverlog("Server terminated");
  terminate_server();
  terminate_log();
  exit(0);
}

void sigchld_handler(int sig)
{
  int status ;
  pid_t pid ;
  
  //get pid and status
  pid = wait(&status);

  if(WIFEXITED(status))
  {  
    //printf ("Process %d exited with %i and signal %i\n" , pid , WEXITSTATUS(status), sig);
    serverlogsigchld(pid, WEXITSTATUS(status), sig);
  }
}

