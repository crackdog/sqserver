#ifndef _LOG_H_
#define _LOG_H_

#include "sqserver.h"

//private:
extern void printctime(void);
extern void settimestamp(int t);

//public
extern void setlogdir(const char * logdirectory);
extern void serverlog(char * str);
extern void serverlogwi(char * str1, long x, char * str2);
extern void terminate_log(void);
extern void serverlogsigchld(pid_t pid, int exitstatus, int sig);
extern void initlogfile(logtype lt);
extern const char * getlogdir(void);

#endif
