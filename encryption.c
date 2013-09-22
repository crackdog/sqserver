#include "sqserver.h"
#include "encryption.h"
//#include "log.h"

binarydata key = {NULL, 0};

void setKey(const char * base64key)
{
  key = base64decode(base64key);
}

binarydata getKey()
{
  return key;
}

char * loadKey(const char * filename)
{
  FILE * keyfile;
  char * newkey;
  size_t keylen;
  
  keyfile = fopen(filename, "r");
  if(keyfile == NULL)
  {
    return NULL;
  }
  else
  {
    fseek(keyfile, 0, SEEK_END);
    keylen = ftell(keyfile) + 1;
    fseek(keyfile, 0, SEEK_SET);
    newkey = (char *) malloc(keylen * sizeof(char));
    if(fgets(newkey, keylen, keyfile) != NULL)
    {
      newkey[keylen-1] = '\0';
    }
    else
    {
      newkey = NULL;
    }
    return newkey;
  }
}

char * generateKey(size_t len)
{
  char * newb64key;
  char * binkey;
  size_t i;
  
  binkey = (char *) malloc(len + sizeof(char));
  
  srand(time(NULL));
  
  for(i = 0; i < len; i++)
  {
    binkey[i] = rand() % 0xff;
  }
  
  newb64key = base64encode(binkey, len);//const void * databuf, size_t datalen);
  
  free(binkey);
  
  return newb64key;
}

char * encrypt_msg(const char * msg)
{
  size_t msglen;
  char * tmpmsg;
  char * retmsg;
  
  msglen = strlen(msg) + 1;
  
  tmpmsg = (char *) malloc(msglen * sizeof(char));
  strncpy(tmpmsg, msg, msglen);
  tmpmsg[msglen-1] = '\0';
  
  xor_crypt(tmpmsg, msglen-1);
  retmsg = base64encode(tmpmsg, msglen);
  free(tmpmsg);
  
  return retmsg;
}

char * decrypt_msg(const char * encrypted_msg)
{
  size_t msglen;
  char * msg;
  binarydata cryptmsg;
  
  cryptmsg = base64decode(encrypted_msg);
  
  msglen = cryptmsg.len + 1;
  msg = (char *) malloc(msglen * sizeof(char));
  memcpy(msg, cryptmsg.data, cryptmsg.len);
  msg[msglen-1] = '\0';
  
  free(cryptmsg.data);
  
  xor_crypt(msg, msglen-1);
  return msg;
}

void xor_crypt(void * buffer, size_t length)
{
  char * cbuffer;
  size_t i, j;
  
  if(key.data != NULL && key.len != 0)
  {
    cbuffer = (char *) buffer;
    
    for(i = 0, j = 0; i < length; i++, j++)
    {
      if(j >= key.len)
      {
        j = 0;
      }
      cbuffer[i] ^= key.data[j];
    }
  }
}

//char * base64encode(const void * databuf, size_t datalen)
//{
  














