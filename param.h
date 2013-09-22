#ifndef _PARAM_H_
#define _PARAM_H_

typedef enum {
  NOPARAM,
  ENCRYPT,
  DECRYPT,
  GENERATE_KEY,
  BASE64ENCODE,
  BASE64DECODE,
  SERVER,
  PARAM_END
} cmd_param;

extern cmd_param getParam(int argc, const char * argv[]);

#endif

