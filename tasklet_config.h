#ifndef __TASKLET_CONFIG_H__
#define __TASKLET_CONFIG_H__
#include <stdint.h>

#define MAX_CAES 3
#define UART_BASE 0x10000000UL
#define CLINT_BASE 0x2000000UL
#define MTIMECMP_OFFSET 0x4000
#define MTIMECMP_OFFSET_HART 0x1000

/* ---------------------------------------------------------------------

        Monitor tasklet description
 
 * --------------------------------------------------------------------- */
/* Memory layout of Monitor tasklets 
 *    Code: 0x80000000 - 0x800300000 : is shared by all monitors
 *    config_memory @hart(i)
 *       SP    : BASE_ADDR_MONITOR{i-1}
 *       TP    : SP - STACK_SIZE(0x200000) - TP_SIZE(0x10000)
 *       GP    : TP - GP_SIZE(0x10000)	-- this is not needed for a regular tasklet
 *       Config: BASE_ADDR_MONITOR{i} 
 *       BASE_ADDR_MONITOR(i): BASE_ADDR_MONITOR{0} + CONFIG_MEM_SIZE(0x400000) 
 * 
 */
 
#define BASE_ADDR_MONITOR	0x80400000UL
#define CONFIG_SIZE		0x80000
#define TP_SIZE			0x80000
#define GP_SIZE			0x80000
#define CONFIG_MEM_SIZE		0x400000
#define STACK_SIZE		0x200000
#define CONFIG_ADDR(n) 	(BASE_ADDR_MONITOR + CONFIG_MEM_SIZE * (n))
#define CODE_ADDR(n) 	(0x80000000UL)
#define PC_ADDR(n) 	(CODE_ADDR((n+1)))
#define SP_ADDR(n) 	(CONFIG_ADDR((n+1)))
#define TP_ADDR(n) 	(SP_ADDR(n) - STACK_SIZE - TP_SIZE)
#define GP_ADDR(n) 	(TP_ADDR(n) - GP_SIZE)

/* ---------------------------------------------------------------------

        Master Controller tasklet description: for testing
 
 * --------------------------------------------------------------------- */
/* with MAX_CAES = 3, 
	BASE_ADDR_MASTER   = 0x81000000 
        PC_MASTER_ADDR 	   = 0x87000000
        SP_MASTER_ADDR 	   = 0x81400000
        TP_MASTER_ADDR 	   = 0x81180000
        GP_MASTER_ADDR 	   = 0x81100000
        PC_MASTER_ADDR     = 0x87000000
        CONFIG_MASTER_ADDR = 0x81000000
 */
#define BASE_ADDR_MASTER	SP_ADDR(MAX_CAES-1)
#define CONFIG_MASTER_SIZE	0x80000
#define TP_MASTER_SIZE		0x80000
#define GP_MASTER_SIZE		0x80000
#define CONFIG_MASTER_MEM_SIZE	0x400000
#define STACK_MASTER_SIZE	0x200000
#define CONFIG_MASTER_ADDR 	(BASE_ADDR_MASTER)
#define PC_MASTER_ADDR          0x87000000UL
#define SP_MASTER_ADDR	 	(CONFIG_MASTER_ADDR + CONFIG_MASTER_MEM_SIZE)
#define TP_MASTER_ADDR	 	(SP_MASTER_ADDR - STACK_MASTER_SIZE - TP_MASTER_SIZE)
#define GP_MASTER_ADDR	 	(TP_MASTER_ADDR - GP_MASTER_SIZE)

/* ---------------------------------------------------------------------

			       Appl. tasklets
 
 * --------------------------------------------------------------------- */
/* with MAX_CAES = 3, 
	BASE_ADDR_TASKLET = 0x81400000
        SP_TASKLET_ADDR = 0x81800000
        TP_TASKLET_ADDR = 0x81180000
        CONFIG_TASKLET_ADDR = 0x81400000
 */
