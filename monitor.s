.include "ascii_tbl.s"
.include "hw_info.s"
.include "tasklet_config.s"

.equ DEBUG_ALL, 0
.equ DEBUG, 0
.equ TEST_INT, 0

.section .text
.extern monitor_main
.extern test_hart0
.extern end_hart0

.globl _start
_start:

    # Clear all interrupt enable bits (for monitor only)
    li a1, 0xFFFFFFFF       # Load immediate value with all bits set
    csrc mie, a1            # Clear all bits in MIE register

    # Initialize from configuration memory
    li a1, BASE_ADDR_MONITOR
    li a2, CONFIG_MEM_SIZE 
    csrr a0, mhartid	# Hart ID
    mul a2, a2, a0
    add a1, a1, a2	# base address of CONFIG_MEM of the hart
    ld sp, SP_OFFSET(a1)
    ld tp, TP_OFFSET(a1)
    ld gp, GP_OFFSET(a1)
    sw a0, TASKLET_HARTID_OFFSET(a1)
    add fp, sp, 0

    lw a2, TASKLET_ENTRY_OFFSET(a1)	# entry (initially 0, otherwise nonzero)

.if DEBUG_ALL # expects (hartid)(empty)/n
    li a1, UART_BASE 
    andi a3, a3, 0	# hartid
    addi a3, a0, 48
    sw a3, 0(a1)
    andi a3, a3, 0	# empty
    addi a3, a2, 48
    sw a3, 0(a1)
    andi a3, a3, 0	# newline
    addi a3, a3, ASCII_newln
    sw a3, 0(a1)
.endif
#    andi a0, a0, 0
    beq a2, x0, init_setup # skip init_setup

    j monitor

init_setup:
    # a1 has BASE_ADDR_MONITOR of hartid
#    csrr a0, mhartid	# Hart ID
#    sw a0, TASKLET_HARTID_OFFSET(a1)	# entry (hartid)

.if DEBUG_ALL	# expects I(hart id)/n
    csrr a0, mhartid	# Hart ID
    li a1, UART_BASE 
    andi a3, a3, 0	
    addi a3, a3, ASCII_I
    sw a3, 0(a1)
    andi a3, a3, 0	# hartid
    addi a3, a0, 48
    sw a3, 0(a1)
    andi a3, a3, 0	# empty
    addi a3, a3, ASCII_newln
    sw a3, 0(a1)
.endif

    # Set the mtvec register to the address of the trap vector
    la a1, trap_vector
    csrw mtvec, a1

    # Clear Timer Interrupt (To kill timer interrupt. Is it needed?)
    csrr a0, mhartid	# Hart ID
    li a1, CLINT_BASE   # Load base address of CLINT
    li a2, MTIMECMP_OFFSET # Offset for mtimecmp register
    add a1, a1, a2      # Calculate address of mtimecmp register

    li a3, MTIMECMP_OFFSET_HART # Offset per Hart (e.g., 4 KB)
    mul a4, a0, a3 # a4 = Hart ID * Offset per Hart
    add a1, a1, a4 # a1 = Base MTIMECMP Address + (Hart ID * Offset per Hart)

    li a2, -1           # Load the maximum value to clear the timer interrupt
    sd a2, 0(a1)        # Store value in mtimecmp to disable timer interrupt

    j monitor

trap_vector:	# ISR
    # Save all general-purpose registers on the stack of interrupted task
    addi sp, sp, -33*8      # Allocate space on stack (32 registers + 1 for return address)
    sd ra, 0(sp)            # Save return address
    sd t0, 1*8(sp)          # Save temporary registers
    sd t1, 2*8(sp)
    sd t2, 3*8(sp)
    sd t3, 4*8(sp)
    sd t4, 5*8(sp)
    sd t5, 6*8(sp)
    sd t6, 7*8(sp)
    sd a0, 8*8(sp)          # Save argument registers
    sd a1, 9*8(sp)
    sd a2, 10*8(sp)
    sd a3, 11*8(sp)
    sd a4, 12*8(sp)
    sd a5, 13*8(sp)
    sd a6, 14*8(sp)
    sd a7, 15*8(sp)
    sd s0, 16*8(sp)         # Save saved registers, s0 == fp
    sd s1, 17*8(sp)
    sd s2, 18*8(sp)
    sd s3, 19*8(sp)
    sd s4, 20*8(sp)
    sd s5, 21*8(sp)
    sd s6, 22*8(sp)
    sd s7, 23*8(sp)
    sd s8, 24*8(sp)
    sd s9, 25*8(sp)
    sd s10, 26*8(sp)
    sd s11, 27*8(sp)
    sd gp, 28*8(sp)         # Save global pointer
    sd tp, 29*8(sp)         # Save thread pointer
