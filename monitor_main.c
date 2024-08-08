#include <stdint.h>
#include "tasklet_config.h"
#include "utility.h"

// __attribute__((section(".rodata"))) char * tmps = "string";

// the following code allocates and initializes the first a few bytes of configuration memory for monitors running on all Harts.
__attribute__((section(".data_mon0"))) tasklet_config mon0 = { PC_ADDR(0), SP_ADDR(0), TP_ADDR(0), GP_ADDR(0), \
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, \
                 900, 0, 0 };
__attribute__((section(".data_mon1"))) tasklet_config mon1 = { PC_ADDR(1), SP_ADDR(1), TP_ADDR(1), GP_ADDR(1), \
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, \
                 901, 0, 0 };
__attribute__((section(".data_mon2"))) tasklet_config mon2 = { PC_ADDR(2), SP_ADDR(2), TP_ADDR(2), GP_ADDR(2), \
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, \
                 902, 0, 0 };
__attribute__((section(".data_master"))) tasklet_config mon3 = { PC_MASTER_ADDR, SP_MASTER_ADDR, TP_MASTER_ADDR, GP_MASTER_ADDR, \
                {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, \
                {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, \
                 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, \
                 100, 0, 0 };


uint64_t process_int_cmd(int_cmd_queue * iq, tasklet_queue * tq) {
   uint64_t addr = (uint64_t) iq;
   debug_print("  process_int_cmd: iq addr = \0");
   print_hexuint64(addr);
   if (iq->front == iq->back) { // nothing to do
       debug_print("  process_int_cmd: interrupt queue empty. return.\n\0");
       return 0;
   }
   switch_cmd * cmd = & iq->cmds[iq->back];
   iq->back++;
   // if cp == 0, just switch to anything else
   if (cmd->cp != 0) { // target is specified
        // print cmd:
        debug_print("  process_int_cmd: target = \0");
        print_hexuint64(cmd->cp);
        tq->slots[cmd->slot].cp = cmd->cp;
        tasklet_config * tcp = (tasklet_config *)(cmd->cp);
        debug_print("  process_int_cmd: target PC = \0");
        print_hexuint64(tcp->pc);
        
        // may optimize this with uint32_t save
        tq->slots[cmd->slot].active_save = cmd->active_save;
        if ((cmd->active_save & 0xF)) { // jump to that address
            // print switch:<cp>
            debug_print("  process_int_cmd: switich to new cp @\0");
            print_hexuint64(cmd->cp);
            return cmd->cp;
        }
    }
    else { // cmd->cp == 0
        debug_print("  process_intcmd:RoundRobin\n\0");
        // pick next tasklet in the tasklet slots
        uint32_t i = (tq->curr_slot + 1) % NUM_TASKLET_SLOTS;
        while (i != tq->curr_slot) {
            if ((tq->slots[i].active_save & 0x0F) != 0) {
                debug_print("  process_int_cmd: switich to new cp @\0");
                print_hexuint64(tq->slots[i].cp);
                return tq->slots[i].cp;
            }
            i = (i+1) % NUM_TASKLET_SLOTS;
        }
    }
    debug_print("  process_int_cmd: return:0\n\0");
    return 0;
}

void end_hart0() {
    int i, j;
    uint64_t sum = 0;
    for (i = 0; i < 1000000; i++)
        for (j = 0; j < 10; j++) sum++;
    if (sum > 1000000000) {
    debug_print("end0\n\0");
    } else {
    debug_print("end0\n\0");
    }
}

void test_hart0() {
    uint64_t i, j;
    uint32_t * clint_addr = (uint32_t *)(CLINT_BASE);
    debug_print("hart0:\n\0");
    for (i = 0; i < 10000000; i++)  j++;
    trigger_int(1, CONFIG_TASKLET_ADDR(1), 0, (1 | 0 << 16));
    for (i = 0; i < 10000000; i++)  j++;
    trigger_int(2, CONFIG_TASKLET_ADDR(2), 0, (1 | 0 << 16));
    for (i = 0; i < 10000000; i++)  j++;
//    trigger_int(1, CONFIG_TASKLET_ADDR(1), 1, (1 | 0 << 16));
    for (i = 0; i < 10000000; i++)  j++;
//    trigger_int(2, CONFIG_TASKLET_ADDR(2), 1, (1 | 0 << 16));
}

uint64_t monitor_main(tasklet_arg * arg) {
    int i;
    int_cmd_queue * iq;
    tasklet_queue * tq;

#ifdef DEBUG
    if (arg->tasklet_id != 900 + arg->hartid) 
        debug_print("NO\0");
    uint64_t t = (uint64_t) arg;
    print_hexuint64(t);
#endif
    volatile uint32_t * msip = (uint32_t *) CLINT_BASE;
    tq = (tasklet_queue *) &arg->tqueue;
    if (arg->entry == 0) { // initial entry
        /* initialize intQ and slotQ */
        arg->entry = 0xbeef;
        if (arg->hartid == 0) { // initialize Interrupt Queue
            for (i = 0; i < MAX_CAES; i++) {
                iq = (int_cmd_queue *) INT_MSGQ_ADDR(i);
                iq->front = iq->back = 0;
            }
        }
        tq->low = tq->high = tq->curr_slot = tq->max_slots = tq->n_active = 0;
#ifndef INTEL
        asm volatile ("csrr %0, mhartid" : "=r" (arg->hartid));
        asm volatile ("csrw mcycle, 0" : : );
        asm volatile ("csrw minstret, 0" : : );
        uint64_t cycles1, cycles2, instructions1, instructions2;
        asm volatile ("csrr %0, mcycle" : "=r" (cycles1) : );
        asm volatile ("csrr %0, minstret" : "=r" (instructions1) : );
        asm volatile ("csrr %0, mcycle" : "=r" (cycles2) : );
        asm volatile ("csrr %0, minstret" : "=r" (instructions2) : );
//        print_hexuint64(cycles2 - cycles1);
//        print_hexuint64(instructions2 - instructions1);
#endif
        debug_print("monintor Init at hart\0");
        char ci = arg->hartid;
        ci = ci + '0';
        debug_putc(ci);
        debug_putc('\n');
    }
#if DEBUG
    else {	// debugging purpose
        debug_print("montor ReEntry at hart");
        char ci = arg->hartid;
        ci = ci + '0';
        debug_putc(ci); debug_putc('\n');
    }
    // print <hart id>:intDest:<interruptted hart ids>
    char ci = arg->hartid;
    ci = ci + '0';
    debug_putc(ci); debug_putc(':');
    debug_print("intDest:\0");
    char c = '0';
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
    
    iq = (int_cmd_queue *) INT_MSGQ_ADDR(arg->hartid);

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
    printf("Config size = 0x%lx < 0x%x\n", sizeof(tasklet_config) + sizeof(tasklet_arg), CONFIG_SIZE);
    for (i = 0; i < 3; i++) {
        printf("SP_ADDR(%d) = 0x%lx\n", i, SP_ADDR(i));
        printf("TP_ADDR(%d) = 0x%lx\n", i, TP_ADDR(i));
        printf("GP_ADDR(%d) = 0x%lx\n", i, GP_ADDR(i));
        printf("CODE_ADDR(%d) = 0x%lx\n", i, CODE_ADDR(i));
        printf("CONFIG_ADDR(%d) = 0x%lx\n", i, CONFIG_ADDR(i));
    }

    t1.tasklet_id = 0;
    t1.entry = argc - 1;
    printf("tasklet_arg size = 0x%lx\n", sizeof(tasklet_arg));
    monitor_main(&t1);
}
#endif
