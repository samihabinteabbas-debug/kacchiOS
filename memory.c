#include "memory.h"
#include "serial.h"

#define MEM_START 0x80000000
#define MEM_END   0x80020000

/* ---------------- stack ---------------- */

static uint32_t stack_top;

/* ---------------- heap ---------------- */

typedef struct memblk {
    struct memblk *next;
    uint32_t size;
} memblk_t;

static memblk_t *freelist;

void memory_init(void) {
    stack_top = MEM_END;

    freelist = (memblk_t*)MEM_START;
    freelist->size = MEM_END - MEM_START;
    freelist->next = 0;

    serial_puts("[mem] initialized\n");
}

void* alloc_stack(void) {
    stack_top -= STACKSIZE;

    serial_puts("[mem] stack alloc\n");
    return (void*)stack_top;
}

void free_stack(void *addr) {
    stack_top = (uint32_t)addr;

    serial_puts("[mem] stack free\n");
}

/* ---------------- heap ---------------- */

void* alloc_mem(uint32_t size) {
    memblk_t *prev = 0;
    memblk_t *curr = freelist;

    while (curr) {
        if (curr->size >= size) {
            if (curr->size == size) {
                if (prev) prev->next = curr->next;
                else freelist = curr->next;
            } else {
                memblk_t *next =
                    (memblk_t*)((uint32_t)curr + size);
                next->size = curr->size - size;
                next->next = curr->next;

                if (prev) prev->next = next;
                else freelist = next;
            }

            serial_puts("[mem] heap alloc\n");
            return (void*)curr;
        }
        prev = curr;
        curr = curr->next;
    }
    return 0;
}

void free_mem(void *addr, uint32_t size) {
    memblk_t *blk = (memblk_t*)addr;
    blk->size = size;
    blk->next = freelist;
    freelist = blk;

    serial_puts("[mem] heap free\n");
}