#    sd fp, 30*8(sp)         # Save frame pointer  --> s0
    csrr a1, mepc           # Save the machine exception program counter (current PC)
    sd a1, 31*8(sp)         # Save PC

    # check if it is software interrupt. If not, mret
    csrr a1, mcause          # Read the cause of the trap
    bgez a1, handle_exception # it is exception

.if DEBUG_ALL
    li a3, UART_BASE 
    li a2, ASCII_C
    sw a2, 0(a3)
    andi a2, a2, 0
    addi a2, a1, 48
    sw a2, 0(a3)
    andi a2, a2, 0
    addi a2, a2, ASCII_newln 
    sw a2, 0(a3)
.endif

    andi a1, a1, 0x3f	# mask interrupt bit at the top
    li   a2, SOFTWARE_INT
    bne a1, a2, handle_other_traps # if != 3 (handle_other_traps)

handle_software_int:
.if DEBUG_ALL	# ret
    # print ret
    li a1, UART_BASE 
    andi a2, a2, 0
    addi a2, a2, ASCII_s
    sw a2, 0(a1)
    andi a2, a2, 0
    addi a2, a2, ASCII_newln 
    sw a2, 0(a1)
.endif
    
    # calculate config memory start address 
    li a1, BASE_ADDR_MONITOR
    li a2, CONFIG_MEM_SIZE 
    csrr a0, mhartid	# Hart ID
    mul a2, a2, a0
    add a1, a1, a2	# a1 has base address of config memory
    ld tp,TP_OFFSET(a1)
    ld gp,GP_OFFSET(a1)


nextl2:
.if 0
    # TODO: We should have a separate stack of monitor
    # For now, Let's use the interrupted task's stack
    # the code below overwrites its own stack when monitor is interrupted.
    # save interrupted task's sp
#    ld  a1, 0(a1)	# sp of monitor
#    sd	sp, -8(a1)	# store the current sp at the top
    # use stack of the monitor
    # set up sp, tp, gp from config memory
#    addi sp, a1, -8	# change sp
#    addi fp, sp, 0
.endif
    addi a0, a1, TASKLET_ARG_OFFSET # offset from the start of config memory
    call monitor_main
    # now a0 has the address of tasklet config_data if switching is to happen

.if 0
    # recover sp
#    ld sp, 0(sp)
.endif

.if DEBUG # ret
    # print ret
    li a1, UART_BASE 
    andi a2, a2, 0
    addi a2, a2, ASCII_r
    sw a2, 0(a1)
    andi a2, a2, 0
    addi a2, a2, ASCII_e 
    sw a2, 0(a1)
    andi a2, a2, 0
    addi a2, a2, ASCII_t 
    sw a2, 0(a1)
    andi a2, a2, 0
    addi a2, a2, ASCII_newln 
    sw a2, 0(a1)
.endif
    
    # Clear software interrupt
    csrr a2, mhartid	# Hart ID
    li a1, CLINT_BASE # CLINT base address:Load address of MSIP register into a1
    slli a2, a2, 2 	# a2 = hart_id * 4
    add a1, a1, a2
    li a2, 0         # Load immediate value 0 into a2
    sw a2, 0(a1)     # Write 1 to MSIP register to clear software interrupt

    # TODO: if the interrupted tasklet is the tasklet to be switched
    #       do not start the tasklet newly, but simply return to it.
    j final_mret     # a0 has the value of return PC if non-zero

.if DEBUG # ret
    # print ret
    li a1, UART_BASE 
    andi a2, a2, 0
    addi a2, a2, ASCII_T
    sw a2, 0(a1)
    addi a2, a2, ASCII_newln 
    sw a2, 0(a1)
.endif

handle_exception:	# print E(code)
.if 0
    csrr a0, mhartid	# Hart ID
    li a3, UART_BASE 
    addi a2, a0, ASCII_0
    sw a2, 0(a3)
    andi a2, a2, 0
    addi a2, a2, ASCII_E
    sw a2, 0(a3)

    addi a1, a1, ASCII_0	# code + ASCII_0
    sw a1, 0(a3)
.endif
    csrr t0, mtval

    andi a2, a2, 0
    addi a2, a2, ASCII_newln 
    sw a2, 0(a3)

    mv a0, x0
    j final_mret                  # return from interrupt
    
handle_other_traps:
.if DEBUG_ALL	# ret
    # print ret
    li a1, UART_BASE 
    andi a2, a2, 0
    addi a2, a2, ASCII_o
    sw a2, 0(a1)
    andi a2, a2, 0
    addi a2, a2, ASCII_newln 
    sw a2, 0(a1)
