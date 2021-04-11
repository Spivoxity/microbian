// microbian.c
// Copyright (c) 2018 J. M. Spivey

#include "microbian.h"
#include "lib.h"
#include "hardware.h"
#include <string.h>

// #define TIMEOUT 1


/* PROCESS DESCRIPTORS */

/* Each process has a descriptor, allocated when the process is
created.  The p_next field in the descriptor allows each process to be
linked into at most one queue -- either the queue of ready processes
at some priority level, or the queue of senders waiting to deliver a
message to a particular receiver process. */

struct proc {
    int p_pid;                  /* Process ID (equal to index) */
    char p_name[16];            /* Name for debugging */
    unsigned p_state;           /* SENDING, RECEIVING, etc. */
    unsigned *p_sp;             /* Saved stack pointer */
    void *p_stack;              /* Stack area */
    unsigned p_stksize;         /* Stack size (bytes) */
    int p_priority;             /* Priority: 0 is highest */
    
    struct proc *p_waiting;     /* Processes waiting to send */
    int p_pending;              /* Whether HARDWARE message pending */
    int p_msgtype;              /* Message type to send or recieve */
    message *p_message;         /* Pointer to message buffer */
#ifdef TIMEOUT
    int p_timeout;              /* Timeout for receive */
#endif
    struct proc *p_next;        /* Next process in ready or send queue */
};

/* Possible p_state values */
#define DEAD 0
#define ACTIVE 1
#define SENDING 2
#define RECEIVING 3
#define SENDREC 4
#define IDLING 5

#define NO_TIME 0x80000000


/* STORAGE ALLOCATION */

/* Stack space for processes is allocated from the low end of the
space between the static data and the main stack.  Process
descriptors are allocated from the opposite end of the space; this
is deliberate to reduce the likelihood that a process overrunning
its stack space will destroy its own descriptor or that of its
neighbour. */

extern unsigned char __stack_limit[], __end[];

static unsigned char *hbot = __end;
static unsigned char *htop = __stack_limit;

#define ROUNDUP(x, n)  (((x)+(n)-1) & ~((n)-1))

/* sbrk -- allocate space at the bottom of the heap */
static void *sbrk(int inc) {
    hbot = (unsigned char *) ROUNDUP((unsigned) hbot, 8);
    inc = ROUNDUP(inc, 8);

    if (inc > htop - hbot)
        panic("Microbian is out of memory");
    void *result = hbot;
    hbot += inc;
    return result;
}

/* new_proc -- allocate a process descriptor from the top of the heap */
static struct proc *new_proc(void) {
    if (htop - hbot < sizeof(struct proc))
        panic("No space for process");
    htop -= sizeof(struct proc);
    return (struct proc *) htop;
}


/* PROCESS TABLE */

#define NPROCS 32

static struct proc *os_ptable[NPROCS];
static unsigned os_nprocs = 0;

static struct proc *os_current;
static struct proc *idle_proc;

#define BLANK 0xdeadbeef        /* Filler for initial stack */

static void kprintf_setup(void);
static void kprintf_internal(char *fmt, ...);

/* pad -- pad with spaces to a specified width */
static void pad(char *buf, int width) {
    int w = strlen(buf);
    if (w < width) {
        memset(buf+w, ' ', width-w);
        buf[width] = '\0';
    }
}

/* microbian_dump -- display process states */
static void microbian_dump(void) {
    char buf[16];

    static const char *status[] = {
        "[DEAD]   ",
        "[ACTIVE] ",
        "[SEND]   ",
        "[RECEIVE]",
        "[SENDREC]",
        "[IDLE]   "
    };

    kprintf_setup();
    kprintf_internal("\r\nPROCESS DUMP\r\n");

    /* Our version of printf is a bit feeble, so the following is
       more painful than it should be. */

    for (int pid = 0; pid < os_nprocs; pid++) {
        struct proc *p = os_ptable[pid];
        unsigned *z = (unsigned *) p->p_stack;
        while (*z == BLANK) z++;
        unsigned free = (char *) z - (char *) p->p_stack;

        sprintf(buf, "%u/%u", p->p_stksize-free, p->p_stksize);
        pad(buf, 9);
        kprintf_internal("%s%d: %s %x stk=%s %s\r\n",
                         (pid < 10 ? " " : ""), pid,
                         status[p->p_state], (unsigned) p->p_stack,
                         buf, p->p_name);
    }
}


