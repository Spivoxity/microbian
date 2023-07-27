#pragma once
#include "defines.h"



/* Possible priorities */
#define P_HANDLER 0             /* Interrupt handler */
#define P_HIGH 1                /* Responsive */
#define P_LOW 2                 /* Normal */
#define P_IDLE 3                /* The idle process */
#define NPRIO 3                 /* Number of non-idle priorities */

typedef struct {                /* 16 bytes */
    unsigned short type;        /* Type of message */
    unsigned short sender;      /* PID of sender */
    union {                     /* An integer, a pointer, or four bytes */
        int int1; void *ptr1;
        struct { byte byte1, byte2, byte3, byte4; };
    };
    union { int int2; void *ptr2; }; /* Another integer or pointer */
    union { int int3; void *ptr3; }; /* A third integer or pointer */
} message;


/* PROCESS DESCRIPTORS */

/* Each process has a descriptor, allocated when the process is
created.  The next field in the descriptor allows each process to be
linked into at most one queue -- either the queue of ready processes
at some priority level, or the queue of senders waiting to deliver a
message to a particular receiver process. */

typedef struct _proc *proc;

struct _proc {
    int pid;                  /* Process ID (equal to index) */
    char name[16];            /* Name for debugging */
    unsigned state;           /* SENDING, RECEIVING, etc. */
    unsigned *sp;             /* Saved stack pointer */
    void *stack;              /* Stack area */
    unsigned stksize;         /* Stack size (bytes) */
    int priority;             /* Priority: 0 is highest */
    proc waiting;             /* Processes waiting to send */
    int pending;              /* Whether HARDWARE message pending */
    int filter;               /* Message type accepted by receive */
    message *msgbuf;          /* Pointer to message buffer */
#ifdef _TIMEOUT
    int timeout;              /* Timeout for receive */
#endif

#ifdef _SCHEDULING_OPT

    unsigned long long n_ticks;
    unsigned long n_calls;
    unsigned long long age;   /* Age: how long this process has run for. Never going to overflow (too big).*/
#endif
    proc next;                /* Next process in ready or send queue */

};


/* PROCESS TABLE */

#define NPROCS 32
extern proc os_ptable[NPROCS];
extern unsigned os_nprocs;



#define BLANK 0xdeadbeef        /* Filler for initial stack */



unsigned long long score(proc p);

/* create_proc -- allocate and initialise process descriptor */
proc create_proc(char *name, unsigned stksize);

/* new_proc -- allocate a process descriptor from the top of the heap */
proc new_proc(void);

/* sbrk -- allocate space at the bottom of the heap */
void *sbrk(int inc);