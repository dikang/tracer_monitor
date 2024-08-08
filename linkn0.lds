OUTPUT_ARCH( "riscv" )

ENTRY( _start )

MEMORY
{
  ram   (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 128M
}
MEMORY
{
  TEXT (rx) : ORIGIN = 0x80000000, LENGTH = 0x00300000
  DATA_MON0 (rw) : ORIGIN = 0x80400000, LENGTH = 0x40000
  DATA_MON1 (rw) : ORIGIN = 0x80800000, LENGTH = 0x40000
  DATA_MON2 (rw) : ORIGIN = 0x80c00000, LENGTH = 0x40000
  DATA_MASTER (rw) : ORIGIN = 0x81000000, LENGTH = 0x40000
}

PHDRS
{
  text PT_LOAD;
  data PT_LOAD;
  rodata PT_LOAD;
  bss PT_LOAD;
  data_mon0 PT_LOAD;
  data_mon1 PT_LOAD;
  data_mon2 PT_LOAD;
  data_master PT_LOAD;
}

SECTIONS
{
  . = 0x80000000;
  .text : {
    PROVIDE(_text._start = .);
    *(.text .text.*) 
    PROVIDE(_text._end = .);
  } > TEXT : text

  .data : {
    PROVIDE(_data_start = .);
    *(.data .data.*)
    PROVIDE(_data_end = .);
  } > TEXT : data

  .rodata : {
    PROVIDE(_rodata_start = .);
    *(.rodata .rodata.*)
    PROVIDE(_rodata_end = .);
  } > TEXT : rodata

  . = 0x80400000;
  .data_mon0 :{
    PROVIDE(_data_mon0_start = .);
    *(.data_mon0) 
    PROVIDE(_data_mon0_end = .);
  } > DATA_MON0

  . = 0x80800000;
  .data_mon1 :{
    PROVIDE(_data_mon1_start = .);
    *(.data_mon1) 
    PROVIDE(_data_mon1_end = .);
  } > DATA_MON1

  . = 0x80c00000;
  .data_mon2 :{
    PROVIDE(_data_mon2_start = .);
    *(.data_mon2) 
    PROVIDE(_data_mon2_end = .);
  } > DATA_MON2

  . = 0x81000000;
  .data_master :{
    PROVIDE(_data_master_start = .);
    *(.data_master) 
    PROVIDE(_data_master_end = .);
  } > DATA_MASTER


  PROVIDE(_memory_start = ORIGIN(ram));
  PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));
}
