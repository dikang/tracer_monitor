#include <stdint.h>

void myprint(int t) {
    unsigned int * uart = 0x10000000;
    *uart = t+48;
    *uart = t+1+48;
    *uart = t+2+48;
    *uart = t+3+48;
}

void putc(unsigned char c) {
#ifdef INTEL
    printf("%1c", c);
#else
    unsigned int * uart = 0x10000000;
    *uart = c;
#endif
}

void print_hexint(uint64_t t) {
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
 
    putc(48);
    putc(120);

    for (i = sizeof(t)*2; i > 0; i -= 2) {
        putc(oc[i-2]);
        putc(oc[i-1]);
    }
    putc('\n');
}

#ifdef INTEL
main() {
    uint64_t a = 0x80102345;
    print_hexint(a); 
}

#endif
