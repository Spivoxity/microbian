/* microbian.c */
/* Copyright (c) 2018 J. M. Spivey */

#include "microbian.h"
#include "lib.h"
#include "hardware.h"
#include <string.h>
#include "process.h"


#ifdef MICROBIAN_DEBUG
void debug_sched(int pid);
#define DEBUG_SCHED(pid) debug_sched(pid)
#else
#define DEBUG_SCHED(pid)
#endif




/* Possible state values */
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
static proc new_proc(void) {
    if (htop - hbot < sizeof(struct _proc))
        panic("No space for process");
    htop -= sizeof(struct _proc);
    return (proc) htop;
}


/* PROCESS TABLE */

#define NPROCS 32

static proc os_ptable[NPROCS];
static unsigned os_nprocs = 0;

static proc os_current;
static proc idle_proc;

#define BLANK 0xdeadbeef        /* Filler for initial stack */

static void kprintf_setup(void);

static void kprintf_internal(char *fmt, ...);

/* pad -- pad string with spaces to a specified width */
static void pad(char *buf, int width) {
    int w = strlen(buf);
    if (w < width) {
        memset(buf + w, ' ', width - w);
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
        proc p = os_ptable[pid];

        /* Measure free space on the process stack */
        unsigned *z = (unsigned *) p->stack;
        while (*z == BLANK) z++;
        unsigned free = (char *) z - (char *) p->stack;

        sprintf(buf, "%u/%u", p->stksize - free, p->stksize);
        pad(buf, 9);
        kprintf_internal("%s%d: %s %x stk=%s %s\r\n",
                         (pid < 10 ? " " : ""), pid,
                         status[p->state], (unsigned) p->stack,
                         buf, p->name);
    }
}


/* PROCESS QUEUES */

/* os_readyq -- one queue for each priority */
typedef struct _queue *queue;

static struct _queue {
    proc head, tail;
} os_readyq[NPRIO];

/* make_ready -- add process to the ready queue based on priority and average running time ifdef _SCHEDULING_OPT */
static inline void make_ready(proc p) {
    int prio = p->priority;
    if (prio == P_IDLE)
        return;

    p->state = ACTIVE;
    p->next = NULL;

    queue q = &os_readyq[prio];
    if (q->head == NULL) {
        // If the queue is empty, insert the process at the front
        q->head = p;
        q->tail = p;
    } else {


#ifdef _SCHEDULING_OPT // then the queue is by increasing score
        if (score(p) >= score(q->tail)) {
            q->tail->next = p;
            q->tail = p;
        } else {
            // find first element in queue that has score bigger or equal to p,
            // then insert **behind** that element.
            proc prev = NULL;
            proc curr = q->head;

            while (curr != NULL && score(curr) <
                                   score(p)) {//Normally, because the queue is ordered, the first condition will always be false.
                prev = curr;
                curr = curr->next;
            }

            if (prev == NULL) {// iff score(p) < score(q->head)
                q->head = p;
                q->head->next = curr;
            } else {
                prev->next = p;
                p->next = curr;
            }
        }
#else
        // Insert the process at the end of the queue
        q->tail->next = p;
        q->tail = p;
#endif
    }
}

#ifdef _SCHEDULING_OPT
// Calculates the amount of time between two unsigned integers, handling overflow gracefully.
unsigned delta(unsigned t1, unsigned t2) {
    if (t1 > t2) {
        // Assume overflow and treat t2 as such.
        t2 += (unsigned)sizeof(unsigned);
    }
    return t2 - t1;
}
#endif

/* choose_proc -- the current process is blocked: pick a new one */
static inline void choose_proc(void) {
#ifdef _SCHEDULING_OPT
    TIMER0_CAPTURE[1];
    os_current->age += delta(TIMER0_CC[0], TIMER0_CC[1]);
#endif
    // Original sequential iteration through the queues
    for (int p = 0; p < NPRIO; p++) {
        queue q = &os_readyq[p];
        if (q->head != NULL) {
            os_current = q->head;
            q->head = os_current->next;
            DEBUG_SCHED(os_current->pid);
#ifdef _SCHEDULING_OPT
            TIMER0_CAPTURE[0] = 1;
#endif
            return;
        }
    }

    // If no process is found in the queues, assign the idle process
    os_current = idle_proc;
    DEBUG_SCHED(0);

}


