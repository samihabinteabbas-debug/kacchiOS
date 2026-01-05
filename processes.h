#ifndef PROCESSES_H
#define PROCESSES_H

/* Demo processes */
void procA(void);  /* Producer - sends IPC messages */
void procB(void);  /* Consumer - receives IPC messages */
void procC(void);  /* Memory tester */
void procD(void);  /* Status reporter */

#endif
