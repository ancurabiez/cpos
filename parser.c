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

/// Return 1 if error
/// f flag [0 check] [1 get]
static uint8_t parser_check_get_data(struct isofield_cfg *bc, const uint8_t *msg,
		                      const uint8_t *bitmap, const uint8_t f,
		                      uint8_t *buf, const uint16_t blen,
		                      const uint8_t bit)
{
  uint8_t i = 1, flag = 0, bitm = 64;
  uint16_t len = 0;
  uint8_t t[4];
  
  memset(t, 0, sizeof(t));
  memset(buf, 0, blen);
  
  msg += 4; // skip mti
  msg += 16; // skip primary bitmap
  
  if (*bitmap == '1') { // skip secondary bitmap jika ada
    msg += 16;
    bitm += 64;
  }

  for (; i < bitm; i++) {
    if (*(bitmap + i) == '1') {
      len = utils_get_field_cfg(bc, i + 1, &flag);

      if (!flag) { //fixed length
        if (((i +1) == bit) && f) {
          memcpy(buf, msg, len);
          *(buf + len) = '\0';
          
          break;
        }
        
        msg += len;
      } else {
        // get data length
        memcpy(t, msg, len);
        t[len] = '\0';
        msg += len;
        len = atoi((char*) t);
        
        if (((i +1) == bit) && f) {
          memcpy(buf, msg, len);
          *(buf + len) = '\0';
          
          break;
        }
        
        msg += len;
      } 
    }
  }

  if (!f) {
    if (*msg != '\0')
      return 1;
  } else {
    if (!(*buf))
      return 1;
  }
    
  return 0;
}

//return NULL if error
struct isomsg* cpos_parse_init()
{
  struct isomsg *imsg;
  
  imsg = calloc(ISO_BIT_LEN, sizeof(struct isomsg));
  if (!imsg)
    return NULL;
  
  return imsg;
}

void cpos_parse_free(struct isomsg *imsg)
{
  uint8_t i;
  
  for (i = 0; i < ISO_BIT_LEN; i++)
    if (imsg[i].data)
      free(imsg[i].data); 
  
  free(imsg);
}

// return 1 if error
uint8_t cpos_parse(struct isofield_cfg *bc, struct isomsg *imsg,
		                uint8_t *msg)
{
  uint8_t bitmap[128];
  uint8_t buf[1024];
  uint16_t i;
   
  memset(bitmap, 0, 128);

  if (parser_get_bitmap(msg, 0, bitmap, sizeof(bitmap)))
    return 1;
  if (*(bitmap) == '1') // get sec bitmap if any
    if (parser_get_bitmap(msg, 1, bitmap + 64, sizeof(bitmap)))
      return 1;

  for (i = 0; i < ISO_BIT_LEN; i++) {
    if (!parser_check_get_data(bc, msg, &bitmap[0], 1, 
                   buf, sizeof(buf), i)) {
      imsg[i].data = strdup((char*) buf);
    } else {
      imsg[i].data = NULL;
    }
  }
  
  return 0;
}
