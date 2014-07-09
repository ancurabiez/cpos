#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cpos.h"


int main(void)
{
  int i;
  int count = 100000;
  clock_t start, end;
  
  start = clock();
  
  struct isofield_cfg *ifield;
  struct isomsg *imsg;
  char buffer[1024];

  ifield = cpos_init("cpos.cfg");
  if (!ifield)
    return 1;
    
  for (i = 0; i < count; i++) {
    imsg = cpos_msg_new();
    if (!imsg)
      return 1;
   
    cpos_build_set_field(imsg, 2, "4729700101137365", 16);
    cpos_build_set_field(imsg, 3, "880891", 6);
    cpos_build_set_field(imsg, 4, "000000000000", 12);
    cpos_build_set_field(imsg, 7, "1127041312", 10);
    cpos_build_set_field(imsg, 11, "036030", 6);
    cpos_build_set_field(imsg, 12, "111312", 6);
    cpos_build_set_field(imsg, 13, "1127", 4);
    cpos_build_set_field(imsg, 14, "0000", 4);
    cpos_build_set_field(imsg, 15, "1128", 4);
    cpos_build_set_field(imsg, 18, "6971", 4);
    cpos_build_set_field(imsg, 32, "717", 3);
    cpos_build_set_field(imsg, 33, "226", 3);
    cpos_build_set_field(imsg, 37, "013331036030", 12);
    cpos_build_set_field(imsg, 41, "00000036", 8);
    cpos_build_set_field(imsg, 42, "000            ", 15);
    cpos_build_set_field(imsg, 43, "ABCDFG ABCDEFGH                  1234567", 40);
    cpos_build_set_field(imsg, 49, "450", 3);
    cpos_build_set_field(imsg, 61, "1339617306680", 13);
    cpos_build_set_field(imsg, 103, "054201", 6);

    cpos_build_msg(ifield, imsg, "0210", buffer, sizeof(buffer));
    //printf("Build message: [%s]\n", buffer);
 
    cpos_msg_free(imsg);
  }
  
  cpos_close(ifield);
  
  end = clock();
  
  printf("Count: %d\n", count);
  printf("Elapsed: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);
  
  return 0;
}