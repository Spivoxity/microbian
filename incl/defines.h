typedef unsigned char byte;
#define _TIMEOUT 1
#define _SCHEDULING_OPT 1

#ifdef UBIT_V1
#define TICK 5                  // Interval between updates (ms)
#endif
#ifdef UBIT_V2
#define TICK 1                  // helps with faster display update
#endif

/* Possible state values */
#define DEAD 0
#define ACTIVE 1
#define SENDING 2
#define RECEIVING 3
#define SENDREC 4
#define IDLING 5

#define NO_TIME 0x80000000

/* corresponding enum */
const char *status[] = {
        "[DEAD]   ",
        "[ACTIVE] ",
        "[SEND]   ",
        "[RECEIVE]",
        "[SENDREC]",
        "[IDLE]   "
};