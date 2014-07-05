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


/// Return 1 if error
/// flag : 0 primary bitmap
///        1 secondary bitmap
static uint8_t parser_get_bitmap(const uint8_t *msg, const uint8_t flag,
                          uint8_t *bitmap, const uint16_t blen)
{
  uint8_t f[3];
  uint8_t bin[8];
  uint8_t i = 0, t = 0;
  uint8_t bitmap_tmp[64];
  
  memset(f, 0, sizeof(f));
  memset(bin, 0, sizeof(bin));
  memset(bitmap_tmp, 0, sizeof(bitmap_tmp));
  
  if (!flag) //primary
    msg += 4;
  else
    msg += 20;

  for (i = 0; i < 16; i += 2) {
    memcpy(f, msg, 2);
    f[2] = '\0';

    if (!utils_hex2bin(f, bin, sizeof(bin)))
      return 1;
    
    memcpy(bitmap_tmp + t, bin, 8);
    
    t += 8;
    msg += 2;
  }
  
  memcpy(bitmap, bitmap_tmp, 64);
  return 0;
}

static uint8_t parser_check_get_data(struct isofield_cfg *bc, const uint8_t *msg,
                           const uint8_t *bitmap, const uint8_t f, uint8_t *buf)
{
  uint8_t i = 1, flag = 0, bitm = 64, format;
  uint16_t len = 0, maxlen;
  uint8_t t[4];
  uint16_t count = 0;
  uint8_t *p = buf;
  
  memset(t, 0, sizeof(t));
  
  msg += 4; // skip mti
  msg += 16; // skip primary bitmap
  
  if (*bitmap == '1') { // skip secondary bitmap jika ada
    msg += 16;
    bitm += 64;
  }

  for (; i < bitm; i++) {
    if (*(bitmap + i) == '1') {
      len = utils_get_field_cfg(bc, i + 1, &flag, &maxlen, &format);

      if (!flag) { //fixed length
        if (f) {
          *(p + count) = i + 1;
          count += 1;
          
          memcpy(p + count, msg, len);
          count += len;
          
          memset(p + count, 0, 1);
          count += 1;
        }
        
        msg += len;
      } else {
        // get data length
        memcpy(t, msg, len);
        t[len] = '\0';
        msg += len;
        len = atoi((char*) t);
        
        if (f) {
          *(p + count) = i + 1;
          count += 1;
          
          memcpy(p + count, msg, len);
          count += len;
          
          memset(p + count, 0, 1);
          count += 1;
        }
        
        msg += len;
      } 
    }
    
    if (f) {
      if (i == (bitm -1)) {
        if (*msg == '\0')
          return 0;
      } else {
        if (*msg == '\0')
          return 1;
      }
    }
  }

  if (!f) {
    if (*msg != '\0')
      return 1;
  }
    
  return 0;
}

uint8_t cpos_parse(struct isofield_cfg *bc, struct isomsg *imsg,
                   uint8_t *msg, uint16_t msglen)
{
  uint8_t bitmap[128];
  uint16_t len, c;
  uint8_t *buf;
   
  memset(bitmap, 0, 128);

  if (parser_get_bitmap(msg, 0, bitmap, sizeof(bitmap)))
    return 1;
  if (*(bitmap) == '1') // get sec bitmap if any
    if (parser_get_bitmap(msg, 1, bitmap + 64, sizeof(bitmap)))
      return 1;
  
  buf = malloc(msglen + 1);
  if (!buf)
    return 1;

  memset(buf, 0, msglen + 1);
  
  if (!parser_check_get_data(bc, msg, &bitmap[0], 1, buf))
    return 1;
  
  uint8_t *p = buf;
  
  len = 0;
  while (*p) {
    c = *p;
    p++;
    
    len = strlen((char*) p);
    imsg[c].data = malloc(len + 1);
    if (!imsg[c].data)
      return 1;
	        
    memcpy(imsg[c].data, p, len + 1);
    imsg[c].len = len;
	      
    p += (len +1);
  }
  
  free(buf);
  
  return 0;
}
