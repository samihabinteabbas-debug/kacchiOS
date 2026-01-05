/* serial.h - Serial port driver interface */
#ifndef SERIAL_H
#define SERIAL_H

#include "types.h"

void serial_init(void);
void serial_putc(char c);
void serial_puts(const char* str);
char serial_getc(void);
void serial_putu(unsigned int x);
void serial_puthex(uint32_t val) {
    char hex[] = "0123456789ABCDEF";
    for (int i = 28; i >= 0; i -= 4) {
        serial_putc(hex[(val >> i) & 0xF]);
    }
}
#endif
