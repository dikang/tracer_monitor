#include <stdint.h>
#include "taskletconfig.h"
#define DEBUG 1

#ifndef INTEL
extern void print_hexint(uint64_t t);
#else
extern void print_hexint(uint64_t t) {
}
#endif

// the following code allocates and initializes the first a few bytes of configuration memory for monitors running on all Harts.
__attribute__((section(".data0"))) tasklet_config tc0 = { PC_ADDR(0), SP_ADDR(0), TP_ADDR(0), GP_ADDR(0), \
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, \
                 900, 0, 0 };
__attribute__((section(".data1"))) tasklet_config tc1 = { PC_ADDR(1), SP_ADDR(1), TP_ADDR(1), GP_ADDR(1), \
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, \
                 901, 0, 0 };
__attribute__((section(".data2"))) tasklet_config tc2 = { PC_ADDR(2), SP_ADDR(2), TP_ADDR(2), GP_ADDR(2), \
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, \
                 902, 0, 0 };
__attribute__((section(".data3"))) tasklet_config tc3 = { PC_ADDR(3), SP_ADDR(3), TP_ADDR(3), GP_ADDR(3), \
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, \
                 903, 0, 0 };

#if DEBUG

void debug_putc(char c) {
    char * uart = (char *) UART_BASE;
    * uart = c;
}
#else
void debug_putc(char c) {
}
#endif


uint64_t process_int_cmd(int_cmd_queue * iq, tasklet_queue * tq) {
   switch_cmd * cmd = & iq->cmds[iq->back];
   if (iq->front != iq->back && cmd->cp != 0) { // target tasklet
        iq->back++;
        tq->slots[cmd->slot].cp = cmd->cp;
        // may optimize this with uint32_t save
        tq->slots[cmd->slot].active = cmd->active;
        tq->slots[cmd->slot].context_save = cmd->context_save;
        if (cmd->active) { // jump to that address
            return cmd->cp;
        }
    }
    else {	// no target tasklet
        if (iq->front != iq->back) iq->back++;
        // pick next tasklet in the tasklet slots
        if (tq->n_active != 0) {
            uint32_t i = (tq->curr_slot + 1) % NUM_TASKLET_SLOTS;
            while (i != tq->curr_slot) {
                if (tq->slots[i].active != 0) {
                    return tq->slots[i].cp;
                }
                i = (i+1) % NUM_TASKLET_SLOTS;
            }
        }
    }
}

void set_int_cmd(int hartid, uint64_t cp, uint64_t slot, uint8_t active, uint8_t save) {
    tasklet_slot * sl;
    sl->cp = cp;
//    sl->slot = slot;
    sl->active = active;
//    sl->save = save;
}

void trigger_int(uint32_t hartid) {
     uint32_t clint_addr = (CLINT_BASE + 4 * hartid);
     uint32_t * clint = (uint32_t*) (clint_addr);
     * clint = 1;
}
void end_hart0() {
    int i, j;
    uint64_t sum = 0;
    for (i = 0; i < 1000000; i++)
        for (j = 0; j < 10; j++) sum++;
    if (sum > 1000000000) {
    debug_putc('e'); debug_putc('n'); debug_putc('d'); debug_putc('0'); 
    debug_putc('\n');
    } else {
    debug_putc('e'); debug_putc('n'); debug_putc('d'); debug_putc('0'); 
    debug_putc('\n');
    }
}

void test_hart0() {
    uint64_t i, j;
     uint32_t * clint_addr = (uint32_t *)(CLINT_BASE);
    debug_putc('h'); debug_putc('a'); debug_putc('r'); debug_putc('t'); debug_putc('0'); debug_putc(':'); debug_putc('\n');
    for (i = 0; i < 10000000; i++)  j++;
    clint_addr[1] = 1; 
    for (i = 0; i < 10000000; i++)  j++;
    clint_addr[2] = 1; 
    for (i = 0; i < 10000000; i++)  j++;
    clint_addr[1] = 1; 
    for (i = 0; i < 10000000; i++)  j++;
    clint_addr[2] = 1; 
}

