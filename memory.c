#include "memory.h"
#include "serial.h"

#define MEM_START 0x80000000
#define MEM_END   0x80020000
typedef struct {
    void *base;
    int used;
} stack_region_t;

#define MAX_STACKS MAX_PROCESSES

static stack_region_t stack_table[MAX_STACKS];

/* ---------------- stack ---------------- */

static uint32_t stack_top;

/* ---------------- heap ---------------- */

typedef struct memblk {
    struct memblk *next;
    uint32_t size;
} memblk_t;

static memblk_t *freelist;

void memory_init(void) {
    uint32_t addr = MEM_END;

    for (int i = 0; i < MAX_STACKS; i++) {
        addr -= STACKSIZE;
        stack_table[i].base = (void*)addr;
        stack_table[i].used = 0;
    }

    freelist = (memblk_t*)MEM_START;
    freelist->size = addr - MEM_START;
    freelist->next = 0;

    serial_puts("[mem] initialized\n");
}

void* alloc_stack(void) {
    for (int i = 0; i < MAX_STACKS; i++) {
        if (!stack_table[i].used) {
            stack_table[i].used = 1;
            serial_puts("[mem] stack alloc\n");
            return (uint32_t*)stack_table[i].base + STACKSIZE / sizeof(uint32_t);
        }
    }
    return 0;  // no free stack
}

void free_stack(void *addr) {
    for (int i = 0; i < MAX_STACKS; i++) {
        uint32_t *top =
            (uint32_t*)stack_table[i].base + STACKSIZE / sizeof(uint32_t);

        if (top == addr) {
            stack_table[i].used = 0;
            serial_puts("[mem] stack free\n");
            return;
        }
    }
}


/* ---------------- heap ---------------- */

typedef struct memblk {
    struct memblk *next;
    uint32_t size;          
} memblk_t;

static memblk_t *freelist;

void* alloc_mem(uint32_t size) {
    memblk_t *prev = 0;
    memblk_t *curr = freelist;

    uint32_t total = size + sizeof(memblk_t);

    while (curr) {
        if (curr->size >= total) {

            /* Exact fit */
            if (curr->size == total) {
                if (prev)
                    prev->next = curr->next;
                else
                    freelist = curr->next;
            }
            /* Split block */
            else {
                memblk_t *next =
                    (memblk_t*)((uint32_t)curr + total);

                next->size = curr->size - total;
                next->next = curr->next;

                if (prev)
                    prev->next = next;
                else
                    freelist = next;

                curr->size = total;
            }

            serial_puts("[mem] heap alloc\n");
            return (void*)((uint32_t)curr + sizeof(memblk_t));
        }

        prev = curr;
        curr = curr->next;
    }

    return 0;   
}

/* Free heap memory with coalescing */
void free_mem(void *addr, uint32_t size) {
    if (!addr)
        return;

    memblk_t *blk =
        (memblk_t*)((uint32_t)addr - sizeof(memblk_t));

    blk->size = size + sizeof(memblk_t);

    memblk_t *prev = 0;
    memblk_t *curr = freelist;

    /* Insert block in sorted address order */
    while (curr && curr < blk) {
        prev = curr;
        curr = curr->next;
    }

    blk->next = curr;
    if (prev)
        prev->next = blk;
    else
        freelist = blk;

    /* Coalesce with next block */
    if (blk->next &&
        (uint32_t)blk + blk->size == (uint32_t)blk->next) {
        blk->size += blk->next->size;
        blk->next = blk->next->next;
    }

    /* Coalesce with previous block */
    if (prev &&
        (uint32_t)prev + prev->size == (uint32_t)blk) {
        prev->size += blk->size;
        prev->next = blk->next;
    }

    serial_puts("[mem] heap free (coalesced)\n");
}

