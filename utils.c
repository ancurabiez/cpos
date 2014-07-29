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

struct error_lists {
  uint8_t erno;
  char strerr[32];
} ELISTS [5] = {
    {CPOS_OK, "OK\n"},
    {CPOS_ERROR, "ERROR\n"},
    {CPOS_NOMEM, "Not enough memory\n"},
    {CPOS_LENERR, "Message length not match\n"}
};

/* ----------------------------------------------------------------------------------- */
/* crypto/ebcdic.c */

/*      Initial Port for  Apache-1.3     by <Martin.Kraemer@Mch.SNI.De>
        Adapted for       OpenSSL-0.9.4  by <Martin.Kraemer@Mch.SNI.De>
*/

#ifdef _OSD_POSIX
/*
      "BS2000 OSD" is a POSIX subsystem on a main frame.
      It is made by Siemens AG, Germany, for their BS2000 mainframe machines.
      Within the POSIX subsystem, the same character set was chosen as in
      "native BS2000", namely EBCDIC. (EDF04)

      The name "ASCII" in these routines is misleading: actually, conversion
      is not between EBCDIC and ASCII, but EBCDIC(EDF04) and ISO-8859.1;
      that means that (western european) national characters are preserved.

      This table is identical to the one used by rsh/rcp/ftp and other POSIX tools.
*/

/* Here's the bijective ebcdic-to-ascii table: */
const unsigned char os_toascii[256] = {
/*00*/ 0x00, 0x01, 0x02, 0x03, 0x85, 0x09, 0x86, 0x7f,
       0x87, 0x8d, 0x8e, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /*................*/
/*10*/ 0x10, 0x11, 0x12, 0x13, 0x8f, 0x0a, 0x08, 0x97,
       0x18, 0x19, 0x9c, 0x9d, 0x1c, 0x1d, 0x1e, 0x1f, /*................*/
/*20*/ 0x80, 0x81, 0x82, 0x83, 0x84, 0x92, 0x17, 0x1b,
       0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x05, 0x06, 0x07, /*................*/
/*30*/ 0x90, 0x91, 0x16, 0x93, 0x94, 0x95, 0x96, 0x04,
       0x98, 0x99, 0x9a, 0x9b, 0x14, 0x15, 0x9e, 0x1a, /*................*/
/*40*/ 0x20, 0xa0, 0xe2, 0xe4, 0xe0, 0xe1, 0xe3, 0xe5,
       0xe7, 0xf1, 0x60, 0x2e, 0x3c, 0x28, 0x2b, 0x7c, /* .........`.<(+|*/
/*50*/ 0x26, 0xe9, 0xea, 0xeb, 0xe8, 0xed, 0xee, 0xef,
       0xec, 0xdf, 0x21, 0x24, 0x2a, 0x29, 0x3b, 0x9f, /*&.........!$*);.*/
/*60*/ 0x2d, 0x2f, 0xc2, 0xc4, 0xc0, 0xc1, 0xc3, 0xc5,
       0xc7, 0xd1, 0x5e, 0x2c, 0x25, 0x5f, 0x3e, 0x3f, /*-/........^,%_>?*/
/*70*/ 0xf8, 0xc9, 0xca, 0xcb, 0xc8, 0xcd, 0xce, 0xcf,
       0xcc, 0xa8, 0x3a, 0x23, 0x40, 0x27, 0x3d, 0x22, /*..........:#@'="*/
/*80*/ 0xd8, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
       0x68, 0x69, 0xab, 0xbb, 0xf0, 0xfd, 0xfe, 0xb1, /*.abcdefghi......*/
/*90*/ 0xb0, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
       0x71, 0x72, 0xaa, 0xba, 0xe6, 0xb8, 0xc6, 0xa4, /*.jklmnopqr......*/
/*a0*/ 0xb5, 0xaf, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
       0x79, 0x7a, 0xa1, 0xbf, 0xd0, 0xdd, 0xde, 0xae, /*..stuvwxyz......*/
/*b0*/ 0xa2, 0xa3, 0xa5, 0xb7, 0xa9, 0xa7, 0xb6, 0xbc,
       0xbd, 0xbe, 0xac, 0x5b, 0x5c, 0x5d, 0xb4, 0xd7, /*...........[\]..*/
/*c0*/ 0xf9, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
       0x48, 0x49, 0xad, 0xf4, 0xf6, 0xf2, 0xf3, 0xf5, /*.ABCDEFGHI......*/
/*d0*/ 0xa6, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
       0x51, 0x52, 0xb9, 0xfb, 0xfc, 0xdb, 0xfa, 0xff, /*.JKLMNOPQR......*/
/*e0*/ 0xd9, 0xf7, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
       0x59, 0x5a, 0xb2, 0xd4, 0xd6, 0xd2, 0xd3, 0xd5, /*..STUVWXYZ......*/
/*f0*/ 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
       0x38, 0x39, 0xb3, 0x7b, 0xdc, 0x7d, 0xda, 0x7e  /*0123456789.{.}.~*/
};