.endif

    mv a0, x0
    j final_mret

external_interrupt_handler:
    # Acknowledge the interrupt, for example by clearing the pending bit
    # in the PLIC (Platform-Level Interrupt Controller)
    li a1, 0xc000000  # Replace with actual PLIC base address
    sw zero, 0(a1)              # Clear the interrupt
    mv a0, x0

.if DEBUG	# ret
    # print ret
    li a1, UART_BASE 
    andi a2, a2, 0
    addi a2, a2, ASCII_e
    sw a2, 0(a1)
    andi a2, a2, 0
    addi a2, a2, ASCII_newln 
    sw a2, 0(a1)
.endif
    
    # Add code to handle the specific interrupt here

final_mret:	# a0 has the return PC value if a0 is nonzero
    # Restore all general-purpose registers
    ld a1, 31*8(sp)         # Restore PC
    csrw mepc, a1           # Write back to mepc
    beq a0, x0, target_return	# normal return from ISR
  
    # Jump to new tasklet
    la a1, _tasklet_start
    csrw mepc, a1           # Write back to mepc with target tasklet
    mret 		    # no need to recover registers

target_return:
    ld ra, 0(sp)            # Restore return address
    ld t0, 1*8(sp)          # Restore temporary registers
    ld t1, 2*8(sp)
    ld t2, 3*8(sp)
    ld t3, 4*8(sp)
    ld t4, 5*8(sp)
    ld t5, 6*8(sp)
    ld t6, 7*8(sp)
    ld a0, 8*8(sp)          # Restore argument registers
    ld a1, 9*8(sp)
    ld a2, 10*8(sp)
    ld a3, 11*8(sp)
    ld a4, 12*8(sp)
    ld a5, 13*8(sp)
    ld a6, 14*8(sp)
    ld a7, 15*8(sp)
    ld s0, 16*8(sp)         # Restore saved registers, s0 == fp
    ld s1, 17*8(sp)
    ld s2, 18*8(sp)
    ld s3, 19*8(sp)
    ld s4, 20*8(sp)
    ld s5, 21*8(sp)
    ld s6, 22*8(sp)
    ld s7, 23*8(sp)
    ld s8, 24*8(sp)
    ld s9, 25*8(sp)
    ld s10, 26*8(sp)
    ld s11, 27*8(sp)
    ld gp, 28*8(sp)         # Restore global pointer
    ld tp, 29*8(sp)         # Restore thread pointer
#    ld fp, 30*8(sp)         # Restore frame pointer

    addi sp, sp, 33*8       # Deallocate stack space
    # Return from the interrupt
    mret

monitor:
    # TODO: call monitor_main() to handle direct request (not via interrut)

.if DEBUG_ALL	# expects M(hart id)/n
    csrr a0, mhartid	# Hart ID
    li a1, UART_BASE 
    andi a3, a3, 0	
    addi a3, a3, ASCII_M
    sw a3, 0(a1)
    andi a3, a3, 0	# hartid
    addi a3, a0, 48
    sw a3, 0(a1)
    andi a3, a3, 0	# newln
    addi a3, a3, ASCII_newln
    sw a3, 0(a1)

.endif

    # Enable global interrupts (MIE)
    csrsi mstatus, 0x8

    # Enable software interrupts (MSIE)
    li a1, 0x8
    csrs mie, a1

.if TEST_INT
    # test code: hart1 triggers software interrupt on Hart1 and 2
    csrr a0, mhartid	# Hart ID
    li a2, 0
    beq a0, a2, hart0 # If Hart1, trigger sw INT to Hart2 and Hart2

tloop:	# HART 1, 2: infinite loop

    wfi
    j tloop

    # HART 0: test (trigger s/w interrupt to Hart 1 and 2) and infinite loop		
hart0:
    call test_hart0
    call end_hart0

hart0_wfi:
    wfi
    j hart0_wfi
.else
    csrr a0, mhartid	# Hart ID
    li a2, 0
    bne a0, a2, hartN # If non-zero Hart, wfi

hart0:

    li a1, BASE_ADDR_MONITOR
    addi a0, a1, TASKLET_ARG_OFFSET # offset from the start of config memory
    call monitor_main

    li a2, CONFIG_MEM_SIZE 
    # run tasklet_main whose cp is at 0x81000000
    la a1, _tasklet_start
    li a0, CONFIG_MASTER_ADDR
    jalr ra, a1, 0
 
    # if tasklet_main returns, wfi
hart0_wfi:
    wfi
    j hart0_wfi

hartN: 
    # then busy loop
hartN_wfi:
    j hartN_wfi

.endif


