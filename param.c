#include "sqserver.h"
#include "param.h"

const char * param_list[] = {"", "-e", "-d", "-g", "-be", "-bd", "-s"};

cmd_param getParam(int argc, const char * argv[])
{
  cmd_param i;
  
  if(argc < 2)
  {
    return NOPARAM;
  }
  else
  {
    for(i = NOPARAM + 1; i < PARAM_END; i++)
    {
      if(strncmp(argv[1], param_list[i], strlen(param_list[i])) == 0)
        return i;
    }
  }
  return NOPARAM; 
}

