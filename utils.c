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


static struct bit_cfg__ *ELEMENT_CFG;


// return 1 if error
// return 2 if file cfg error
uint8_t cpos_init(const char *cfg_file)
{
  FILE *f;
  char buf[16];
  uint16_t i = 0;
  
  ELEMENT_CFG = calloc(ISO_BIT_LEN, sizeof(struct bit_cfg__));
  if (!ELEMENT_CFG)
    return 1;
  
  f = fopen(cfg_file, "r");
  if (!f) {
    free(ELEMENT_CFG);
    return 2;
  }
  
  while (fgets(buf, sizeof(buf), f)) {
    if (buf[0] != '{')
      continue;
    
    ELEMENT_CFG[i].flag =  atoi(buf +1);
    ELEMENT_CFG[i].len = atoi(buf + 3);
        
    i++;
  }
  
  fclose(f);
  return 0;
}

void cpos_close()
{
  free(ELEMENT_CFG);
}

inline uint16_t utils_get_element_cfg(const uint8_t bit, uint8_t *flag)
{
  uint16_t len = 0;
  
  len = (*(ELEMENT_CFG + (bit -1))).len;
  *flag = (*(ELEMENT_CFG + (bit -1))).flag;
  
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
