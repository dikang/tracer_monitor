#include <stdint.h>
#include "tasklet_config.h"
#include "utility.h"

extern void tasklet0(tasklet_arg * );
extern void tasklet1(tasklet_arg * );
extern void tasklet2(tasklet_arg * );
extern void tasklet3(tasklet_arg * );
extern uint64_t wait(uint64_t ) ;
const char t = 't';	/* for testing rodata section */
uint64_t gt;

/* tasklets */
__attribute__((section(".data_tasklet0"))) tasklet_config tc0 = { 0, SP_TASKLET_ADDR(0), TP_TASKLET_ADDR(0), 0, \
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, \
                 1, 0, 1 };
__attribute__((section(".data_tasklet1"))) tasklet_config tc1 = { 0, SP_TASKLET_ADDR(1), TP_TASKLET_ADDR(1), 0, \
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, \
                 2, 0, 1 };
__attribute__((section(".data_tasklet2"))) tasklet_config tc2 = { 0, SP_TASKLET_ADDR(2), TP_TASKLET_ADDR(2), 0, \
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, \
                 3, 0, 2 };
__attribute__((section(".data_tasklet3"))) tasklet_config tc3 = { 0, SP_TASKLET_ADDR(3), TP_TASKLET_ADDR(3), 0, \
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, \
                 4, 0, 2 };


uint64_t tasklet_main(tasklet_arg * arg) {
    // testing readonly data
    // for debugging purpose
    if (arg->tasklet_id != 100 + arg->hartid) { debug_putc('N'); debug_putc('O');}
    uint64_t t = (uint64_t) arg;
    debug_print("Tasklet Main arg address =\0");
    print_hexuint64(t);
    // end of debugging
    debug_print("Tasklet Main starts:\0");
    char ci = arg->hartid;
    ci = ci + '0';
    debug_putc(ci);
    debug_putc('\n');
    /* spawn tasklet0 on CAE1 */
    /* 1. set up tasklet_conf data */
    void (*funcPtr)();
    funcPtr = tasklet0;
    tasklet_setup(&tc0, (uint64_t)funcPtr, SP_TASKLET_ADDR(0), TP_TASKLET_ADDR(0),0,0);
    debug_print("  PC of tasklet0 = \0");
    print_hexuint64((uint64_t)funcPtr); 
    funcPtr = tasklet1;
    tasklet_setup(&tc1, (uint64_t)funcPtr, SP_TASKLET_ADDR(1), TP_TASKLET_ADDR(1),0,0);
    debug_print("  PC of tasklet1 = \0");
    print_hexuint64((uint64_t)funcPtr); 
    funcPtr = tasklet2;
    tasklet_setup(&tc2, (uint64_t)funcPtr, SP_TASKLET_ADDR(2), TP_TASKLET_ADDR(2),0,0);
    funcPtr = tasklet3;
    tasklet_setup(&tc3, (uint64_t)funcPtr, SP_TASKLET_ADDR(3), TP_TASKLET_ADDR(3),0,0);
    debug_print("Tasklet Main finished setup of tasklets\n\0");
    wait(2);
    debug_print("Tasklet Main triggers interrupt to hart 1 for tasklet0\n\0");
    /* 2. let tasklet0 start on CAE1 */
    trigger_int(1, (uint64_t) &tc0, 0, (1 | (0 << 16)));
    wait(2);
    debug_print("Tasklet Main triggers interrupt to hart 1 for tasklet1\n\0");
    /* 3. let tasklet1 start on CAE1 */
    trigger_int(1, (uint64_t) &tc1, 1, (1 | (0 << 16)));
    /* switch back tasklet2 on CAE1 */
    debug_print("Tasklet Main infinite loop\n\0");
    while (1);
}

uint64_t wait(uint64_t seed) {
    uint64_t i, j, k;
    k = seed;
    for (i = 0; i < 1000000; i++)
        for (j = 0; j < 100; j++)
            k++;
    return ;
}

int switch_tasklet(uint32_t hart_id, uint32_t slot, uint32_t active_save, tasklet_config *tc) {
    uint64_t int_msgq = INT_MSGQ_ADDR(hart_id);
    int_cmd_queue * iq = (int_cmd_queue *) int_msgq;
  
    // disable_interrupt();
    // lock();
    if (((iq->front + 1) % NUM_INTQ_SLOTS) == iq->back) { // full
       // unlock();
       // enable_interrupt();
        return 0;    
    }
    iq->cmds[iq->front].cp = (uint64_t) tc;
    iq->cmds[iq->front].slot = slot;
    iq->cmds[iq->front].active_save = active_save;
    
    iq->front = (iq->front + 1) % NUM_INTQ_SLOTS;
    // trigger_interrupt(hartid);
    // unlock();
    // enable_interrupt();
    return 1;
}

void tasklet_setup(tasklet_config * tc, uint64_t funcPtr, uint64_t sp, uint64_t tp, uint32_t id, uint32_t entry) {
    tc->pc = funcPtr;
    tc->sp = sp;
    tc->tp = tp;
    tc->tasklet_id = id;
    tc->entry = entry;
}
void tasklet0(tasklet_arg * arg) {
    uint64_t hartid;
    debug_print("tasklet0 starts on hart\0");
    asm volatile ("csrr %0, mhartid" : "=r" (hartid));
    char ci = hartid;
    ci = ci + '0';
    debug_putc(ci);
    debug_putc('\n');
    debug_print("tasklet0 enters infinite loop\n\0");
    while(1);
}

void tasklet1(tasklet_arg * arg) {
    uint32_t hartid;
    debug_print("tasklet1 starts on hart\0");
    asm volatile ("csrr %0, mhartid" : "=r" (hartid));
    char ci = hartid;
    ci = ci + '0';
    debug_putc(ci);
    debug_putc('\n');
    wait(2);
    debug_print("tasklet1 forked lightning tasklet2\n\0");
    trigger_int(2, (uint64_t) &tc2, 1, (1 | (0 << 16)));
    debug_print("tasklet1 is still running\n\0");
    while(1);
}

void tasklet2(tasklet_arg * arg) {
    uint32_t hartid;
    debug_print("tasklet2 starts on hart\0");
    asm volatile ("csrr %0, mhartid" : "=r" (hartid));
    char ci = hartid;
    ci = ci + '0';
    debug_putc(ci);
    debug_putc('\n');
    debug_print("tasklet2 enters infinite loop\n\0");
    while(1);
}

void tasklet3(tasklet_arg * arg) {
    uint32_t hartid;
    debug_print("tasklet3 starts on hart\0");
    asm volatile ("csrr %0, mhartid" : "=r" (hartid));
    char ci = hartid;
    ci = ci + '0';
    debug_putc(ci);
    debug_putc('\n');
    debug_print("tasklet3 enters infinite loop\n\0");
    while(1);
}

