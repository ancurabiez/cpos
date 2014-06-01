/*
**  Author:
**     Lulus Wijayakto  <l.wijayakto@yahoo.com>
**
**  License: LGPL
**
**  27022013
*/

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <ctype.h>

#include "cpos.h"


static const char const *format[] = {"LPS", "RPS", "LPZ", "RPZ", "NONE"};


char* trim(const char* str)
{
  char *buf;
  int len;
  
  len = strlen(str);
  
  buf = malloc(len + 1);
  if (!buf)
    return NULL;
    
  while (*str) {
    if (!isalnum(*str)) {
      str++;
      len--;
      continue;
    }
    
    break;
  }
  
  len--;
  while (len) {
    if (!isalnum(*(str + len))) {
      len--;
      continue;
    }
    
    break;
  }
  
  len++;
 
  memcpy(buf, str, len);
  buf[len] = '\0';
  
  return buf;
}

static uint8_t get_format(const char *str)
{
  uint8_t i;
  
  for (i = 0; i < 5; i++) {
    if (!strcmp(str, format[i]))
      return i;
  }
  
  return 10;
}

static inline void error__(uint8_t line, uint8_t opt,
                          struct isofield_cfg **bc)
{
  free(*bc);
  *bc = NULL;
  
  fprintf(stderr, "Error configuration line %d:%d\n", line, opt);
}

struct isofield_cfg* cpos_init(const char *cfg_file)
{
  FILE *f;
  char buf[256];
  uint8_t i = 0, err = 0, x;
  uint16_t tokencount, line = 0;
  long l;
  struct isofield_cfg *bc;
  char *ptr, *ptr1;
  char *str, *token, *subtoken, *tokenval;
  
  bc = calloc(ISO_BIT_LEN, sizeof(struct isofield_cfg));
  if (!bc)
    return NULL;
  
  f = fopen(cfg_file, "r");
  if (!f) {
    free(bc);
    return NULL;
  }
  
  while (fgets(buf, sizeof(buf), f)) {
	line ++;
	char *bufstr = buf;
    
	if (buf[0] != '{')
      continue;
	
	bufstr++;
	tokencount = 0;
	x = 0;
	
	token = strtok_r(bufstr, "}", &ptr);
	for (str = token; ; str = NULL) {
	  tokencount++;	
		
      subtoken = strtok_r(str, ",", &ptr);
      if (!subtoken)
        break;
           
      tokenval = trim(subtoken);
      	
      if (tokencount == 1) {
	    if (!strcmp(tokenval, "LLVAR")) {
          bc[i].flag = 1;
          bc[i].len = 2;
          x = 1;
        }
        else if (!strcmp(tokenval, "LLLVAR")) {
          bc[i].flag = 1;
          bc[i].len = 3;
          x = 1;
        }
        else {
          l = strtol(tokenval, &ptr1, 10);
          
          if (l > 999) {
	        error__(line, tokencount, &bc);
            err = 1;
            break;
          }
          
          if (*ptr1 !=  '\0') {
            error__(line, tokencount, &bc);
            err = 1;
            break;
          }
                   
          bc[i].flag = 0;
          bc[i].len = l;
        }
      }
      else if (tokencount == 2) {
        l = strtol(tokenval, &ptr1, 10);
        
        if (*ptr1 != '\0') {
          error__(line, tokencount, &bc);
          err = 1;
          break;
        }
        
    	if (l > 999) {
          error__(line, tokencount, &bc);
          err = 1;
          break;
    	}
    	
    	if ((l == 0) && (x == 1)) {
          error__(line, tokencount, &bc);
          err = 1;
          break;
    	}
    	
    	bc[i].maxlen = l;
      }
      else if (tokencount == 3) {
    	bc[i].format = get_format(tokenval);
        if ((bc[i].format == 10) ||
            ((bc[i].format == NONE) && (x == 1))) {
          error__(line, tokencount, &bc);
          err = 1;
          break;
        }
      }
      
      free(tokenval);
    }
	
    i++;
    
    if (err) {
      free(tokenval);
      break;
    }
  }
  
  fclose(f);
  return bc;
}

void cpos_close(struct isofield_cfg *bc)
{
  free(bc);
}

//return NULL if error
struct isomsg* cpos_msg_new()
{
  struct isomsg *imsg;
  
  imsg = calloc(ISO_BIT_LEN, sizeof(struct isomsg));
  if (!imsg)
    return NULL;
  
  return imsg;
}

void cpos_msg_free(struct isomsg *imsg)
{
  uint8_t i;
  
  for (i = 0; i < ISO_BIT_LEN; i++)
    if (imsg[i].data)
      free(imsg[i].data); 
  
  free(imsg);
}

inline uint16_t utils_get_field_cfg(struct isofield_cfg *bc,
                const uint8_t bit, uint8_t *flag,
                uint16_t *maxlen, uint8_t *format)
{
  uint16_t len = 0;
  
  len = (*(bc + (bit -1))).len;
  *flag = (*(bc + (bit -1))).flag;
  *maxlen = (*(bc + (bit -1))).maxlen;
  *format = (*(bc + (bit -1))).format;
  
  return len;
}

void* fill(const void* data, uint16_t datalen,
		    uint16_t maxlen, uint8_t format)
{
  void *buf = NULL;
  
  buf = malloc(maxlen + 1);
  if (!buf)
    return NULL;

  if ((format == LPS) || (format == RPS))
    memset(buf, ' ', maxlen);
  else if ((format == LPZ) || (format == RPZ))
    memset(buf, '0', maxlen);
  
  if ((format == LPS) || (format == LPZ)) {
    memcpy(buf + (maxlen - datalen), data, datalen);
    memset(buf + maxlen, '\0', 1);
  } else if ((format == RPS) || (format == RPZ)) {
    memcpy(buf, data, datalen);
    memset(buf + maxlen, '\0', 1);
  } else {
    memset(buf, '\0', maxlen);
    memcpy(buf, data, datalen);
  }
   
  return buf;
}

/// Return NULL if error
uint8_t* utils_hex2bin(const uint8_t *hex, uint8_t *bin, size_t blen)
{
  if (blen < 8)
    return NULL;
  
  uint8_t bit = 0, t = 0;
  uint8_t res_hex = 0;
  char *ptr = NULL;
  
  res_hex = strtoul((char *) hex, &ptr, 16);
  if (strcmp(ptr, "")) //error
    return NULL;
  
  for (bit = 1 << (CHAR_BIT -1); bit; bit >>= 1) {
    if (res_hex & bit)
      *(bin + t) = '1';
    else
      *(bin + t) = '0';
    
    t++;
  }
  
  *(bin + t) = '\0';
  
  return bin;
}

uint8_t* utils_bin2hex(const uint8_t *binary, uint8_t *hexa,
                       size_t hlen_)
{
  if (hlen_ <= 32)
    return NULL;
  
  uint8_t len = 0, count = 0, sum = 0, hlen = 0, c = 0;
  char hextmp[64];
  
  memset(hextmp, 0, 64);
  
  len = strlen((char*) binary);
  hlen = len / 4;

  if ((len % 4) != 0)
    hlen++;
  
  c = len;
  
  while (c > 0) {
    count = 0;
    sum = 0;
    
    while (count < 4) {
      c--;
      
      if (*(binary + c) == '1')
        sum += pow(2, count);
     
      count++;
    }
    
    hlen--;
    
    if (sum < 10)
      *(hextmp + hlen) = 48 + sum;
    else
      *(hextmp + hlen) = 55 + sum;
  }

  memcpy(hexa, hextmp, strlen(hextmp)); 

  return hexa;
}