/* deliver_special -- deliver a special message and mark the recipient ready */
static inline void deliver_special(proc pdst, int src, int type) {
    message *buf = pdst->msgbuf;
    if (buf) {
        buf->sender = src;
        buf->type = type;
    }
}


/* TIMEOUTS */

/* Programs that include a timer may also use a form of receive() with
a timeout, so that a message of type TIMEOUT from hardware is
delivered after an interval if no genuine message has arrived.

A process p has a timeout set if p->timeout != NO_TIME.  Such
processes are also listed in timeout[0..n_timeouts), so we can find
them quickly on each tick. The global ticks variable and the
timeout field count in ticks from the last timer update, and the
next update is due when ticks exceeds next_time.  This value can be
negative, because we delay firing timeouts until the tick after they
are due, to avoid firing them early.  If calls the tick() come
at regular intervals (whatever they are), then this scheme ensures
that no timer fires earlier than it should, even if the timer is set
just before a tick. */

#ifdef _TIMEOUT

static proc timeout[NPROCS];
static int n_timeouts = 0;

static int ticks = 0;           /* Accumulated ticks since last update (ms) */
static int next_time = NO_TIME; /* Earliest timeout due (could be -ve) */




/* set_timeout -- schedule a timeout */
static void set_timeout(int ms) {
    int due = ticks + ms;
    assert(n_timeouts < NPROCS);
    assert(os_current->timeout == NO_TIME);
    os_current->timeout = due;
    timeout[n_timeouts++] = os_current;
    if (next_time == NO_TIME || due < next_time)
        next_time = due;
}

/* cancel_timeout -- cancel a timeout before it is due */
static void cancel_timeout(proc p) {
    assert(p->timeout != NO_TIME);
    p->timeout = NO_TIME;

    /* Delete p from the timeout array */
    for (int i = 0; i < n_timeouts; i++) {
        if (timeout[i] == p) {
            timeout[i] = timeout[--n_timeouts];
            return;
        }
    }

    panic("Cancelling an unset timeout");
}

