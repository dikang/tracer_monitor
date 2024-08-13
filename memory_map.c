#include <stdio.h>
#include "tasklet_config.h"

main()
{
   int i;
  
   printf("---- Monitor ----\n");
   for (i = 0; i < MAX_CAES; i++) {
       printf("CODE_ADDR(%d) = 0x%lx\n", i, CODE_ADDR(i));
       printf("CONFIG_ADDR(%d) = 0x%lx\n", i, CONFIG_ADDR(i));
       printf("GP_ADDR(%d) = 0x%lx\n", i, GP_ADDR(i));
       printf("TP_ADDR(%d) = 0x%lx\n", i, TP_ADDR(i));
       printf("SP_ADDR(%d) = 0x%lx\n", i, SP_ADDR(i));
   }

   printf("\n---- Interrupt Msg Queues ----\n");
   for (i = 0; i < MAX_CAES; i++) {
       printf("INT_MSGQ_ADDR(%d) = 0x%lx\n", i, INT_MSGQ_ADDR(i));
   }
 
   printf("\n---- Tasklet Master ----\n");
   printf("CONFIG_MASTER_ADDR = 0x%lx\n", CONFIG_MASTER_ADDR);
   printf("PC_MASTER_ADDR = 0x%lx\n", PC_MASTER_ADDR);
   printf("GP_MASTER_ADDR = 0x%lx\n", GP_MASTER_ADDR);
   printf("TP_MASTER_ADDR = 0x%lx\n", TP_MASTER_ADDR);
   printf("SP_MASTER_ADDR = 0x%lx\n", SP_MASTER_ADDR);

   printf("\n---- Tasklets ----\n");
   for (i = 0; i < 4 ; i++) {
       printf("CONFIG_TASKLET_ADDR(%d) = 0x%lx\n", i, CONFIG_TASKLET_ADDR(i));
       printf("TP_TASKLET_ADDR(%d) = 0x%lx\n", i, TP_TASKLET_ADDR(i));
       printf("SP_TASKLET_ADDR(%d) = 0x%lx\n", i, SP_TASKLET_ADDR(i));
   }
}
