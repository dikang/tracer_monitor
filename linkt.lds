
OUTPUT_ARCH( "riscv" )

ENTRY( tasklet_main )

MEMORY
{
  ram   (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 128M
  DATA_TASKLET (rw)  : ORIGIN = 0x81200000, LENGTH = 0x200000
  DATA_TASKLET0 (rw) : ORIGIN = 0x81400000, LENGTH = 0x400000
  DATA_TASKLET1 (rw) : ORIGIN = 0x81800000, LENGTH = 0x400000
  DATA_TASKLET2 (rw) : ORIGIN = 0x81c00000, LENGTH = 0x400000
  DATA_TASKLET3 (rw) : ORIGIN = 0x82000000, LENGTH = 0x400000
  TEXT (rx)  : ORIGIN = 0x87000000, LENGTH = 0x180000
}

PHDRS
{
  data_tasklet0 PT_LOAD;
  data_tasklet1 PT_LOAD;
  data_tasklet2 PT_LOAD;
  data_tasklet3 PT_LOAD;
  text PT_LOAD;
  data PT_LOAD;
  bss PT_LOAD;
  rodata PT_LOAD;
  got PT_LOAD;
}

SECTIONS
{
  . = 0x81400000;
  .data_tasklet0 :{
    PROVIDE(_data_tasklet0_start = .);
    *(.data_tasklet0) 
    PROVIDE(_data_tasklet0_end = .);
  } > DATA_TASKLET0 : data_tasklet0

  . = 0x81800000;
  .data_tasklet1 :{
    PROVIDE(_data_tasklet1_start = .);
    *(.data_tasklet1) 
    PROVIDE(_data_tasklet1_end = .);
  } > DATA_TASKLET1 : data_tasklet1

  . = 0x81c00000;
  .data_tasklet2 :{
    PROVIDE(_data_tasklet2_start = .);
    *(.data_tasklet2) 
    PROVIDE(_data_tasklet2_end = .);
  } > DATA_TASKLET2 : data_tasklet2

  . = 0x82000000;
  .data_tasklet3 :{
    PROVIDE(_data_tasklet3_start = .);
    *(.data_tasklet3) 
    PROVIDE(_data_tasklet3_end = .);
  } > DATA_TASKLET3 : data_tasklet3

  . = 0x87080000;
  .text : {
    PROVIDE(_text._start = .);
    *(.text .text.*) 
    PROVIDE(_text._end = .);
  } > TEXT : text

  .data : AT(ADDR(.text) + SIZEOF(.text)) {
    PROVIDE(_data_start = .);
    *(.data .data.*)
    PROVIDE(_data_end = .);
  } > TEXT : data

  .bss : {
    PROVIDE(_bss_start = .);
    *(.bss .bss.*)
    *(COMMON)
    PROVIDE(_bss_end = .);
  } > TEXT : bss

  .rodata : {
    PROVIDE(_rodata_start = .);
    *(.rodata .rodata.*)
    PROVIDE(_rodata_end = .);
  } > TEXT : rodata 

  .got : {
    PROVIDE(_got_start = .);
    *(.got .got.*)
    PROVIDE(_got_end = .);
  } > TEXT : got



  PROVIDE(_memory_start = ORIGIN(ram));
  PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));
}
