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
#include <sys/stat.h>

#include "cpos.h"


static uint8_t build_bitmap(struct build_msg *be,
               uint8_t *buf, size_t buflen)
{
  uint8_t i, t = 0;
  uint8_t bitmap_tmp[128];
  uint8_t b1[64];
  
  memset(bitmap_tmp, '0', 128);
  memset(b1, 0, 64);
  
  for (i = 0; i < 128; i++) {
    if (be[i].bit != 0) {
      *(bitmap_tmp + (be[i].bit -1)) = '1';
      t = i;
    }
  }
  
  i = 64;

  if (t > 64) { // jika bitmap lebih dari 64 maka enable bit 1
    *bitmap_tmp = '1';
    i = 128;
  }

  *(bitmap_tmp + i) = '\0';

  utils_bin2hex(&bitmap_tmp[0], b1, sizeof(b1));
  memcpy(buf, b1, sizeof(b1));

  i = (uint8_t) (strlen((char*) b1) * 4);

  return i;
}

//return NULL if error
struct build_msg* build_init(void)
{
  struct build_msg *be;
  
  be = (struct build_msg*) calloc(ISO_BIT_LEN, sizeof(struct build_msg));
  if (!be)
    return NULL;
  
  return be;
}

void build_free(struct build_msg *be)
{
  uint8_t i;
  
  for (i = 0; i < ISO_BIT_LEN; i++) {
    if (be[i].bit != 0)
      free(be[i].data); 
  }
  
  free(be);
}

// return 1 if error
uint8_t build_element(struct build_msg *be, const uint8_t bit,
                      const void *data, size_t data_len)
{
  if ((bit <= 1) || (bit > 128))
    return 1;
  
  if (be[bit - 1].bit == 0) {
    be[bit - 1].bit = bit;
    be[bit - 1].data_len = data_len;
    be[bit - 1].data = malloc(data_len + 1);
    memcpy(be[bit - 1].data, data, data_len);  
  } else
    return 1;
  
  return 0;
}

// return NULL if header error
uint8_t* build_msg(struct build_msg *be, const char *mti,
                   void *buf, size_t blen, uint8_t header)
{
  uint8_t bitmap[128];
  uint8_t i, bitmap_len = 0;
  uint8_t flag, len;
  void *tmp;
  
  memset(bitmap, 0, 128);
  memset(buf, 0, blen);

  tmp = buf;
  bitmap_len = build_bitmap(be, &bitmap[0], 128);

  memcpy(buf, mti, 4); // mti
  buf += 4;

  memcpy(buf, bitmap, (bitmap_len / 4)); // bitmap
  buf += (bitmap_len / 4);
  
  for (i = 0; i < ISO_BIT_LEN; i++) {
    if (be[i].bit != 0) {
      len = utils_get_element_cfg(i +1, &flag);
      
      if (flag == 0) {// fixed len
        memcpy(buf, be[i].data, len);
        buf += len;
      }
      else { // dynamic len
        sprintf((char*) buf, "%0*d", len, be[i].data_len);
        buf += len;
        
        memcpy(buf, be[i].data, be[i].data_len);
        buf += be[i].data_len;
      }
    }
  }
  
  if (header) {
    char *bb;
    
    len = strlen((char*) tmp);
    
    bb = malloc(len + 8);
    if (!bb)
      return NULL;
    
    memset(bb, 0, len + 8);
        
    sprintf(bb, "%0*d%s", 4, len, (char*)tmp);
    buf -= len;
    memcpy(buf, bb, len + 4);
    
    free(bb);
  }
  
  return buf;
}
