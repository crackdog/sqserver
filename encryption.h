#ifndef _ENCRYPTION_H_
#define _ENCRYPTION_H_

//typedefs
typedef struct {
  uint8_t * data;
  size_t len;
} binarydata;

//for intern usage:
extern void xor_crypt(void * buffer, size_t length);
extern char * base64encode(const void * databuf, size_t datalen);
extern binarydata base64decode(const char * databuf);

//for extern usage:
//binarydata encryptionkey = { NULL, 0 };
extern void setKey(const char * base64key); //set the encryptionkey
extern binarydata getKey(void);

extern char * loadKey(const char * filename); //returns base64key
extern char * generateKey(size_t len); //generates base64key
extern char * encrypt_msg(const char * msg); //returns a zero-terminated string
extern char * decrypt_msg(const char * encrypted_msg); //returns a zero-terminated string

#endif

