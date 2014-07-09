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

#include "cpos.h"


static uint8_t build_bitmap(struct isomsg *be,
               uint8_t *buf, size_t buflen)
{
  uint8_t i, t = 0;
  uint8_t bitmap_tmp[128];
  uint8_t b1[64];
  
  memset(bitmap_tmp, '0', 128);
  memset(b1, 0, 64);
  
  for (i = 0; i < 128; i++) {
    if (be[i].data) {
      *(bitmap_tmp + i) = '1';
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

// return 1 if error
uint8_t cpos_build_set_field(struct isomsg *be, const uint8_t bit,
                      const void *data, size_t data_len)
{
  if ((bit <= 1) || (bit > 128))
    return ERROR;
  
  if (!be[bit - 1].data) {
    be[bit - 1].len = data_len;
    be[bit - 1].data = malloc(data_len + 1);
    if (!be[bit -1].data)
      return NOMEM;
    
    memcpy(be[bit - 1].data, data, data_len);  
  } else
    return ERROR;
  
  return OK;
}

// return NULL if header error
uint8_t* cpos_build_msg(struct isofield_cfg *bc, struct isomsg *be,
		           const char *mti, void *buf, size_t blen)
{
  uint8_t bitmap[128];
  uint8_t i, bitmap_len = 0;
  uint8_t flag, format;
  uint16_t len, maxlen;
  void *f;
    
  memset(bitmap, 0, 128);
  memset(buf, 0, blen);

  bitmap_len = build_bitmap(be, &bitmap[0], 128);

  memcpy(buf, mti, 4); // mti
  buf += 4;

  memcpy(buf, bitmap, (bitmap_len / 4)); // bitmap
  buf += (bitmap_len / 4);
  
  for (i = 0; i < ISO_BIT_LEN; i++) {
    if (be[i].data) {
      len = utils_get_field_cfg(bc, i +1, &flag, &maxlen, &format);
      
      if (flag == 0) {// fixed len
    	if (len < be[i].len)
          len = be[i].len;
    	
      	f = utils_fill(be[i].data, be[i].len, len, format);
        memcpy(buf, f, len);
    	        
        buf += len;
      }
      else { // dynamic len
    	if ((maxlen == 0) || (maxlen < be[i].len))
          maxlen = be[i].len;
    	       
    	sprintf((char*) buf, "%0*d", len, maxlen);
        buf += len;
        
        f = utils_fill(be[i].data, be[i].len, maxlen, format);
        memcpy(buf, f, maxlen);
        
        buf += be[i].len;
      }
      
      free(f);
    }
  }
  
  return buf;
}
