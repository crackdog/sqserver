#include "sqserver.h"
#include "encryption.h"
//#include "log.h"

static binarydata key = {NULL, 0};

const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned char decodeb64[] = {
    66,66,66,66,66,66,66,66,66,64,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

#define WHITESPACE 64
#define EQUAL      65
#define INVALID    66



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
  
  tmpmsg = (char *) malloc(msglen);
  strncpy(tmpmsg, msg, msglen);
  tmpmsg[msglen-1] = '\0';
  
  xor_crypt(tmpmsg, msglen);
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
  
  msglen = cryptmsg.len;
  msg = (char *) malloc(msglen + 1);
  memcpy(msg, cryptmsg.data, cryptmsg.len);
  msg[msglen] = '\0';
  
  free(cryptmsg.data);
  
  xor_crypt(msg, msglen);
  
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

char * base64encode(const void * databuf, size_t datalen)
{
  const uint8_t * data;
  char * result;
  size_t resultlen;
  size_t i, j;
  uint8_t x0, x1, x2, x3;
  uint32_t x;
  
  //datalen--;
  
  data = (const uint8_t *) databuf;
  resultlen = (((datalen + (datalen % 3)) / 3) * 4) + 1;
  
  result = (char *) malloc(resultlen * sizeof(char));
  
  for(i=0, j=0; i < datalen; i+=3)
  {
    //saving the 3 bytes into an 32 bit number
    x = data[i] << 16;
    
    if(i+1 < datalen)
    {
      x += data[i+1] << 8;
    }
    if(i+2 < datalen)
    {
      x += data[i+2];
    }
    
    //seperate into 4 6bit numbers
    x0 = (uint8_t) (x >> 18) & 63;
    x1 = (uint8_t) (x >> 12) & 63;
    x2 = (uint8_t) (x >> 6) & 63;
    x3 = (uint8_t) x & 63;
    
    result[j++] = base64chars[x0];
    result[j++] = base64chars[x1];
    
    if(i+1 < datalen)
    {
      result[j++] = base64chars[x2];
    }
    else
    {
      result[j++] = '=';
    }
    
    if(i+2 < datalen)
    {
      result[j++] = base64chars[x3];
    }
    else
    {
      result[j++] = '=';
    }
  }
  
  result[j] = '\0';
  return result;
}

binarydata base64decode(const char * databuf)
{
  binarydata result;
  uint8_t * buffer;
  size_t datalen, padcount;
  size_t dataindex, bufferindex;
  uint32_t x;
  uint8_t nextByte;
  int i;
  
  datalen = strlen(databuf);
  
  buffer = (uint8_t *) malloc(datalen);
  
  padcount = 0;
  
  for(dataindex = 0, i = 0, bufferindex = 0, x = 0; dataindex < datalen; dataindex++)
  {
    nextByte = decodeb64[databuf[dataindex]];
    
    if(nextByte == INVALID)
    {
      //printf("invalid: '%c' (%i)\n", databuf[dataindex], databuf[dataindex]);
      //continue;
      exit(1337);
    }
    
    if(nextByte != WHITESPACE)
    {
      if(nextByte == EQUAL || databuf[dataindex] == '=')
      {
        nextByte = 0x0;
        padcount++;
      }
      
      x <<= 6;
      x |= nextByte;
      
      i++;
      
      if(i >= 4)
      {
        buffer[bufferindex++] = (x >> 16) & 0xff;
        buffer[bufferindex++] = (x >> 8) & 0xff;
        buffer[bufferindex++] = x & 0xff;
        
        i = 0;
        x = 0;
      }
    }
  }
  
  bufferindex -= padcount;
  
  buffer[bufferindex] = '\0';
  
  result.len = bufferindex;
  result.data = malloc(result.len);
  memcpy(result.data, buffer, result.len);
  
  free(buffer);
  
  return result;
}