/* mini-tick -- register a clock tick and fire any timeouts due */
static void mini_tick(int ms) {
    if (next_time == NO_TIME)
        /* No timers active */
        return;
    else if (ticks <= next_time) {
        /* No timer yet expired */
        ticks += ms;
        return;
    }

    /* Search for expired timers */
    int n = 0;                  /* Number of timers remaining */
    next_time = NO_TIME;
    for (int j = 0; j < n_timeouts; j++) {
        proc pdst = timeout[j];
        assert(pdst->timeout != NO_TIME);
        if (pdst->timeout >= ticks) {
            /* The timer is not yet expired, so update its expiry time */
            pdst->timeout -= ticks + ms;
            timeout[n++] = pdst;
            if (next_time == NO_TIME || pdst->timeout < next_time)
                next_time = pdst->timeout;
        } else {
            /* Send the TIMEOUT message */
            pdst->timeout = NO_TIME;
            deliver_special(pdst, HARDWARE, TIMEOUT);
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
static inline int accept(proc pdest, int type) {
    return (pdest->state == RECEIVING
            && (pdest->filter == ANY || pdest->filter == type));
}

/* deliver -- copy a message and make the destination ready */
static inline void deliver(proc pdest, proc psrc) {
    if (pdest->msgbuf) {
        *(pdest->msgbuf) = *(psrc->msgbuf);
        pdest->msgbuf->sender = psrc->pid;
    }
    make_ready(pdest);
}

/* queue_sender -- add current process to a receiver's queue */
static inline void queue_sender(proc pdest) {
    os_current->next = NULL;
    if (pdest->waiting == NULL)
        pdest->waiting = os_current;
    else {
        proc r = pdest->waiting;
        while (r->next != NULL)
            r = r->next;
        r->next = os_current;
    }
}

/* find_sender -- search process queue for acceptable sender */
static proc find_sender(proc pdst, int type) {
    proc psrc, prev = NULL;

    for (psrc = pdst->waiting; psrc != NULL; psrc = psrc->next) {
        if (type == ANY || psrc->msgbuf->type == type) {
            if (prev == NULL)
                pdst->waiting = psrc->next;
            else
                prev->next = psrc->next;

            return psrc;
        }

        prev = psrc;
    }

    return NULL;
}

/* await_reply -- wait for reply after sendrec */
static void await_reply(proc pdst) {
    proc psrc = find_sender(pdst, REPLY);
    if (psrc != NULL) {
        /* Unlikely but not impossible: a REPLY message is already waiting.
           It can't come from the process pdst. */
        deliver(pdst, psrc);
        make_ready(psrc);
    } else {
        pdst->state = RECEIVING;
        pdst->filter = REPLY;
    }
}

static inline proc find_dest(int dest) {
    proc pdest;

    if (dest < 0 || dest >= os_nprocs)
        panic("Sending to a non-existent process %d", dest);

    pdest = os_ptable[dest];

    if (pdest->state == DEAD)
        panic("Sending to a dead process %s", pdest->name);

    return pdest;
}

/* mini_send -- send a message */
static void mini_send(int dest, message *msg) {
    proc pdest = find_dest(dest);

    os_current->msgbuf = msg;

    if (accept(pdest, msg->type)) {
        /* Receiver is waiting: deliver the message and run receiver */
#ifdef _TIMEOUT
        if (pdest->timeout != NO_TIME)
            cancel_timeout(pdest);
#endif
        deliver(pdest, os_current);
        make_ready(os_current);
    } else {
        /* Sender must wait by joining the receiver's queue */
        os_current->state = SENDING;
        queue_sender(pdest);
    }

    choose_proc();
}

/* mini_receive -- receive a message */
static void mini_receive(int type, message *msg
#ifdef _TIMEOUT
        , int timeout
#endif
) {
    os_current->msgbuf = msg;

    /* First see if an interrupt is pending */
    if (os_current->pending && (type == ANY || type == INTERRUPT)) {
        os_current->pending = 0;
        deliver_special(os_current, HARDWARE, INTERRUPT);
        return;
    }

    /* Now see if a sender is waiting */
    if (type != INTERRUPT) {
        proc psrc = find_sender(os_current, type);

        if (psrc != NULL) {
            deliver(os_current, psrc);

            switch (psrc->state) {
                case SENDING:
                    make_ready(psrc);
                    break;

                case SENDREC:
                    await_reply(psrc);
                    break;

                default:
                    panic("Bad state in receive()");
            }

            choose_proc();
            return;
        }
    }

#ifdef _TIMEOUT
    if (timeout == 0) {
        /* No message, so time out immediately */
        deliver_special(os_current, HARDWARE, TIMEOUT);
        return;
    }
#endif

    /* No luck: we must wait. */
    os_current->state = RECEIVING;
    os_current->filter = type;
#ifdef _TIMEOUT
    if (timeout > 0) set_timeout(timeout);
#endif
    choose_proc();
}

/* mini_sendrec -- send a message and wait for reply */
static void mini_sendrec(int dest, message *msg) {
    proc pdest = find_dest(dest);

    if (msg->type == REPLY)
        panic("sendrec may not be used to send REPLY message");

    os_current->msgbuf = msg;

    if (accept(pdest, msg->type)) {
        /* Send the message and wait for a reply */
        deliver(pdest, os_current);
        await_reply(os_current);
    } else {
        /* Join receiver's queue */
        os_current->state = SENDREC;
        queue_sender(pdest);
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
    os_current->priority = P_HANDLER;
    os_handler[irq] = os_current->pid;
}

/* priority -- set process priority */
void priority(int p) {
    if (p < 0 || p > P_LOW) panic("Bad priority %d\n", p);
    os_current->priority = p;
}

/* interrupt -- send interrupt message */
void interrupt(int dest) {
    proc pdest = find_dest(dest);

    if (accept(pdest, INTERRUPT)) {
        /* Receiver is waiting for an interrupt */
        deliver_special(pdest, HARDWARE, INTERRUPT);
        make_ready(pdest);
        if (os_current->priority > P_HANDLER) {
            /* Preempt lower-priority process */
            reschedule();
        }
    } else {
        /* Let's hope it's not urgent! */
        pdest->pending = 1;
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
    int fault = active_irq() + 16;

    static const char *exc_name[] = {
            "*zero*", "Reset", "NMI", "HardFault",
            "MemManage", "BusFault", "UsageFault"
    };

    panic("Unexpected fault %s", exc_name[fault]);
}


/* INITIALISATION */

/* create_proc -- allocate and initialise process descriptor */
static proc create_proc(char *name, unsigned stksize) {
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

#define MAGIC 0xfffffffd        /* Magic value for exception return */
#define INIT_PSR 0x01000000     /* Thumb bit is set */

/* These match the frame layout in mpx.s, and the hardware */
#define ERV_SAVE 0 /* Offset for magic return value */
#define R0_SAVE 9
#define R1_SAVE 10
#define R2_SAVE 11
#define LR_SAVE 14
#define PC_SAVE 15
#define PSR_SAVE 16
#define FRAME_WORDS 17

#define roundup(x, n) (((x) + ((n)-1)) & ~((n)-1))

/* start -- initialise a process to run later */
int start(char *name, void (*body)(int), int arg, int stksize) {
    proc p = create_proc(name, roundup(stksize, 8));

    if (os_current != NULL)
        panic("start() called after scheduler startup");

    /* Fake an exception frame */
    unsigned *sp = p->sp - FRAME_WORDS;
    memset(sp, 0, 4 * FRAME_WORDS);
    sp[PSR_SAVE] = INIT_PSR;
    sp[PC_SAVE] = (unsigned) body & ~0x1; /* Activate the process body */
    sp[LR_SAVE] = (unsigned) exit; /* Make it return to exit() */
    sp[R0_SAVE] = (unsigned) arg;  /* Pass the supplied argument in R0 */
    sp[ERV_SAVE] = MAGIC;
    p->sp = sp;

    make_ready(p);
    return p->pid;
}

/* set_stack -- enter thread mode with specified stack (see mpx.s) */
void set_stack(unsigned *sp);

/* init -- main program, creates application processes */
void init(void);

#define IDLE_STACK 128

/* idle_task -- body of idle process */
static void idle_task(void) {
    /* Pick a genuine process to run */
    yield();

    /* Idle only runs again when there's nothing to do. */
    while (1) pause();
}


/* __start -- start the operating system */
void __start(void) {

    /* Create idle task as process 0 */
    idle_proc = create_proc("IDLE", IDLE_STACK);
    idle_proc->state = IDLING;
    idle_proc->priority = P_IDLE;

#ifdef _SCHEDULING_OPT
    TIMER0_STOP = 1;
    TIMER0_MODE = TIMER_MODE_Timer;
    TIMER0_BITMODE = TIMER_BITMODE_32Bit;
    TIMER0_PRESCALER = 4;      // 1MHz = 16MHz / 2^4
    TIMER0_CLEAR = 1;
    TIMER0_CC[0] = 1000 * TICK;
    TIMER0_START = 1;
#endif
    /* Call the application's setup function */
    init();

    /* The main program morphs into the idle process. */
    os_current = idle_proc;
    DEBUG_SCHED(0);
    set_stack(os_current->sp);
    idle_task();
}


/* SYSTEM CALL INTERFACE */

/* System call numbers */
#define SYS_YIELD 0
#define SYS_SEND 1
#define SYS_RECEIVE 2
#define SYS_SENDREC 3
#define SYS_EXIT 4
#define SYS_DUMP 5
#define SYS_TICK 6

/* System calls retrieve their arguments from the exception frame that
was saved by the SVC instruction on entry to the operating system.  We
can't rely on the arguments still being in r0, r1, etc., because an
interrupt may have intervened and trashed these registers. */

#define sysarg(i, t) ((t) psp[R0_SAVE+(i)])


/* system_call -- entry from system call traps */
unsigned *system_call(unsigned *psp) {

    short *pc = (short *) psp[PC_SAVE]; /* Program counter */
    int op = pc[-1] & 0xff;      /* Syscall number from svc instruction */

    /* Save sp of the current process */
    os_current->sp = psp;

    /* Check for stack overflow */
    if (*(unsigned *) os_current->stack != BLANK)
        panic("Stack overflow");

    switch (op) {
        case SYS_YIELD:
            make_ready(os_current);
            choose_proc();// os_current will have changed after this.
            break;

        case SYS_SEND:
            mini_send(sysarg(0, int), sysarg(1, message *));
            break;

        case SYS_RECEIVE:
            mini_receive(sysarg(0, int), sysarg(1, message *)
#ifdef _TIMEOUT
                    , sysarg(2, int)
#endif
            );
            break;

        case SYS_SENDREC:

            mini_sendrec(sysarg(0, int), sysarg(1, message *));
            break;

        case SYS_EXIT:
            os_current->state = DEAD;
            choose_proc();// os_current will have changed after this.
            break;

        case SYS_DUMP:
            /* Invoking microbian_dump as a system call means that its own
               stack space is taken from the system stack rather than the
               stack of the current process. */
            microbian_dump();
            break;

        case SYS_TICK:
#ifdef _SCHEDULING_OPT
            os_current->n_ticks += 1;
#endif

#ifdef _TIMEOUT

            mini_tick(sysarg(0, int));
#endif
            break;

        default:
            panic("Unknown syscall %d", op);
    }


#ifdef _SCHEDULING_OPT
    TIMER0_CAPTURE[1] = 1;
    os_current->age += delta(TIMER0_CC[0], TIMER0_CC[1]); //TODO: is good?
#endif
    /* Return sp for next process to run */
    return os_current->sp;
}

/* cxt_switch -- context switch following interrupt */
unsigned *cxt_switch(unsigned *psp) {
    os_current->sp = psp;
    make_ready(os_current);
    choose_proc();
    return os_current->sp;
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

void NOINLINE send(int dest, message *msg) {
    syscall(SYS_SEND);
}

void send_msg(int dest, int type) {
    message m;
    m.type = type;
    send(dest, &m);
}

void send_int(int dest, int type, int val) {
    message m;
    m.type = type;
    m.int1 = val;
    send(dest, &m);
}

#ifdef _TIMEOUT

void NOINLINE receive_t(int type, message *msg, int timeout) {
    syscall(SYS_RECEIVE);
}

void receive(int type, message *msg) {
    receive_t(type, msg, -1);
}

#else

void NOINLINE receive(int type, message *msg)
{
    syscall(SYS_RECEIVE);
}

#endif

void NOINLINE sendrec(int dest, message *msg) {
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
    int t = usec << 1;
    while (t > 0) {
        /* 500nsec per iteration */
        nop();
        nop();
        nop();
        t--;
    }
}

/* kprintf_setup -- set up UART connection to host */
static void kprintf_setup(void) {
    /* Delay so any UART activity can cease */
    delay_usec(2000);

    /* Set up pins to maintain signal levels while UART disabled */
    gpio_dir(USB_TX, 1);
    gpio_dir(USB_RX, 0);
    gpio_out(USB_TX, 1);

    /* Reconfigure the UART just to be sure */
    UART_ENABLE = UART_ENABLE_Disabled;
    UART_BAUDRATE = UART_BAUDRATE_9600; /* 9600 baud */
    UART_CONFIG = FIELD(UART_CONFIG_PARITY, UART_PARITY_None);
    /* format 8N1 */
    UART_PSELTXD = USB_TX;              /* choose pins */
    UART_PSELRXD = USB_RX;
    UART_ENABLE = UART_ENABLE_Enabled;
    UART_STARTTX = 1;
    UART_STARTRX = 1;
    UART_RXDRDY = 0;
}

/* kputc -- send output character */
static void kputc(char ch) {
    UART_TXD = ch;
    while (!UART_TXDRDY) {}
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
    /* Caller gets a UART interrupt if enabled. */
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
        kprintf_internal(" in process %s", os_current->name);
    kprintf_internal("\r\n");

    spin();
}

/* badmesg -- default case for switches on message type */
void badmesg(int type) {
    panic("Bad message type %d", type);
}
