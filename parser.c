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
  uint8_t bin[9];
  uint8_t i = 0, t = 0;
  uint8_t bitmap_tmp[65];
  
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
      return CPOS_ERROR;
    
    memcpy(bitmap_tmp + t, bin, 8);
    
    t += 8;
    msg += 2;
  }
  
  memcpy(bitmap, bitmap_tmp, 64);
  return CPOS_OK;
}

static uint8_t parser_check_get_data(struct isofield_cfg *bc,
                  const uint8_t *msg, uint16_t msglen, const uint8_t *bitmap,
                  struct isomsg *imsg)
{
  uint8_t i = 1, flag = 0, bitm = 64, format;
  uint16_t len = 0, maxlen, count = 0;
  uint8_t t[4];
  uint8_t buf[1024];
  
  memset(t, 0, sizeof(t));
  
  msg += 4; // skip mti
  msg += 16; // skip primary bitmap
  count += 20;

  if (*bitmap == '1') { // skip secondary bitmap jika ada
    msg += 16;
    count += 16;
    bitm += 64;
  }

  for (; i < bitm; i++) {
    if (*(bitmap + i) == '1') {
      len = utils_get_field_cfg(bc, i + 1, &flag, &maxlen, &format);
      //printf("OOO [%d] [%d] [%s]\n", i + 1, len, msg);

      if (!flag) { //fixed length
        count += len;
          
        memcpy(buf, msg, len);
        buf[len] = '\0';
          
        imsg[i + 1].data = strdup((char*) buf);
        if (!(imsg[i + 1].data))
          return CPOS_NOMEM;
                   
        imsg[i + 1].len = strlen((char*) imsg[i + 1].data);
        if (imsg[i + 1].len != len)
          return CPOS_LENERR;
        
        msg += len;
      } else {
        count += len;
   
        // get data length
        memcpy(t, msg, len);
        t[len] = '\0';
        msg += len;
        len = atoi((char*) t);
        
        count += len;
        
        memcpy(buf, msg, len);
        buf[len] = '\0';
 
        imsg[i + 1].data = strdup((char*) buf);
        if (!(imsg[i + 1].data))
          return CPOS_NOMEM;
                   
        imsg[i + 1].len = strlen((char*) imsg[i + 1].data);
        if (imsg[i + 1].len != len)
          return CPOS_LENERR;
        
        msg += len;
      }
    }
  }
  
  if (count != msglen)
    return CPOS_ERROR;
  else if (count == msglen) {
    if (*msg == '\0')
      return CPOS_OK;
  }
  
  return CPOS_ERROR;
}

uint8_t cpos_parse(struct isofield_cfg *bc, struct isomsg *imsg,
                   uint8_t *msg, uint16_t msglen, char **err)
{
  uint8_t bitmap[129];
  uint8_t res;
  
  memset(bitmap, 0, 129);

  if (parser_get_bitmap(msg, 0, bitmap, sizeof(bitmap)) != CPOS_OK) {
    *err = util_get_error(CPOS_PRIBITMAP);
    return CPOS_ERROR;
  }
  
  if (*(bitmap) == '1') { // get sec bitmap if any
    if (parser_get_bitmap(msg, 1, bitmap + 64, sizeof(bitmap)) != CPOS_OK) {
      *err = util_get_error(CPOS_SECBITMAP);
      return CPOS_ERROR;
    }
  }
  
  res = parser_check_get_data(bc, msg, msglen, &bitmap[0], imsg);
  *err = util_get_error(res);
  
  if ((res != CPOS_OK) || !(*err))
    return CPOS_ERROR;

  return CPOS_OK;
}
