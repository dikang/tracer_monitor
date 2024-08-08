#include "tasklet_config.h"
#ifndef __UTILITY_H__
#define __UTILITY_H__


#ifndef INTEL
extern void print_hexuint64(uint64_t t);
extern void debug_putc(unsigned char c);
extern void debug_print(char * s);
extern void trigger_int(uint32_t , uint64_t , uint32_t , uint32_t );
#else
void print_hexuint64(uint64_t t) { }
void debug_putc(unsigned char c) { }
void debug_print(char * s) { }
void trigger_int(uint32_t , uint64_t , uint32_t , uint32_t ) {};
#endif
#endif	
