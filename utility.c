#include <stdint.h>
#include "tasklet_config.h"

void trigger_int(uint32_t hartid, uint64_t cp, uint32_t slot, uint32_t active_save) {
    // disable_interrupt();
    // lock();
    /* write the cmd to int_cmd_queue */
    int_cmd_queue *iq = (int_cmd_queue *)INT_MSGQ_ADDR(hartid); 
    debug_print("  trigger_int: iq addr = \0");
    uint64_t addr = (uint64_t) iq;
    print_hexuint64(addr);
    if (((iq->front + 1) % NUM_INTQ_SLOTS) == iq->back) { // full
       debug_print("  trigger_int: iq full\n\0");
       // unlock();
       // enable_interrupt();
        return; 
    }
    switch_cmd * cmd = &iq->cmds[iq->front];
    cmd->cp = cp;
    cmd->slot = slot;
    cmd->active_save = active_save;
    iq->front++;

    debug_print("  trigger_int: trigger interrupt on hart \0");
    char ci = hartid;
    ci = ci + 48;
    debug_putc(ci);
    debug_print("\n");
    /* trigger interrupt */
    uint32_t clint_addr = (CLINT_BASE + 4 * hartid);
    uint32_t * clint = (uint32_t*) (clint_addr);
    * clint = 1;

    // unlock();
    // enable_interrupt();
}
