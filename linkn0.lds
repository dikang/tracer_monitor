OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
  ram   (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 128M
}
MEMORY
{
  TEXT (rx) : ORIGIN = 0x80000000, LENGTH = 0x00300000
  DATA0 (rw) : ORIGIN = 0x80400000, LENGTH = 0x40000
  DATA1 (rw) : ORIGIN = 0x80800000, LENGTH = 0x40000
  DATA2 (rw) : ORIGIN = 0x80c00000, LENGTH = 0x40000
  DATA3 (rw) : ORIGIN = 0x81000000, LENGTH = 0x40000
}

PHDRS
{
  text PT_LOAD;
  data PT_LOAD;
  bss PT_LOAD;
}

SECTIONS
{
  . = 0x80000000;
  .text : {
    PROVIDE(_text._start = .);
    *(.text .text.*) 
    PROVIDE(_text._end = .);
  } > TEXT

  . = 0x80400000;
  .data0 :{
    PROVIDE(_data0_start = .);
    *(.data0) 
    PROVIDE(_data0_end = .);
  } > DATA0

  . = 0x80800000;
  .data1 :{
    PROVIDE(_data1_start = .);
    *(.data1) 
    PROVIDE(_data1_end = .);
  } > DATA1

  . = 0x80c00000;
  .data2 :{
    PROVIDE(_data2_start = .);
    *(.data2) 
    PROVIDE(_data2_end = .);
  } > DATA2

  . = 0x81000000;
  .data3 :{
    PROVIDE(_data3_start = .);
    *(.data3) 
    PROVIDE(_data3_end = .);
  } > DATA3

  .rodata : {
    PROVIDE(_rodata_start = .);
    *(.rodata .rodata.*)
    PROVIDE(_rodata_end = .);
  } >ram AT>ram :text


  PROVIDE(_memory_start = ORIGIN(ram));
  PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));
}