/* The ascii-to-ebcdic table: */
const unsigned char os_toebcdic[256] = {
/*00*/  0x00, 0x01, 0x02, 0x03, 0x37, 0x2d, 0x2e, 0x2f,
	0x16, 0x05, 0x15, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,  /*................*/
/*10*/  0x10, 0x11, 0x12, 0x13, 0x3c, 0x3d, 0x32, 0x26,
	0x18, 0x19, 0x3f, 0x27, 0x1c, 0x1d, 0x1e, 0x1f,  /*................*/
/*20*/  0x40, 0x5a, 0x7f, 0x7b, 0x5b, 0x6c, 0x50, 0x7d,
	0x4d, 0x5d, 0x5c, 0x4e, 0x6b, 0x60, 0x4b, 0x61,  /* !"#$%&'()*+,-./ */
/*30*/  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
	0xf8, 0xf9, 0x7a, 0x5e, 0x4c, 0x7e, 0x6e, 0x6f,  /*0123456789:;<=>?*/
/*40*/  0x7c, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
	0xc8, 0xc9, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6,  /*@ABCDEFGHIJKLMNO*/
/*50*/  0xd7, 0xd8, 0xd9, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6,
	0xe7, 0xe8, 0xe9, 0xbb, 0xbc, 0xbd, 0x6a, 0x6d,  /*PQRSTUVWXYZ[\]^_*/
/*60*/  0x4a, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,  /*`abcdefghijklmno*/
/*70*/  0x97, 0x98, 0x99, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,
	0xa7, 0xa8, 0xa9, 0xfb, 0x4f, 0xfd, 0xff, 0x07,  /*pqrstuvwxyz{|}~.*/
/*80*/  0x20, 0x21, 0x22, 0x23, 0x24, 0x04, 0x06, 0x08,
	0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x09, 0x0a, 0x14,  /*................*/
/*90*/  0x30, 0x31, 0x25, 0x33, 0x34, 0x35, 0x36, 0x17,
	0x38, 0x39, 0x3a, 0x3b, 0x1a, 0x1b, 0x3e, 0x5f,  /*................*/
/*a0*/  0x41, 0xaa, 0xb0, 0xb1, 0x9f, 0xb2, 0xd0, 0xb5,
	0x79, 0xb4, 0x9a, 0x8a, 0xba, 0xca, 0xaf, 0xa1,  /*................*/
/*b0*/  0x90, 0x8f, 0xea, 0xfa, 0xbe, 0xa0, 0xb6, 0xb3,
	0x9d, 0xda, 0x9b, 0x8b, 0xb7, 0xb8, 0xb9, 0xab,  /*................*/
/*c0*/  0x64, 0x65, 0x62, 0x66, 0x63, 0x67, 0x9e, 0x68,
	0x74, 0x71, 0x72, 0x73, 0x78, 0x75, 0x76, 0x77,  /*................*/
/*d0*/  0xac, 0x69, 0xed, 0xee, 0xeb, 0xef, 0xec, 0xbf,
	0x80, 0xe0, 0xfe, 0xdd, 0xfc, 0xad, 0xae, 0x59,  /*................*/
/*e0*/  0x44, 0x45, 0x42, 0x46, 0x43, 0x47, 0x9c, 0x48,
	0x54, 0x51, 0x52, 0x53, 0x58, 0x55, 0x56, 0x57,  /*................*/
/*f0*/  0x8c, 0x49, 0xcd, 0xce, 0xcb, 0xcf, 0xcc, 0xe1,
	0x70, 0xc0, 0xde, 0xdb, 0xdc, 0x8d, 0x8e, 0xdf   /*................*/
};

