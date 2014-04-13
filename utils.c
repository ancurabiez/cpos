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

#include "cpos.h"



// return 1 if error
// return 2 if file cfg error
struct isofield_cfg* cpos_init(const char *cfg_file)
{
  FILE *f;
  char buf[16];
  uint16_t i = 0;
  struct isofield_cfg *bc;
  
  bc = calloc(ISO_BIT_LEN, sizeof(struct isofield_cfg));
  if (!bc)
    return NULL;
  
  f = fopen(cfg_file, "r");
  if (!f) {
    free(bc);
    return NULL;
  }
  
  while (fgets(buf, sizeof(buf), f)) {
    if (buf[0] != '{')
      continue;
    
    bc[i].flag =  atoi(buf +1);
    bc[i].len = atoi(buf + 3);
        
    i++;
  }
  
  fclose(f);
  return bc;
}

void cpos_close(struct isofield_cfg *bc)
{
  free(bc);
}

inline uint16_t utils_get_field_cfg(struct isofield_cfg *bc,
		                              const uint8_t bit, uint8_t *flag)
{
  uint16_t len = 0;
  
  len = (*(bc + (bit -1))).len;
  *flag = (*(bc + (bit -1))).flag;
  
  return len;
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