/* PROCESS QUEUES */

/* os_readyq -- one queue for each priority */
static struct queue {
    struct proc *q_head, *q_tail;
} os_readyq[NPRIO];

/* make_ready -- add process to end of the ready queue for its priority */
static inline void make_ready(struct proc *p) {
    int prio = p->p_priority;
    if (prio == P_IDLE) return;

    p->p_state = ACTIVE;
    p->p_next = NULL;

    struct queue *q = &os_readyq[prio];
    if (q->q_head == NULL)
        q->q_head = p;
    else
        q->q_tail->p_next = p;
    q->q_tail = p;
}

/* choose_proc -- the current process is blocked: pick a new one */
static inline void choose_proc(void) {
    for (int p = 0; p < NPRIO; p++) {
        struct queue *q = &os_readyq[p];
        if (q->q_head != NULL) {
            os_current = q->q_head;
            q->q_head = os_current->p_next;
            return;
        }
    }
    os_current = idle_proc;
}

/* deliver -- copy a message and fill in standard fields */
static inline void deliver(message *buf, int src, int type, message *msg) {
    if (buf) {
        if (msg) *buf = *msg;
        buf->m_sender = src;
        buf->m_type = type;
    }
}


/* TIMEOUTS */

/* Programs that include a timer may also use a form of receive() with
a timeout, so that a message of type TIMEOUT from hardware is
delivered after an interval if not genuine message has arrived.

A process p has a timeout set if p->p_timeout != NO_TIME.  Such
processes are also listed in timeout[0..n_timeouts), so we can find
them quickly on each tick.  The global ticks variable and the
p_timeout field count in ticks from the last timer update, and the
next update is due when ticks exceeds next_time.  This value can be
negative, because we delay firing timeouts until the tick after they
are due, so as to avoid firing them early.  If calls the tick() come
at regular intervals (whatever they are), then this scheme ensures
that no timer fires earlier than it should, even if the timer is set
just before a tick. */

#ifdef TIMEOUT

static struct proc *timeout[NPROCS];
static int n_timeouts = 0;

static int ticks = 0;           /* Accumulated ticks since last update (ms) */
static int next_time = NO_TIME; /* Earliest timeout due (could be -ve) */

/* set_timeout -- schedule a timeout */
static void set_timeout(int ms) {
    int due = ticks + ms;
    assert(n_timeouts < NPROCS);
    assert(os_current->p_timeout == NO_TIME);
    os_current->p_timeout = due;
    timeout[n_timeouts++] = os_current;
    if (next_time == NO_TIME || due < next_time)
        next_time = due;
}

/* cancel_timeout -- cancel a timeout when before it is due */
static void cancel_timeout(struct proc *p) {
    assert(p->p_timeout != NO_TIME);
    p->p_timeout = NO_TIME;

    // Delete p from the timeout array
    for (int i = 0; i < n_timeouts; i++) {
        if (timeout[i] == p) {
            timeout[i] = timeout[--n_timeouts];
            return;
        }
    }

    panic("Cancelling an unset timeout");
}

/* mini-tick -- register a clock tick and fire any timeouts due */
void mini_tick(int ms) {
    if (next_time == NO_TIME)
        // No timers active
        return;
    else if (ticks <= next_time) {
        // No timer yet expired
        ticks += ms;
        return;
    }

    // Search for expired timers
    int n = 0;                  /* Number of timers remaining */
    next_time = NO_TIME;
    for (int j = 0; j < n_timeouts; j++) {
        struct proc *pdst = timeout[j];
        assert(pdst->p_timeout != NO_TIME);
        if (pdst->p_timeout >= ticks) {
            // The timer is not yet expired, so update its expiry time
            pdst->p_timeout -= ticks + ms;
            timeout[n++] = pdst;
            if (next_time == NO_TIME || pdst->p_timeout < next_time)
                next_time = pdst->p_timeout;
        } else {
            // Send the TIMEOUT message
            pdst->p_timeout = NO_TIME;
            deliver(pdst->p_message, HARDWARE, TIMEOUT, NULL);
            make_ready(pdst);
        }
    }

    ticks = 0;
    n_timeouts = n;
}

