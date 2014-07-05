#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cpos.h"

int main(void)
{
  int i;
  int count = 100000;
  char iso8583[512];
  clock_t start, end;
  
  strcpy(iso8583, "0210F23E400188E0800800000000020000001647297"
		  "0010113736588089100000000000011270413120360301113121"
		  "127000011286971037170322601333103603000000036"
		  "000            ABCDFG ABCDEFGH                  1234567"
		  "360013133961730668006054201");
  
  start = clock();
  
  struct isofield_cfg *ifield;
  struct isomsg *imsg;

  ifield = cpos_init("cpos.cfg");
  if (!ifield)
    return 1;
    
  for (i = 0; i < count; i++) {
    imsg = cpos_msg_new();
    if (!imsg)
      return 1;
    
    if (cpos_parse(ifield, imsg, (uint8_t*) iso8583, strlen(iso8583)))
      return 1;
    
    //for (i = 0; i < ISO_BIT_LEN; i++) {
    //  if (imsg[i].data)
    //    printf("BIT [%d] [%d] [%s]\n", i, imsg[i].len, (char*)imsg[i].data);
   // }
    cpos_msg_free(imsg);
  }
  
  cpos_close(ifield);
  
  end = clock();
  
  printf("Count: %d\n", count);
  printf("Elapsed: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);
  
  return 0;
}