#else  /*_OSD_POSIX*/

/*
 * This code does basic character mapping for IBM's TPF and OS/390 operating systems.
 * It is a modified version of the BS2000 table.
 *
 * Bijective EBCDIC (character set IBM-1047) to US-ASCII table:
 * This table is bijective - there are no ambigous or duplicate characters.
 * */
const unsigned char os_toascii[256] = {
    0x00, 0x01, 0x02, 0x03, 0x85, 0x09, 0x86, 0x7f, /* 00-0f:           */
    0x87, 0x8d, 0x8e, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /* ................ */
    0x10, 0x11, 0x12, 0x13, 0x8f, 0x0a, 0x08, 0x97, /* 10-1f:           */
    0x18, 0x19, 0x9c, 0x9d, 0x1c, 0x1d, 0x1e, 0x1f, /* ................ */
    0x80, 0x81, 0x82, 0x83, 0x84, 0x92, 0x17, 0x1b, /* 20-2f:           */
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x05, 0x06, 0x07, /* ................ */
    0x90, 0x91, 0x16, 0x93, 0x94, 0x95, 0x96, 0x04, /* 30-3f:           */
    0x98, 0x99, 0x9a, 0x9b, 0x14, 0x15, 0x9e, 0x1a, /* ................ */
    0x20, 0xa0, 0xe2, 0xe4, 0xe0, 0xe1, 0xe3, 0xe5, /* 40-4f:           */
    0xe7, 0xf1, 0xa2, 0x2e, 0x3c, 0x28, 0x2b, 0x7c, /*  ...........<(+| */
    0x26, 0xe9, 0xea, 0xeb, 0xe8, 0xed, 0xee, 0xef, /* 50-5f:           */
    0xec, 0xdf, 0x21, 0x24, 0x2a, 0x29, 0x3b, 0x5e, /* &.........!$*);^ */
    0x2d, 0x2f, 0xc2, 0xc4, 0xc0, 0xc1, 0xc3, 0xc5, /* 60-6f:           */
    0xc7, 0xd1, 0xa6, 0x2c, 0x25, 0x5f, 0x3e, 0x3f, /* -/.........,%_>? */
    0xf8, 0xc9, 0xca, 0xcb, 0xc8, 0xcd, 0xce, 0xcf, /* 70-7f:           */
    0xcc, 0x60, 0x3a, 0x23, 0x40, 0x27, 0x3d, 0x22, /* .........`:#@'=" */
    0xd8, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, /* 80-8f:           */
    0x68, 0x69, 0xab, 0xbb, 0xf0, 0xfd, 0xfe, 0xb1, /* .abcdefghi...... */
    0xb0, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70, /* 90-9f:           */
    0x71, 0x72, 0xaa, 0xba, 0xe6, 0xb8, 0xc6, 0xa4, /* .jklmnopqr...... */
    0xb5, 0x7e, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, /* a0-af:           */
    0x79, 0x7a, 0xa1, 0xbf, 0xd0, 0x5b, 0xde, 0xae, /* .~stuvwxyz...[.. */
    0xac, 0xa3, 0xa5, 0xb7, 0xa9, 0xa7, 0xb6, 0xbc, /* b0-bf:           */
    0xbd, 0xbe, 0xdd, 0xa8, 0xaf, 0x5d, 0xb4, 0xd7, /* .............].. */
    0x7b, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, /* c0-cf:           */
    0x48, 0x49, 0xad, 0xf4, 0xf6, 0xf2, 0xf3, 0xf5, /* {ABCDEFGHI...... */
    0x7d, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50, /* d0-df:           */
    0x51, 0x52, 0xb9, 0xfb, 0xfc, 0xf9, 0xfa, 0xff, /* }JKLMNOPQR...... */
    0x5c, 0xf7, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, /* e0-ef:           */
    0x59, 0x5a, 0xb2, 0xd4, 0xd6, 0xd2, 0xd3, 0xd5, /* \.STUVWXYZ...... */
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, /* f0-ff:           */
    0x38, 0x39, 0xb3, 0xdb, 0xdc, 0xd9, 0xda, 0x9f  /* 0123456789...... */
};


