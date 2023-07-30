#include "lib.h"
#include "process.h"
#include "microbian.h"
#include <string.h>

proc os_ptable[NPROCS];
unsigned os_nprocs = 0;

unsigned long score(proc p){return p->n_ticks/p->n_calls;}

/* create_proc -- allocate and initialise process descriptor */
proc create_proc(char *name, unsigned stksize) {
    int pid;
    proc p;
    unsigned char *stack;
    unsigned *sp;

    if (os_nprocs >= NPROCS)
        panic("Too many processes");

    /* Allocate descriptor and stack space */
    pid = os_nprocs++;
    p = os_ptable[pid] = new_proc();
    stack = sbrk(stksize);
    sp = (unsigned *) &stack[stksize];

    /* Blank out the stack space to help detect overflow */
    for (unsigned *p = (unsigned *) stack; p < sp; p++) *p = BLANK;

    /* Fill in fields of the descriptor */
    p->pid = pid;
    strncpy(p->name, name, 15);
    p->name[15] = '\0';
    p->sp = sp;
    p->stack = stack;
    p->stksize = stksize;
    p->state = ACTIVE;
    p->priority = P_LOW;
    p->waiting = 0;
    p->pending = 0;
    p->filter = ANY;
#ifdef _TIMEOUT
    p->timeout = NO_TIME;
#endif
    p->msgbuf = NULL;
    p->next = NULL;

    return p;
}



/* STORAGE ALLOCATION */

/* Stack space for processes is allocated from the low end of the
space between the static data and the main stack.  Process
descriptors are allocated from the opposite end of the space; this
is deliberate to reduce the likelihood that a process overrunning
its stack space will destroy its own descriptor or that of its
neighbour. */

extern unsigned char __stack_limit[], __end[];

unsigned char *hbot = __end;
unsigned char *htop = __stack_limit;

#define ROUNDUP(x, n)  (((x)+(n)-1) & ~((n)-1))

/* sbrk -- allocate space at the bottom of the heap */
void *sbrk(int inc) {
    hbot = (unsigned char *) ROUNDUP((unsigned) hbot, 8);
    inc = ROUNDUP(inc, 8);

    if (inc > htop - hbot)
        panic("Microbian is out of memory");
    void *result = hbot;
    hbot += inc;
    return result;
}

/* new_proc -- allocate a process descriptor from the top of the heap */
proc new_proc(void) {
    if (htop - hbot < sizeof(struct _proc))
        panic("No space for process");
    htop -= sizeof(struct _proc);
    return (proc) htop;
}