#define BASE_ADDR_TASKLET	SP_MASTER_ADDR
#define CONFIG_TASKLET_SIZE	0x80000
#define TP_TASKLET_SIZE		0x80000
#define CONFIG_TASKLET_MEM_SIZE	0x400000
#define STACK_TASKLET_SIZE	0x200000
#define CONFIG_TASKLET_ADDR(n) 	(BASE_ADDR_TASKLET + CONFIG_TASKLET_MEM_SIZE * (n))
#define SP_TASKLET_ADDR(n) 	(CONFIG_TASKLET_ADDR(n) + CONFIG_TASKLET_MEM_SIZE)
#define TP_TASKLET_ADDR(n) 	(SP_TASKLET_ADDR(n) - STACK_TASKLET_SIZE - TP_TASKLET_SIZE)

/* ---------------------------------------------------------------------

			       Interrupt
 
 * --------------------------------------------------------------------- */
#define NUM_TASKLET_SLOTS	1024
#define NUM_INTQ_SLOTS		127

/* Memory layout of interrupt exchange 
 * NxN table of entries of two 32 bit words (CMD, subCMD)
 */
typedef struct {
            uint16_t  active;	// enable, disable
            uint16_t  context_save;	// context_save or not
} tasklet_fields;

typedef struct {
     uint64_t cp;	// cp value of the tasklet
     uint32_t slot;	// slot number ??
     union {
        uint32_t active_save;
        tasklet_fields fields;
     };
} switch_cmd ;

typedef struct {
     uint64_t cp;
     union {
        uint32_t active_save;
        tasklet_fields fields;
     };
} tasklet_slot;

typedef struct {
     uint32_t front;
     uint32_t back;
     switch_cmd cmds[NUM_INTQ_SLOTS];
} int_cmd_queue;	// cyclic queue

typedef struct {
     uint32_t low;	
     uint32_t high;	
     uint32_t max_slots;
     uint32_t curr_slot;
     uint32_t n_active;	// number of active slots
     tasklet_slot slots[NUM_TASKLET_SLOTS];
} tasklet_queue;	// cyclic queue

#define INT_TABLE_SIZE	0x40

typedef struct {
    uint64_t pc;	/* PC */
    uint64_t sp;
    uint64_t tp;
    uint64_t gp;
    uint64_t int_reg[32];	/* space to save int registers */
    float    float_reg[32];	/* space to save float registers */
    /* the remaining must be the same as tasklet_arg */
    uint32_t tasklet_id;	// the start address of the arg. of tasklet function
    uint32_t entry;	// 0: new, any nonzero number: reetrant
    uint32_t hartid;	// core ID
} tasklet_config;

typedef struct {
    uint32_t tasklet_id;// the start address of the arg. of tasklet function
    uint32_t entry;	// 0: new, any nonzero number: reetrant
    uint32_t hartid;	// core ID
    tasklet_queue tqueue;
} tasklet_arg;

#define TASKLET_ARG_OFFSET	(36* sizeof(uint64_t) + 32*sizeof(float))
#define TASKLET_ENTRY_OFFSET	(TASKLET_ARG_OFFSET + sizeof(uint32_t))
#define TASKLET_HARTID_OFFSET	(TASKLET_ENTRY_OFFSET + sizeof(uint32_t))

/* Memory layout of interrupt messages */
#define BASE_ADDR_INT_MSGQ   0x803F0000UL
#define INT_MSGQ_SIZE   (sizeof(int_cmd_queue)) 
#define INT_MSGQ_ADDR(n) (BASE_ADDR_INT_MSGQ + INT_MSGQ_SIZE * n)
#define TASKLETQ_SIZE  (sizeof(tasklet_queue))

/* How to trigger switching
 *
 *    disable_interrupt()	- to avoid being kicked out after locking
 *    lock(target_hart_mutex)
 *    add switch command
 *    unlock(target_hart_mutex)
 *    enable_interrupt()
 *
 */
#endif