/*
 * The US-ASCII to EBCDIC (character set IBM-1047) table:
 * This table is bijective (no ambiguous or duplicate characters)
 * */
const unsigned char os_toebcdic[256] = {
    0x00, 0x01, 0x02, 0x03, 0x37, 0x2d, 0x2e, 0x2f, /* 00-0f:           */
    0x16, 0x05, 0x15, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, /* ................ */
    0x10, 0x11, 0x12, 0x13, 0x3c, 0x3d, 0x32, 0x26, /* 10-1f:           */
    0x18, 0x19, 0x3f, 0x27, 0x1c, 0x1d, 0x1e, 0x1f, /* ................ */
    0x40, 0x5a, 0x7f, 0x7b, 0x5b, 0x6c, 0x50, 0x7d, /* 20-2f:           */
    0x4d, 0x5d, 0x5c, 0x4e, 0x6b, 0x60, 0x4b, 0x61, /*  !"#$%&'()*+,-./ */
    0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, /* 30-3f:           */
    0xf8, 0xf9, 0x7a, 0x5e, 0x4c, 0x7e, 0x6e, 0x6f, /* 0123456789:;<=>? */
    0x7c, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, /* 40-4f:           */
    0xc8, 0xc9, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, /* @ABCDEFGHIJKLMNO */
    0xd7, 0xd8, 0xd9, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, /* 50-5f:           */
    0xe7, 0xe8, 0xe9, 0xad, 0xe0, 0xbd, 0x5f, 0x6d, /* PQRSTUVWXYZ[\]^_ */
    0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, /* 60-6f:           */
    0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, /* `abcdefghijklmno */
    0x97, 0x98, 0x99, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, /* 70-7f:           */
    0xa7, 0xa8, 0xa9, 0xc0, 0x4f, 0xd0, 0xa1, 0x07, /* pqrstuvwxyz{|}~. */
    0x20, 0x21, 0x22, 0x23, 0x24, 0x04, 0x06, 0x08, /* 80-8f:           */
    0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x09, 0x0a, 0x14, /* ................ */
    0x30, 0x31, 0x25, 0x33, 0x34, 0x35, 0x36, 0x17, /* 90-9f:           */
    0x38, 0x39, 0x3a, 0x3b, 0x1a, 0x1b, 0x3e, 0xff, /* ................ */
    0x41, 0xaa, 0x4a, 0xb1, 0x9f, 0xb2, 0x6a, 0xb5, /* a0-af:           */
    0xbb, 0xb4, 0x9a, 0x8a, 0xb0, 0xca, 0xaf, 0xbc, /* ................ */
    0x90, 0x8f, 0xea, 0xfa, 0xbe, 0xa0, 0xb6, 0xb3, /* b0-bf:           */
    0x9d, 0xda, 0x9b, 0x8b, 0xb7, 0xb8, 0xb9, 0xab, /* ................ */
    0x64, 0x65, 0x62, 0x66, 0x63, 0x67, 0x9e, 0x68, /* c0-cf:           */
    0x74, 0x71, 0x72, 0x73, 0x78, 0x75, 0x76, 0x77, /* ................ */
    0xac, 0x69, 0xed, 0xee, 0xeb, 0xef, 0xec, 0xbf, /* d0-df:           */
    0x80, 0xfd, 0xfe, 0xfb, 0xfc, 0xba, 0xae, 0x59, /* ................ */
    0x44, 0x45, 0x42, 0x46, 0x43, 0x47, 0x9c, 0x48, /* e0-ef:           */
    0x54, 0x51, 0x52, 0x53, 0x58, 0x55, 0x56, 0x57, /* ................ */
    0x8c, 0x49, 0xcd, 0xce, 0xcb, 0xcf, 0xcc, 0xe1, /* f0-ff:           */
    0x70, 0xdd, 0xde, 0xdb, 0xdc, 0x8d, 0x8e, 0xdf  /* ................ */
};
#endif /*_OSD_POSIX*/
/*------------------------------------------------------------------------------- */


