.include "ascii_tbl.s"
.include "hw_info.s"
.include "tasklet_config.s"

.section .text
.extern print_hexuint64
.globl _tasklet_start
_tasklet_start:
    # a0 has the start address of configuration memory
    ld a1, PC_OFFSET(a0)
    ld sp, SP_OFFSET(a0)
    ld tp, TP_OFFSET(a0)
    ld gp, GP_OFFSET(a0)
    mv s2, a0
    addi s2, s2, TASKLET_ARG_OFFSET # start address of tasklet argument
.if 0 # debugging
    mv a0, a1
    call print_hexuint64
    li a2, UART_BASE 
    li a3, ASCII_T
    sw a3, 0(a2)
    li a3, ASCII_S
    sw a3, 0(a2)
    li a3, ASCII_newln
    sw a3, 0(a2)
.endif
    mv a0, s2
    jalr ra, 0(a1)
    # at return, you may call monitor or just infinite loop
loop:
    j loop

