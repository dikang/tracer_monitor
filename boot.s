.equ CODE_BASE, 0x80400000


.section .text
.globl _start
_start:

    li t1, CODE_BASE
    jalr x0, 0(t1)


