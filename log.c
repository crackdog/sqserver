#include "sqserver.h"
#include "log.h"
#include <time.h>

int timestamp = FALSE;
FILE * logfile = NULL;
char * logdir = NULL;

void setlogdir(const char * logdirectory)
{
  logdir = (char *) malloc(strlen(logdirectory) + 1);
  
  strncpy(logdir, logdirectory, strlen(logdirectory));
}

const char * getlogdir()
{
  return logdir;
}

void initlogfile(logtype lt)
{
  char * ln;
  
  if(lt == SERVER_LOG)
  {
    ln = "server_log_";
  }
  else
  {
    ln = "client_log_";
  }
  
  if(logdir != NULL)
  {
    char logfilename[256];
    int i;
    FILE * tmp;
    
    for(i = 0; i < 100; i++)
    {
      snprintf(logfilename, 256, "%s/%s%i.log", logdir, ln, i);
      tmp = fopen(logfilename, "r");
      if(tmp == NULL)
      {
        break;
      }
    }
    
    logfile = fopen(logfilename, "w");
  }
}

void serverlog(char * str)
{
  if(logfile != NULL)
  {
    if(timestamp)
      printctime();
    
    fprintf(logfile, "%s\n", str);
    
    fflush(logfile);
  }
}

void printctime()
{
  struct tm times;
  time_t rawtime;
  
  time(&rawtime);
  times = *localtime(&rawtime);
  
  fprintf(logfile, "%.2i.%.2i.%.4i %.2i:%.2i:%.2i : ", 
    times.tm_mday, times.tm_mon + 1, times.tm_year + 1900, 
    times.tm_hour, times.tm_min, times.tm_sec);
  
  fflush(logfile);
}

void settimestamp(int t)
{
  timestamp = t;
}

void serverlogwi(char * str1, long x, char * str2)
{
  if(logfile != NULL)
  {
    if(timestamp)
      printctime();
    
    fprintf(logfile, "%s%li%s\n", str1, x, str2);
    
    fflush(logfile);
  }
}

void serverlogsigchld(pid_t pid, int exitstatus, int sig)
{
  if(logfile != NULL)
  {
    if(timestamp)
      printctime();
    
    fprintf(logfile, "process %i exited with %i and signal %i\n" , (int) pid , exitstatus, sig);
    
    fflush(logfile);
  }
}

void terminate_log()
{
  if(logfile != NULL)
  {
    fflush(logfile);
    fclose(logfile);
  }
}