#endif


/* SEND AND RECEIVE */

/* These versions of send and receive are invoked indirectly from user
processes via the system calls send() and receive(). */

/* accept -- test if a process is waiting for a message of given type */
static inline int accept(struct proc *pdest, int type) {
    return (pdest->p_state == RECEIVING
            && (pdest->p_msgtype == ANY || pdest->p_msgtype == type));
}

/* set_state -- set process state for send or receive */
static inline void set_state(struct proc *p, int state,
                             int type, message *msg) {
    p->p_state = state;
    p->p_msgtype = type;
    p->p_message = msg;
}

/* enqueue -- add current process to a receiver's queue */
static inline void enqueue(struct proc *pdest) {
    os_current->p_next = NULL;
    if (pdest->p_waiting == NULL)
        pdest->p_waiting = os_current;
    else {
        struct proc *r = pdest->p_waiting;
        while (r->p_next != NULL)
            r = r->p_next;
        r->p_next = os_current;
    }
}

/* find_sender -- search process queue for acceptable sender */
static struct proc *find_sender(struct proc *pdst, int type) {
    struct proc *psrc, *prev = NULL;
        
    for (psrc = pdst->p_waiting; psrc != NULL; psrc = psrc->p_next) {
        if (type == ANY || psrc->p_msgtype == type) {
            if (prev == NULL)
                pdst->p_waiting = psrc->p_next;
            else
                prev->p_next = psrc->p_next;

            return psrc;
        }

        prev = psrc;
    }

    return NULL;
}

/* await_reply -- wait for reply after sendrec */
static void await_reply(struct proc *pdst, message *msg) {
    struct proc *psrc = find_sender(pdst, REPLY);
    if (psrc != NULL) {
        /* Unlikely but not impossible: a REPLY message is already waiting.
           It can't come from the process pdst. */
        deliver(pdst->p_message, psrc->p_pid, REPLY, msg);
        make_ready(pdst);
        make_ready(psrc);
    } else {
        set_state(pdst, RECEIVING, REPLY, msg);
    }
}

/* mini_send -- send a message */
static void mini_send(int dest, int type, message *msg) {
    int src = os_current->p_pid;
    struct proc *pdest = os_ptable[dest];

    if (dest < 0 || dest >= os_nprocs || pdest->p_state == DEAD)
        panic("Sending to a non-existent process %d", dest);

    if (accept(pdest, type)) {
        // Receiver is waiting: deliver the message and run receiver
        deliver(pdest->p_message, src, type, msg);
#ifdef TIMEOUT
        if (pdest->p_timeout != NO_TIME)
            cancel_timeout(pdest);
#endif
        make_ready(pdest);
        make_ready(os_current);
    } else {
        // Sender must wait by joining the receiver's queue
        set_state(os_current, SENDING, type, msg);
        enqueue(pdest);
    }

    choose_proc();
}

/* mini_receive -- receive a message */
static void mini_receive(int type, message *msg
#ifdef TIMEOUT
                         , int timeout
#endif
    ) {
    // First see if an interrupt is pending
    if (os_current->p_pending && (type == ANY || type == INTERRUPT)) {
        os_current->p_pending = 0;
        deliver(msg, HARDWARE, INTERRUPT, NULL);
        return;
    }

    // Now see if a sender is waiting
    if (type != INTERRUPT) {
        struct proc *psrc = find_sender(os_current, type);

        if (psrc != NULL) {
            deliver(msg, psrc->p_pid, psrc->p_msgtype, psrc->p_message);
            make_ready(os_current);

            switch (psrc->p_state) {
            case SENDING:
                make_ready(psrc);
                break;

            case SENDREC:
                await_reply(psrc, psrc->p_message);
                break;

            default:
                panic("Bad state in receive()");
            }

            choose_proc();
            return;
        }
    }

#ifdef TIMEOUT
    if (timeout == 0) {
        // No message, so time out immediately
        deliver(msg, HARDWARE, TIMEOUT, NULL);
        return;
    }
#endif

    // No luck: we must wait.
    set_state(os_current, RECEIVING, type, msg);
#ifdef TIMEOUT
    if (timeout > 0) set_timeout(timeout);
#endif
    choose_proc();
}    

