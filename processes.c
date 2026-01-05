#include "serial.h" 
#include "processes.h" 
#include "scheduler.h"
#include "process.h"
#include "memory.h"

/* Producer process - sends messages */
void procA(void) {
    serial_puts("\n[ProcA] Producer started\n");
    
    for (int i = 1; i <= 5; i++) {
        serial_puts("[ProcA] Producing item ");
        serial_putu(i);
        serial_puts("\n");
        
        // Send message to procB (PID 2)
        if (process_send(2, i * 100) == 0) {
            serial_puts("[ProcA] Sent value ");
            serial_putu(i * 100);
            serial_puts(" to ProcB\n");
        }
        
        // Simulate work
        for (volatile int j = 0; j < 500000; j++);
        
        schedule();
    }
    
    serial_puts("[ProcA] Producer finished - exiting\n");
    process_exit();
}

/* Consumer process - receives messages */
void procB(void) {
    serial_puts("\n[ProcB] Consumer started\n");
    
    for (int i = 1; i <= 5; i++) {
        serial_puts("[ProcB] Waiting for item...\n");
        
        uint32_t value;
        if (process_receive(&value) == 0) {
            serial_puts("[ProcB] Received value ");
            serial_putu(value);
            serial_puts("\n");
            serial_puts("[ProcB] Processing...\n");
            
            // Simulate processing
            for (volatile int j = 0; j < 500000; j++);
        }
        
        schedule(); 
    }
    
    serial_puts("[ProcB] Consumer finished - exiting\n");
    process_exit();
}

/* Memory tester process */
void procC(void) {
    serial_puts("\n[ProcC] Memory tester started\n");
    
    // Allocate some memory
    serial_puts("[ProcC] Allocating 128 bytes...\n");
    uint32_t *data = (uint32_t*)alloc_mem(128);
    
    if (data) {
        serial_puts("[ProcC] Memory allocated successfully\n");
        
        // Write some data
        for (int i = 0; i < 10; i++) {
            data[i] = i * 10;
        }
        
        serial_puts("[ProcC] Wrote data: ");
        for (int i = 0; i < 10; i++) {
            serial_putu(data[i]);
            serial_puts(" ");
        }
        serial_puts("\n");
        
        schedule(); 
        
        serial_puts("[ProcC] Verifying data: ");
        int correct = 1;
        for (int i = 0; i < 10; i++) {
            if (data[i] != i * 10) {
                correct = 0;
                break;
            }
        }

        // Free memory
        serial_puts("[ProcC] Freeing memory...\n");
        free_mem(data, 128);
    } else {
        serial_puts("[ProcC] Memory allocation failed!\n");
    }
    
    serial_puts("[ProcC] Memory test complete - exiting\n");
    process_exit();
}

/* Status reporter - shows system state */
void procD(void) {
    serial_puts("\n[ProcD] Status reporter started\n");
    
    for (int i = 1; i <= 3; i++) {
        serial_puts("\n[ProcD] ===== System Status Report #");
        serial_putu(i);
        serial_puts(" =====\n");
        
        // Report on each process
        for (int pid = 0; pid < MAX_PROCESSES; pid++) {
            pcb_t *p = process_get(pid);
            if (p && p->state != PROC_UNUSED) {
                serial_puts("[ProcD] PID ");
                serial_putu(pid);
                serial_puts(": State=");
                
                switch(p->state) {
                    case PROC_READY:   serial_puts("READY  "); break;
                    case PROC_RUNNING: serial_puts("RUNNING"); break;
                    case PROC_BLOCKED: serial_puts("BLOCKED"); break;
                    case PROC_ZOMBIE:  serial_puts("ZOMBIE "); break;
                    default:           serial_puts("UNKNOWN"); break;
                }
                
                serial_puts(" Priority=");
                serial_putu(p->priority);
                serial_puts(" Age=");
                serial_putu(p->age);
                serial_puts("\n");
            }
        }
        
        serial_puts("[ProcD]\n\n");
        
        // Wait 
        for (volatile int j = 0; j < 1000000; j++);
        schedule();
    }
    
    serial_puts("[ProcD] Status reporter finished\n");
    process_exit();
}
