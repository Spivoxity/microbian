typedef unsigned char byte;
#define _TIMEOUT 1
#define _SCHEDULING_OPT 1

#ifdef UBIT_V1
#define TICK 5                  // Interval between updates (ms)
#endif
#ifdef UBIT_V2
#define TICK 1                  // helps with faster display update
#endif