/* mini_sendrec -- send a message and wait for reply */
static void mini_sendrec(int dest, int type, message *msg) {
    int src = os_current->p_pid;
    struct proc *pdest = os_ptable[dest];

    if (type == REPLY)
        panic("sendrec may not be used to send REPLY message");

    if (dest < 0 || dest >= os_nprocs || pdest->p_state == DEAD)
        panic("Sending to a non-existent process %d", dest);

    if (accept(pdest, type)) {
        // Send the message and wait for a reply
        deliver(pdest->p_message, src, type, msg);
        make_ready(pdest);
        await_reply(os_current, msg);
    } else {
        // Join receiver's queue
        set_state(os_current, SENDREC, type, msg);
        enqueue(pdest);
    }

    choose_proc();
}

/* INTERRUPT HANDLING */

/* Interrupts send an INTERRUPT message (from HARDWARE) to a
registered handler process.  The default beheviour is to disable the
relevant IRQ in the interrupt handler, so that it can be re-enabled in
the handler once it has reacted to the interrupt.  We only deal with
the genuine interrupts >= 0, not the 16 exceptions that are < 0 this way. */

/* os_handler -- pid of handler process for each interrupt */
static int os_handler[N_INTERRUPTS];

/* connect -- connect the current process to an IRQ */
void connect(int irq) {
    if (irq < 0) panic("Can't connect to CPU exceptions");
    os_current->p_priority = P_HANDLER;
    os_handler[irq] = os_current->p_pid;
}

/* priority -- set process priority */
void priority(int p) {
    if (p < 0 || p > P_LOW) panic("Bad priority %d\n", p);
    os_current->p_priority = p;
}

/* interrupt -- send interrupt message */
void interrupt(int dest) {
    struct proc *pdest = os_ptable[dest];

    if (accept(pdest, INTERRUPT)) {
        // Receiver is waiting for an interrupt
        deliver(pdest->p_message, HARDWARE, INTERRUPT, NULL);

        make_ready(pdest);
        if (os_current->p_priority > P_HANDLER) {
            // Preempt lower-priority process
            reschedule();
        }
    } else {
        // Let's hope it's not urgent!
        pdest->p_pending = 1;
    }
}

/* All interrupts are handled by this common handler, which disables
the interrupt temporarily, then sends or queues a message to the
registered handler task.  Normally the handler task will deal with the
cause of the interrupt, then re-enable it. */

/* default_handler -- handler for most interrupts */
void default_handler(void) {
    int irq = active_irq(), task;
    if (irq < 0 || (task = os_handler[irq]) == 0)
        panic("Unexpected interrupt %d", irq);
    disable_irq(irq);
    interrupt(task);
}

/* hardfault_handler -- substitutes for the definition in startup.c */
void hardfault_handler(void) {
    // On nRF52, other exceptions come here too
    panic("HardFault");
}


/* INITIALISATION */

/* create_proc -- allocate and initialise process descriptor */
static struct proc *create_proc(char *name, unsigned stksize) {
    int pid;
    struct proc *p;
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
    p->p_pid = pid;
    strncpy(p->p_name, name, 15);
    p->p_name[15] = '\0';
    p->p_sp = sp;
    p->p_stack = stack;
    p->p_stksize = stksize;
    p->p_state = ACTIVE;
    p->p_priority = P_LOW;
    p->p_waiting = 0;
    p->p_pending = 0;
    p->p_msgtype = ANY;
#ifdef TIMEOUT
    p->p_timeout = NO_TIME;
#endif
    p->p_message = NULL;
    p->p_next = NULL;

    return p;
}

