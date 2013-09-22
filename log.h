#ifndef _LOG_H_
#define _LOG_H_

//public:
extern void serverlog(char * str);
extern void serverlogwi(char * str1, long x, char * str2);
extern void serverlogsigchld(pid_t pid, int exitstatus, int sig);
extern void initserverlog(const char * logdir);
extern void terminate_log(void);

//private:
extern void printctime(void);
extern void settimestamp(int t);

#endif
