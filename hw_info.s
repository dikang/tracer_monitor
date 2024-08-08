# Enable this for FPGA platform
.equ FPGA, 0

# h/w info
.equ CLINT_BASE, 0x2000000
.equ MTIMECMP_OFFSET, 0x4000
.equ MTIMECMP_OFFSET_HART, 0x1000
.equ SOFTWARE_INT, 0x3
.equ TIMER_INT, 0x7

.if FPGA
.equ UART_BASE, 0x60000100
.else
.equ UART_BASE, 0x10000000
.endif
