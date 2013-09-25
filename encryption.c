#include "sqserver.h"
#include "encryption.h"
//#include "log.h"

static binarydata key = {NULL, 0};

const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const unsigned char decodeb64[] = {
    66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
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
  size_t i, j, padc, databuflen;
  uint32_t x, buffer;
  char y;
  
  int xyz;
  
  databuflen = strlen(databuf);
  
  padc = 0;
  if(databuf[databuflen-1] == '=')
  {
    padc++;
    if(databuf[databuflen-2] == '=')
    {
      padc++;
    }
  }
  
  result.len = ((strlen(databuf) * 3) / 4) - padc;
  result.len = strlen(databuf);
  result.data = (uint8_t *) malloc(result.len * sizeof(uint8_t));
  
  x = 0;
  buffer = 1;
  xyz = 0;
  
  for(i = 0, j = 0; i < databuflen; i++)
  {
    y = decodeb64[databuf[i]];
    
    //printf("i = %i : ", (int) i);
    
    switch(y)
    {
      case INVALID:
        result.data = NULL;
        result.len = 0;
        return result;
      case EQUAL:
        i = databuflen;
        while(j < result.len)
        {
          result.data[j++] = '\0';
        }
        break;
      case WHITESPACE:  
        continue;
      default:
        buffer = (buffer << 6);
        //printf("buffer = %x , y = %x\n", buffer, y);
        buffer |= y;
        //if(buffer & 0x1000000)
        if(xyz < 4)
        {
          result.data[j++] = (buffer >> 16) & 0xff;
          result.data[j++] = (buffer >> 8) & 0xff;
          result.data[j++] = buffer & 0xff;
          
          //printf("%x\n", buffer & 0x1000000);
          
          buffer = 1;
          xyz = 0;
        }
        else
        {
          xyz++;
        }
        
        //printf("%i '%c' buffer = %x\n", y, databuf[i], (int) buffer);
    }
  }
  
  if(buffer & 0x40000)
  {
    result.data[j++] = (buffer >> 10) & 0xff;
    result.data[j++] = (buffer >> 2) & 0xff;
  }
  else if(buffer & 0x1000)
  {
    result.data[j++] = (buffer >> 4) & 0xff;
  }
  
  //printf("\n");
  
  return result;
}

