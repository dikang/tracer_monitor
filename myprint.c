#include <stdint.h>
#include "tasklet_config.h"

void debug_print(char * s) {
    int i = 0;
    while (s[i] != '\0') {
        debug_putc(s[i]);
        i++;
    }
}

void debug_putc(unsigned char c) {
#ifdef INTEL
    printf("%1c", c);
#else
    unsigned int * uart = UART_BASE;
    *uart = c;
#endif
}

void print_hexuint64(uint64_t t) {
    unsigned char * c;
    unsigned int uc, lc, i, j;
    unsigned char oc[sizeof(t)*2];
   
    c = (unsigned char *)&t;
    for (i = 0, j = 0; i < sizeof(t); i++) {
        uc = (*c >> 4);
        lc = (*c & 0xF);
        oc[j++] = uc;
        oc[j++] = lc;
        c++;
    }
    for (i = 0; i < sizeof(t)*2; i++) {
        if (oc[i] < 10) 
            oc[i] = 48 + oc[i];
        else 
            oc[i] = 97 + oc[i] - 10;
    }
 
    debug_putc(48);
    debug_putc(120);

    for (i = sizeof(t)*2; i > 0; i -= 2) {
        debug_putc(oc[i-2]);
        debug_putc(oc[i-1]);
    }
    debug_putc('\n');
}

#ifdef INTEL
main() {
    uint64_t a = 0x80102345;
    print_hexint(a); 
}

#endif