uint64_t monitor_main(tasklet_arg * arg) {
    if (arg->tasklet_id != 900 + arg->hartid) { debug_putc('N'); debug_putc('O');}
    uint64_t t = (uint64_t) arg;
    print_hexint(t);
    volatile uint32_t * msip = (uint32_t *) CLINT_BASE;
    int_cmd_queue * iq = (int_cmd_queue *) INT_MSGQ_ADDR(arg->hartid);
    tasklet_queue * tq = (tasklet_queue *) &arg->tqueue;
    if (arg->entry == 0) { // initial entry
        uint64_t hartid;
        /* initialize intQ and slotQ */
        arg->entry = 0xbeef;
        iq->front = iq->back = 0;
        tq->low = tq->high = tq->curr_slot = tq->max_slots = tq->n_active = 0;
#ifndef INTEL
        asm volatile ("csrr %0, mhartid" : "=r" (hartid));
        asm volatile ("csrw mcycle, 0" : : );
        asm volatile ("csrw minstret, 0" : : );
        uint64_t cycles1, cycles2, instructions1, instructions2;
        asm volatile ("csrr %0, mcycle" : "=r" (cycles1) : );
        asm volatile ("csrr %0, minstret" : "=r" (instructions1) : );
        asm volatile ("csrr %0, mcycle" : "=r" (cycles2) : );
        asm volatile ("csrr %0, minstret" : "=r" (instructions2) : );
//        print_hexint(cycles2 - cycles1);
//        print_hexint(instructions2 - instructions1);
#endif
        debug_putc('m');
        debug_putc('I');
        char ci = arg->hartid;
        ci = ci + '0';
        debug_putc(ci);
        ci = hartid;
        ci = ci + '0';
        debug_putc(ci);
        debug_putc('\n');
    }
#if DEBUG
    else {	// debugging purpose
        debug_putc('m'); debug_putc('R');
        char ci = arg->hartid;
        ci = ci + '0';
        debug_putc(ci); debug_putc('\n');
        return process_int_cmd(iq, tq);
    }
    char ci = arg->hartid;
    ci = ci + '0';
    debug_putc(ci);
    debug_putc('i'); debug_putc('n'); debug_putc('t'); debug_putc(':');
    char c = '0';
    int i;
    for (i = 0; i < 3; i++) {	// shows which Hart has interrupt pending
      if (msip[i] == 1) {
          debug_putc(c);
          debug_putc(',');
      }
      c++;
    }
    debug_putc('\n');
    /* check if there is commands waiting */
//    debug_putc('r'); debug_putc('\n');
#endif
    
    return process_int_cmd(iq, tq);
}

#ifdef INTEL
#include <stdio.h>
void main(int argc, char ** argv) {
    tasklet_config tc;
    tasklet_arg t1;
    int i;
    uint64_t r = (uint64_t) &tc.tasklet_id - (uint64_t)&tc;
    printf("TASKLET_ARG_OFFSET = %ld\n", TASKLET_ARG_OFFSET);
    printf("TASKLET_ARG_OFFSET = %ld\n", r);
    printf("TASKLET_HARTID_OFFSET = %ld\n", TASKLET_HARTID_OFFSET);
    r = (uint64_t) &tc.hartid - (uint64_t)&tc;
    printf("TASKLET_HARTID_OFFSET = %ld\n", r);
    printf("Config size = 0x%x < 0x%x\n", sizeof(tasklet_config) + sizeof(tasklet_arg), CONFIG_SIZE);
    for (i = 0; i < 3; i++) {
        printf("SP_ADDR(%d) = 0x%x\n", i, SP_ADDR(i));
        printf("TP_ADDR(%d) = 0x%x\n", i, TP_ADDR(i));
        printf("GP_ADDR(%d) = 0x%x\n", i, GP_ADDR(i));
        printf("CODE_ADDR(%d) = 0x%x\n", i, CODE_ADDR(i));
        printf("CONFIG_ADDR(%d) = 0x%x\n", i, CONFIG_ADDR(i));
    }

    t1.tasklet_id = 0;
    t1.entry = argc - 1;
    printf("tasklet_arg size = 0x%x\n", sizeof(tasklet_arg));
    monitor_main(&t1);
}
#endif