uint8_t* cpos_zero_trim(const uint8_t *str)
{
  uint8_t *buf;
  int len;
  
  len = strlen((char*) str);
  
  buf = malloc(len + 1);
  if (!buf)
    return NULL;
  
  while (*str) {
    if (*str == '0') {
      str++;
      len--;
      continue;
    }
    
    break;
  }
  
  memcpy(buf, str, len);
  len++;
  buf[len] = '\0';
  
  return buf;	
}

uint8_t* cpos_space_trim(const uint8_t* str)
{
  uint8_t *buf;
  int len;
  
  len = strlen((char*) str);
  
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
  
  bc = malloc(ISO_BIT_LEN * sizeof(struct isofield_cfg));
  if (!bc)
    return NULL;
  
  memset(bc, 0, sizeof(struct isomsg) * ISO_BIT_LEN);
  
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
           
      tokenval = (char*) cpos_space_trim((uint8_t *) subtoken);
      	
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
          
          if ((l > 999) || (l < 0)) {
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
        
        if ((l > 999) || (l < 0) ||
            ((l > 0) && (x == 0))) {
          error__(line, tokencount, &bc);
          err = 1;
          break;
        }
    
        //if ((l == 0) && (x == 1)) {
        //  error__(line, tokencount, &bc);
        //  err = 1;
        //  break;
        //}
        
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
  
  imsg = malloc(sizeof(struct isomsg) * ISO_BIT_LEN);
  if (!imsg)
    return NULL;
  
  memset(imsg, 0, sizeof(struct isomsg) * ISO_BIT_LEN);
  
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

void* utils_fill(const void* data, uint16_t datalen,
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

char* util_get_error(uint8_t erno)
{
  char *tmp;
  
  tmp = strdup(ELISTS[erno].strerr);
  if (!tmp)
    return NULL;
  
  return tmp;
}


uint8_t* cpos_ascii2hex(const uint8_t *str)
{
  uint16_t i, len, c = 0;
  uint8_t b1[4];
  uint8_t *buf;
  
  len = strlen((char*) str);
  buf = malloc((len * 2) + 1);
  if (!buf)
    return NULL;
  
  memset(buf, 0, (len *2) + 1);
  
  for (i = 0; i < len; i++) {
    b1[0] = (str[i] >> 4) & 0x0f;
    b1[1] = (str[i] & 0x0f);
  
    if (b1[0] < 10)
      b1[0] += 48;
    else
      b1[0] += 55;

    if (b1[1] < 10)
      b1[1] += 48;
    else
      b1[1] += 55;

    memcpy(buf + c, b1, 2);
    c += 2;
  }

  return buf;
}

uint8_t* cpos_hex2ascii(const uint8_t *hex)
{
  uint16_t i = 0, c;
  uint8_t *buf;
  uint8_t b1[4];
  
  c = strlen((char*) hex);
  buf = malloc(c);
  if (!buf)
    return NULL;
  
  memset(buf, 0, c);
  
  while (*hex) {
    memcpy(b1, hex, 2);
    b1[2] = '\0';
    
    c = strtol((char*) b1, NULL, 16);
    buf[i] = c;
    
    hex += 2;
    i++;
  }
  
  return buf;
}

uint8_t* cpos_ebcdic2ascii(const void *ebcd, size_t len)
{
  uint8_t *buf;
  uint16_t i = 0;
  const uint8_t *ebcdtmp = ebcd;

  buf = malloc(len + 1);
  if (!buf)
    return NULL;

  memset(buf, 0, len);

  while (len-- != 0) {
    *(buf + i) = os_toascii[*ebcdtmp++];
    i++;
  }

  return buf;
}

uint8_t* cpos_ascii2ebcdic(const void *str, size_t len)
{
  uint8_t *buf;
  uint16_t i = 0;
  const uint8_t *strtmp = str;

  buf = malloc(len + 1);
  if (!buf)
    return NULL;

  memset(buf, 0, len);

  while (len-- != 0) {
    *(buf + 1) = os_toebcdic[*strtmp++];
    i++;
  }

  return buf;
}
