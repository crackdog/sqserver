#include "sqserver.h"
#include "log.h"
#include <time.h>

int timestamp = FALSE;
FILE * logfile = NULL;

void initserverlog(const char * logdir)
{
  char logfilename[256];
  int i;
  FILE * tmp;
  
  strncpy(logfilename, logdir, 256);
  strncat(logfilename, "/sqserver.log", 256);

  logfile = fopen(logfilename, "w");

  for(i = 0; i < 100; i++)
  {
    snprintf(logfilename, 256, "%s/%i.log", logdir, i);
    tmp = fopen(logfilename, "r");
    if(tmp == NULL)
    {
      //fclose(tmp);
      fprintf(logfile, "%s is not existing\n", logfilename);
      break;
    }
    fclose(tmp);
  }
  
  logfile = fopen(logfilename, "w");
}

void serverlog(char * str)
{
  if(timestamp)
    printctime();
  
  fprintf(logfile, "%s\n", str);
  
  fflush(logfile);
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
  if(timestamp)
    printctime();
  
  fprintf(logfile, "%s%li%s\n", str1, x, str2);
  
  fflush(logfile);
}

void serverlogsigchld(pid_t pid, int exitstatus, int sig)
{
  if(timestamp)
    printctime();
  
  fprintf(logfile, "process %d exited with %i and signal %i\n" , pid , exitstatus, sig);
  
  fflush(logfile);
}

void terminate_log()
{
  fflush(logfile);
  fclose(logfile);
}