#define MAGIC 0xfffffffd        /* Magic value for exception return */
#define INIT_PSR 0x01000000     /* Thumb bit is set */

// These match the frame layout in mpx.s, and the hardware
#define ERV_SAVE 0 // Offset for magic return value
#define R0_SAVE 9
#define R1_SAVE 10
#define R2_SAVE 11
#define LR_SAVE 14
#define PC_SAVE 15
#define PSR_SAVE 16
#define FRAME_WORDS 17

#define roundup(x, n) (((x) + ((n)-1)) & ~((n)-1))

/* start -- initialise process to run later */
int start(char *name, void (*body)(int), int arg, int stksize) {
    struct proc *p = create_proc(name, roundup(stksize, 8));

    if (os_current != NULL)
        panic("start() called after scheduler startup");

    /* Fake an exception frame */
    unsigned *sp = p->p_sp - FRAME_WORDS;
    memset(sp, 0, 4*FRAME_WORDS);
    sp[PSR_SAVE] = INIT_PSR;
    sp[PC_SAVE] = (unsigned) body & ~0x1; // Activate the process body
    sp[LR_SAVE] = (unsigned) exit; // Make it return to exit()
    sp[R0_SAVE] = (unsigned) arg;  // Pass the supplied argument in R0
    sp[ERV_SAVE] = MAGIC;
    p->p_sp = sp;

    make_ready(p);
    return p->p_pid;
}

/* set_stack -- enter thread mode with specified stack (see mpx.s) */
void set_stack(unsigned *sp);

/* init -- main program, creates application processes */
void init(void);

#define IDLE_STACK 128

/* __start -- start the operating system */
void __start(void) {
    // Create idle task as process 0
    idle_proc = create_proc("IDLE", IDLE_STACK);
    idle_proc->p_state = IDLING;
    idle_proc->p_priority = P_IDLE;

    // Call the application's setup function
    init();

    // The main program morphs into the idle process.
    os_current = idle_proc;
    set_stack(os_current->p_sp);
    yield();                    // Pick a genuine process to run

    // Idle only runs again when there's nothing to do.
    while (1) pause();
}


/* SYSTEM CALL INTERFACE */

// System call numbers
#define SYS_YIELD 0
#define SYS_SEND 1
#define SYS_RECEIVE 2
#define SYS_SENDREC 3
#define SYS_EXIT 4
#define SYS_DUMP 5
#define SYS_TICK 6

/* System calls retrieve their arguments from the exception frame that
was saved by the SVC instruction on entry to the operating system.  We
can't rely on the arguments stiull being in r0, r1, etc., because an
interrupt may have intervened and trashed these registers. */

#define arg(i, t) ((t) psp[R0_SAVE+(i)])

/* system_call -- entry from system call traps */
unsigned *system_call(unsigned *psp) {
    short *pc = (short *) psp[PC_SAVE]; // Program counter
    int op = pc[-1] & 0xff;      // Syscall number from svc instruction

    // Save sp of the current process
    os_current->p_sp = psp;

    // Check for stack overflow
    if (* (unsigned *) os_current->p_stack != BLANK)
        panic("Stack overflow");

    switch (op) {
    case SYS_YIELD:
        make_ready(os_current);
        choose_proc();
        break;

    case SYS_SEND:
        mini_send(arg(0, int), arg(1, int), arg(2, message *));
        break;

    case SYS_RECEIVE:
        mini_receive(arg(0, int), arg(1, message *)
#ifdef TIMEOUT
                     , arg(2, int)
#endif
            );
        break;

    case SYS_SENDREC:
        mini_sendrec(arg(0, int), arg(1, int), arg(2, message *));
        break;

    case SYS_EXIT:
        os_current->p_state = DEAD;
        choose_proc();
        break;

    case SYS_DUMP:
        /* Invoking microbian_dump as a system call means that its own
           stack space is taken from the system stack rather than the
           stack of the current process. */
        microbian_dump();
        break;

    case SYS_TICK:
        mini_tick(arg(0, int));
        break;

    default:
        panic("Unknown syscall %d", op);
    }

    // Return sp for next process to run
    return os_current->p_sp;
}

/* cxt_switch -- context switch following interrupt */
unsigned *cxt_switch(unsigned *psp) {
    os_current->p_sp = psp;
    make_ready(os_current);
    choose_proc();
    return os_current->p_sp;
}


/* SYSTEM CALL STUBS */

/* Each function defined here leaves its arguments in r0, r1, etc.,
and executes an svc instruction with operand equal to the system call
number.  After state has been saved, system_call() is invoked and
retrieves the call number and arguments from the exception frame.
Calls to these functions must not be inlined, or the arguments will
not be found in the right places. */

#define NOINLINE __attribute((noinline))

void NOINLINE yield(void) {
    syscall(SYS_YIELD);
}

void NOINLINE send(int dest, int type, message *msg) {
    syscall(SYS_SEND);
}

#ifdef TIMEOUT

void NOINLINE receive_t(int type, message *msg, int timeout) {
    syscall(SYS_RECEIVE);
}

void receive(int type, message *msg) {
    receive_t(type, msg, -1);
}

#else

void NOINLINE receive(int type, message *msg) {
    syscall(SYS_RECEIVE);
}

#endif

void NOINLINE sendrec(int dest, int type, message *msg) {
    syscall(SYS_SENDREC);
}

void NOINLINE exit(void) {
    syscall(SYS_EXIT);
}

void NOINLINE dump(void) {
    syscall(SYS_DUMP);
}

void NOINLINE tick(int ms) {
    syscall(SYS_TICK);
}


/* DEBUG PRINTING */

/* The routines here work by reconfiguring the UART, disabling
interrupts and polling: they should be used only for debugging. */

/* delay_usec -- delay loop */
static void delay_usec(int usec) {
    int t = usec<<1;
    while (t > 0) {
        // 500nsec per iteration
        nop(); nop(); nop();
        t--;
    }
}
        
/* kprintf_setup -- set up UART connection to host */
static void kprintf_setup(void) {
    // Delay so any UART activity can cease
    delay_usec(2000);

    // Set up pins to maintain signal levels while UART disabled
    gpio_dir(USB_TX, 1); gpio_dir(USB_RX, 0); gpio_out(USB_TX, 1);

    // Reconfigure the UART just to be sure
    UART_ENABLE = UART_ENABLE_Disabled;
    UART_BAUDRATE = UART_BAUDRATE_9600; // 9600 baud
    UART_CONFIG = FIELD(UART_CONFIG_PARITY, UART_PARITY_None);
                                        // format 8N1
    UART_PSELTXD = USB_TX;              // choose pins
    UART_PSELRXD = USB_RX;
    UART_ENABLE = UART_ENABLE_Enabled;
    UART_STARTTX = 1;
    UART_STARTRX = 1;
    UART_RXDRDY = 0;
}

/* kputc -- send output character */
static void kputc(char ch) {
    UART_TXD = ch;
    while (! UART_TXDRDY) { }
    UART_TXDRDY = 0;
}


/* kprintf_internal -- internal version of kprintf */
static void kprintf_internal(char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    do_print(kputc, fmt, va);
    va_end(va);
}

/* kprintf -- printf variant for debugging (disables interrupts) */
void kprintf(char *fmt, ...) {
    va_list va;
    unsigned prev = get_primask();
    intr_disable();
    kprintf_setup();

    va_start(va, fmt);
    do_print(kputc, fmt, va);
    va_end(va);

    set_primask(prev);
    // Caller gets a UART interrupt if enabled.
}

/* panic -- the unusual has happened.  Did you think it impossible? */
void panic(char *fmt, ...) {
    va_list va;
    intr_disable();
    kprintf_setup();     

    kprintf_internal("\r\nPanic: ");
    va_start(va, fmt);
    do_print(kputc, fmt, va);
    va_end(va);
    if (os_current != NULL)
         kprintf_internal(" in process %s", os_current->p_name);
    kprintf_internal("\r\n");

    spin();
}

/* badmesg -- default case for switches on message type */
void badmesg(int type) {
    panic("Bad message type %d", type);
}
