/* common/hardware.h */
/* Copyright (c) 2018-20 J. M. Spivey */

#define UBIT 1
#define UBIT_V2 1

/* Hardware register definitions for nRF52833 */

#define BIT(i) (1 << (i))
#define SET_BIT(reg, n) reg |= BIT(n)
#define GET_BIT(reg, n) (((reg) >> (n)) & 0x1)
#define CLR_BIT(reg, n) reg &= ~BIT(n)
#define GET_BYTE(reg, n) (((reg) >> (8*(n))) & 0xff)
#define SET_BYTE(reg, n, v) \
    reg = (reg & ~(0xff << 8*n)) | ((v & 0xff) << 8*n)

/* The macros SET_FIELD, etc., are defined in an indirect way that
permits (because of the timing of CPP macro expansion) the 'field'
argument to be a macro that expands the a 'position, width' pair. */

#define SET_FIELD(reg, field, val) _SET_FIELD(reg, field, val)
#define _SET_FIELD(reg, pos, wd, val) \
    reg = (reg & ~_MASK(pos, wd)) | _FIELD(pos, wd, val)

#define GET_FIELD(reg, field) _GET_FIELD(reg, field)
#define _GET_FIELD(reg, pos, wd)  ((reg >> pos) & _MASK0(wd))

#define FIELD(field, val) _FIELD(field, val)
#define _FIELD(pos, wd, val)  (((val) & _MASK0(wd)) << pos)

#define MASK(field) _MASK(field)
#define _MASK(pos, wd)  (_MASK0(wd) << pos)

#define _MASK0(wd)  (~((-2) << (wd-1)))

#define __BIT(pos) pos
#define __FIELD(pos, wd) pos, wd


/* Device pins as encoded (port, pin) pairs */ 
#define DEVPIN(p, i) ((p<<5) + i)
#define PORT(x) ((x)>>5)
#define PIN(x) ((x)&0x1f)

#define PAD14 DEVPIN(0, 1) 
#define PAD0  DEVPIN(0, 2)
#define PAD1  DEVPIN(0, 3)
#define PAD2  DEVPIN(0, 4)
#define USB_TX DEVPIN(0, 6)
#define I2C0_SCL DEVPIN(0, 8)
#define PAD9  DEVPIN(0, 9)
#define PAD8  DEVPIN(0, 10)
#define PAD7  DEVPIN(0, 11)
#define   COL2 PAD7
#define PAD12 DEVPIN(0, 12)
#define PAD15 DEVPIN(0, 13)
#define PAD5  DEVPIN(0, 14)
#define   BUTTON_A PAD5
#define ROW3 DEVPIN(0, 15)
#define I2C0_SDA DEVPIN(0, 16)
#define PAD13 DEVPIN(0, 17)
#define ROW5 DEVPIN(0, 19)
#define ROW1 DEVPIN(0, 21)
#define ROW2 DEVPIN(0, 22)
#define PAD11 DEVPIN(0, 23)
#define   BUTTON_B PAD11
#define ROW4 DEVPIN(0, 24)
#define PAD19 DEVPIN(0, 26)
#define   I2C1_SCL PAD19
#define PAD4  DEVPIN(0, 28)
#define   COL1 PAD4
#define PAD10 DEVPIN(0, 30)
#define   COL5 PAD10
#define PAD3  DEVPIN(0, 31)
#define   COL3 PAD3

#define PAD20 DEVPIN(1, 0)
#define   I2C1_SDA PAD20
#define PAD16 DEVPIN(1, 2)
#define PAD6  DEVPIN(1, 5)
#define   COL4 PAD6
#define USB_RX DEVPIN(1, 8)


/* Interrupts */
#define SVC_IRQ    -5
#define PENDSV_IRQ -2
#define SYSTICK_IRQ -1
#define RADIO_IRQ   1
#define UART0_IRQ   2
#define I2C0_IRQ    3
#define I2C1_IRQ    4
#define GPIOTE_IRQ  6
#define ADC_IRQ     7
#define TIMER0_IRQ  8
#define TIMER1_IRQ  9
#define TIMER2_IRQ 10
#define RTC0_IRQ   11
#define TEMP_IRQ   12
#define RNG_IRQ    13
#define RTC1_IRQ   17
#define TIMER3_IRQ 26
#define TIMER4_IRQ 27
#define PWM0_IRQ   28
#define PWM1_IRQ   33
#define PWM2_IRQ   34
#define SPI0_IRQ   35
#define UART1_IRQ  40
#define PWM3_IRQ   45
#define SPI1_IRQ   47

#define N_INTERRUPTS 64

/* For compatibility, we allow UART as a synonym for UART0 */
#define UART_IRQ UART0_IRQ
#define uart_handler uart0_handler


/* Device registers */
#define _BASE(addr) ((unsigned volatile *) addr)
#define _REG(ty, addr) (* (ty volatile *) addr)
#define _ARR(ty, addr) ((ty volatile *) addr)

/* Common layout for DMA parameters */
typedef struct {
    void *PTR;
    unsigned MAXCNT;
    unsigned AMOUNT;
} dma_param;


/* System contol block */
DEVICE scb {
    REGISTER unsigned CPUID @ 0x00;
    REGISTER unsigned ICSR @ 0x04;
#define   SCB_ICSR_PENDSVSET __BIT(28)
#define   SCB_ICSR_VECTACTIVE __FIELD(0, 8)
    REGISTER unsigned SCR @ 0x10;
#define   SCB_SCR_SLEEPONEXIT __BIT(1)
#define   SCB_SCR_SLEEPDEEP __BIT(2)
#define   SCB_SCR_SEVONPEND __BIT(4)
    REGISTER unsigned SHPR[3] @ 0x18;
};

INSTANCE scb SCB @ 0xe000ed00;


/* Debug */
DEVICE debug {
    REGISTER unsigned DEMCR @ 0xfc;
#define   DEBUG_DEMCR_TRCENA __BIT(24)
};

INSTANCE debug DEBUG @ 0xe000ed00;


/* Nested vectored interupt controller */
DEVICE nvic {
    REGISTER unsigned ISER[8] @ 0x100;
    REGISTER unsigned ICER[8] @ 0x180;
    REGISTER unsigned ISPR[8] @ 0x200;
    REGISTER unsigned ICPR[8] @ 0x280;
    REGISTER unsigned IPR[60] @ 0x400;
};

INSTANCE nvic NVIC @ 0xe000e000;


/* Systick timer */
DEVICE systick {
    REGISTER unsigned CSR @ 0x010;
#define   SYSTICK_CSR_COUNTFLAG __BIT(16)
#define   SYSTICK_CSR_CLKSOURCE __FIELD(2, 1)
#define     SYSTICK_CLKSOURCE_External 0
#define     SYSTICK_CLKSOURCE_Internal 1
#define   SYSTICK_CSR_TICKINT __BIT(1)
#define   SYSTICK_CSR_ENABLE __BIT(0)
    REGISTER unsigned RVR @ 0x014;
    REGISTER unsigned CVR @ 0x018;
    REGISTER unsigned CALIB @ 0x1c;
#define   SYSTICK_CALIB_NOREF __BIT(31)
#define   SYSTICK_CALIB_SKEW __BIT(30)
#define   SYSTICK_CALIB_TENMS __FIELD(0, 24)
};

#define SYSTICK_CLOCK 64000000

INSTANCE systick SYSTICK @ 0xe000e000;


/* Data watchpoint and trace */
DEVICE dwt {
    REGISTER unsigned CTRL @ 0x000;
#define   DWT_CTRL_CYCCNTENA __BIT(0)
    REGISTER unsigned CYCCNT @ 0x004;
};

INSTANCE dwt DWT @ 0xe0001000;


/* Clock control */
DEVICE clock {
    REGISTER unsigned HFCLKSTART @ 0x000;
    REGISTER unsigned LFCLKSTART @ 0x008;
    REGISTER unsigned HFCLKSTARTED @ 0x100;
    REGISTER unsigned LFCLKSTARTED @ 0x104;
    REGISTER unsigned LFCLKSRC @ 0x518;
#define   CLOCK_LFCLKSRC_RC 0
    REGISTER unsigned XTALFREQ @ 0x550;
#define   CLOCK_XTALFREQ_16MHz 0xFF
};

INSTANCE clock CLOCK @ 0x40000000;


/* Memory protection unit */
DEVICE mpu {
    REGISTER unsigned DISABLEINDEBUG @ 0x608;
};

INSTANCE mpu MPU @ 0x40000000;


/* Factory information */
DEVICE ficr {
    REGISTER unsigned DEVICEID[2] @ 0x060;
    REGISTER unsigned DEVICEADDR[2] @ 0x0a4;
    REGISTER unsigned OVERRIDEEN @ 0x0a0;
#define   FICR_OVERRIDEEN_NRF __BIT(0)
    REGISTER unsigned NRF_1MBIT[5] @ 0x0b0;
};

INSTANCE ficr FICR @ 0x10000000;


/* Power management */
DEVICE power {
/* Tasks */
    REGISTER unsigned CONSTLAT @ 0x078;
    REGISTER unsigned LOWPWR @ 0x07c;
/* Events */
    REGISTER unsigned POFWARN @ 0x108;
/* Registers */
    REGISTER unsigned INTENSET @ 0x304;
    REGISTER unsigned INTENCLR @ 0x308;
    REGISTER unsigned RESETREAS @ 0x400;
#define   POWER_RESETREAS_RESETPIN __BIT(0)
#define   POWER_RESETREAS_DOG __BIT(1)
#define   POWER_RESETREAS_SREQ __BIT(2)
#define   POWER_RESETREAS_LOCKUP __BIT(3)
#define   POWER_RESETREAS_OFF __BIT(16)
#define   POWER_RESETREAS_LPCOMP __BIT(17)
#define   POWER_RESETREAS_DIF _BIT(18)
#define   POWER_RESETREAS_ALL 0x0007000f
    REGISTER unsigned RAMSTATUS @ 0x428;
    REGISTER unsigned SYSTEMOFF @ 0x500;
    REGISTER unsigned POFCON @ 0x510;
#define   POWER_POFCON_POF __BIT(1)
#define   POWER_POFCON_TRESHOLD _FIELD(1, 2)
#define     POWER_THRESHOLD_V21 0
#define     POWER_THRESHOLD_V23 1
#define     POWER_THRESHOLD_V25 2
#define     POWER_THRESHOLD_V27 3
    REGISTER unsigned GPREGRET @ 0x51c;
    REGISTER unsigned GPREGRET2 @ 0x520;
    REGISTER unsigned DCDCEN @ 0x578;
};

/* Interrupts */
#define POWER_INT_POFWARN 2

INSTANCE power POWER @ 0x40000000;


/* Watchdog timer */
DEVICE wdt {
/* Tasks */
    REGISTER unsigned START @ 0x000;
/* Events */
    REGISTER unsigned TIMEOUT @ 0x100;
/* Registers */
    REGISTER unsigned INTENSET @ 0x304;
    REGISTER unsigned INTENCLR @ 0x308;
    REGISTER unsigned RUNSTATUS @ 0x400;
    REGISTER unsigned REQSTATUS @ 0x404;
    REGISTER unsigned CRV @ 0x504;
#define   WDT_HERTZ 32768
    REGISTER unsigned RREN @ 0x508;
    REGISTER unsigned CONFIG @ 0x50c;
#define   WDT_CONFIG_SLEEP __BIT(0)
#define   WDT_CONFIG_HALT __BIT(3)
    REGISTER unsigned RR[8] @ 0x600;
#define   WDT_MAGIC 0x6e524635
};

/* Interrupts */
#define WDT_INT_TIMEOUT 0

INSTANCE wdt WDT @ 0x40010000;


/* Non-Volatile Memory Controller */
DEVICE nvmc {
    REGISTER unsigned READY @ 0x400;
    REGISTER unsigned CONFIG @ 0x504;
#define   NVMC_CONFIG_WEN __BIT(0)
#define   NVMC_CONFIG_EEN __BIT(1)
    REGISTER void *ERASEPAGE @ 0x508;
    REGISTER unsigned ICACHECONF @ 0x540;
#define   NVMC_ICACHECONF_CACHEEN __BIT(0)
};

INSTANCE nvmc NVMC @ 0x4001e000;


/* GPIO */
DEVICE gpio {
/* Registers */
    REGISTER unsigned OUT @ 0x004;
    REGISTER unsigned OUTSET @ 0x008;
    REGISTER unsigned OUTCLR @ 0x00c;
    REGISTER unsigned IN @ 0x010;
    REGISTER unsigned DIR @ 0x014;
    REGISTER unsigned DIRSET @ 0x018;
    REGISTER unsigned DIRCLR @ 0x01c;
    REGISTER unsigned PINCNF[32] @ 0x200;
#define   GPIO_PINCNF_DIR __FIELD(0, 1)
#define     GPIO_DIR_Input 0
#define     GPIO_DIR_Output 1
#define   GPIO_PINCNF_INPUT __FIELD(1, 1)
#define     GPIO_INPUT_Connect 0
#define     GPIO_INPUT_Disconnect 1
#define   GPIO_PINCNF_PULL __FIELD(2, 2)
#define     GPIO_PULL_Disabled 0
#define     GPII_PULL_Pulldown 1
#define     GPIO_PULL_Pullup 3
#define   GPIO_PINCNF_DRIVE __FIELD(8, 3)
#define     GPIO_DRIVE_S0S1 0
#define     GPIO_DRIVE_H0S1 1
#define     GPIO_DRIVE_S0H1 2
#define     GPIO_DRIVE_H0H1 3
#define     GPIO_DRIVE_D0S1 4
#define     GPIO_DRIVE_D0H1 5
#define     GPIO_DRIVE_S0D1 6 /* Open drain */
#define     GPIO_DRIVE_H0D1 7
#define   GPIO_PINCNF_SENSE __FIELD(16, 2)
#define     GPIO_SENSE_Disabled 0
#define     GPIO_SENSE_High 2
#define     GPIO_SENSE_Low 3
};

INSTANCE gpio GPIO0 @ 0x50000500;

INSTANCE gpio GPIO1 @ 0x50000800;


/* GPIOTE */
DEVICE gpiote {
/* Tasks */
    REGISTER unsigned OUT[4] @ 0x000;
    REGISTER unsigned SET[4] @ 0x030;
    REGISTER unsigned CLR[4] @ 0x060;
/* Events */
    REGISTER unsigned IN[4] @ 0x100;
    REGISTER unsigned PORT @ 0x17c;
/* Registers */
    REGISTER unsigned INTENSET @ 0x304;
    REGISTER unsigned INTENCLR @ 0x308;
    REGISTER unsigned CONFIG[4] @ 0x510;
#define   GPIOTE_CONFIG_MODE __FIELD(0, 2)
#define     GPIOTE_MODE_Event 1
#define     GPIOTE_MODE_Task 3
#define   GPIOTE_CONFIG_PSEL __FIELD(8, 6) // Note 6 bits inc port number
#define   GPIOTE_CONFIG_POLARITY __FIELD(16, 2)
#define     GPIOTE_POLARITY_LoToHi 1
#define     GPIOTE_POLARITY_HiToLo 2
#define     GPIOTE_POLARITY_Toggle 3
#define   GPIOTE_CONFIG_OUTINIT __FIELD(20, 1)
};

/* Interrupts */
#define GPIOTE_INT_IN0 0
#define GPIOTE_INT_IN1 1
#define GPIOTE_INT_IN2 2
#define GPIOTE_INT_IN3 3
#define GPIOTE_INT_PORT 31

INSTANCE gpiote GPIOTE @ 0x40006000;


/* PPI */
typedef struct { unsigned EN, DIS; } ppi_chg;

typedef struct { unsigned volatile *EEP, *TEP; } ppi_chan;

DEVICE ppi {
/* Tasks */
    REGISTER ppi_chg CHG[6] @ 0x000;
/* Registers */
    REGISTER unsigned CHEN @ 0x500;
    REGISTER unsigned CHENSET @ 0x504;
    REGISTER unsigned CHENCLR @ 0x508;
    REGISTER ppi_chan CH[20] @ 0x510;
    REGISTER unsigned CHGRP[6] @ 0x800;
};

INSTANCE ppi PPI @ 0x4001f000;


/* Radio */
DEVICE radio {
/* Tasks */
    REGISTER unsigned TXEN @ 0x000;
    REGISTER unsigned RXEN @ 0x004;
    REGISTER unsigned START @ 0x008;
    REGISTER unsigned STOP @ 0x00c;
    REGISTER unsigned DISABLE @ 0x010;
    REGISTER unsigned RSSISTART @ 0x014;
    REGISTER unsigned RSSISTOP @ 0x018;
    REGISTER unsigned BCSTART @ 0x01c;
    REGISTER unsigned BCSTOP @ 0x020;
/* Events */
    REGISTER unsigned READY @ 0x100;
    REGISTER unsigned ADDRESS @ 0x104;
    REGISTER unsigned PAYLOAD @ 0x108;
    REGISTER unsigned END @ 0x10c;
    REGISTER unsigned DISABLED @ 0x110;
    REGISTER unsigned DEVMATCH @ 0x114;
    REGISTER unsigned DEVMISS @ 0x118;
    REGISTER unsigned RSSIEND @ 0x11c;
    REGISTER unsigned BCMATCH @ 0x128;
/* Registers */
    REGISTER unsigned SHORTS @ 0x200;
    REGISTER unsigned INTENSET @ 0x304;
    REGISTER unsigned INTENCLR @ 0x308;
    REGISTER unsigned CRCSTATUS @ 0x400;
    REGISTER unsigned RXMATCH @ 0x408;
    REGISTER unsigned RXCRC @ 0x40c;
    REGISTER unsigned DAI @ 0x410;
    REGISTER void *PACKETPTR @ 0x504;
    REGISTER unsigned FREQUENCY @ 0x508;
    REGISTER unsigned TXPOWER @ 0x50c;
    REGISTER unsigned MODE @ 0x510;
#define   RADIO_MODE_NRF_1Mbit 0
    REGISTER unsigned PCNF0 @ 0x514;
#define   RADIO_PCNF0_LFLEN __FIELD(0, 4)
#define   RADIO_PCNF0_S0LEN __FIELD(8, 1)
#define   RADIO_PCNF0_S1LEN __FIELD(16, 4)
    REGISTER unsigned PCNF1 @ 0x518;
#define   RADIO_PCNF1_MAXLEN __FIELD(0, 8)
#define   RADIO_PCNF1_STATLEN __FIELD(8, 8)
#define   RADIO_PCNF1_BALEN __FIELD(16, 3)
#define   RADIO_PCNF1_ENDIAN __FIELD(24, 1)
#define     RADIO_ENDIAN_Little 0
#define     RADIO_ENDIAN_Big 1
#define   RADIO_PCNF1_WHITEEN __BIT(25)
    REGISTER unsigned BASE0 @ 0x51c;
    REGISTER unsigned BASE1 @ 0x520;
    REGISTER unsigned PREFIX0 @ 0x524;
    REGISTER unsigned PREFIX1 @ 0x528;
    REGISTER unsigned TXADDRESS @ 0x52c;
    REGISTER unsigned RXADDRESSES @ 0x530;
    REGISTER unsigned CRCCNF @ 0x534;
    REGISTER unsigned CRCPOLY @ 0x538;
    REGISTER unsigned CRCINIT @ 0x53c;
    REGISTER unsigned TEST @ 0x540;
    REGISTER unsigned TIFS @ 0x544;
    REGISTER unsigned RSSISAMPLE @ 0x548;
    REGISTER unsigned STATE @ 0x550;
    REGISTER unsigned DATAWHITEIV @ 0x554;
    REGISTER unsigned BCC @ 0x560;
    REGISTER unsigned DAB[8] @ 0x600;
    REGISTER unsigned DAP[8] @ 0x620;
    REGISTER unsigned DACNF @ 0x640;
    REGISTER unsigned OVERRIDE[5] @ 0x724;
    REGISTER unsigned POWER @ 0xffc;
};

/* Interrupts */
#define RADIO_INT_READY 0
#define RADIO_INT_END 3
#define RADIO_INT_DISABLED 4

INSTANCE radio RADIO @ 0x40001000;


/* TIMERS: Timers are all 8/16/24/32 bit; timers 3 and 4 have 6 CC registers
   instead of 4. */
DEVICE timer {
/* Tasks */
    REGISTER unsigned START @ 0x000;
    REGISTER unsigned STOP @ 0x004;
    REGISTER unsigned COUNT @ 0x008;
    REGISTER unsigned CLEAR @ 0x00c;
    REGISTER unsigned SHUTDOWN @ 0x010;
    REGISTER unsigned CAPTURE[6] @ 0x040;
/* Events */
    REGISTER unsigned COMPARE[6] @ 0x140;
/* Registers */
    REGISTER unsigned SHORTS @ 0x200;
    REGISTER unsigned INTENSET @ 0x304;
    REGISTER unsigned INTENCLR @ 0x308;
    REGISTER unsigned MODE @ 0x504;
#define   TIMER_MODE_Timer 0
#define   TIMER_MODE_Counter 1
    REGISTER unsigned BITMODE @ 0x508;
#define   TIMER_BITMODE_16Bit 0
#define   TIMER_BITMODE_8Bit 1
#define   TIMER_BITMODE_24Bit 2
#define   TIMER_BITMODE_32Bit 3
    REGISTER unsigned PRESCALER @ 0x510;
    REGISTER unsigned CC[6] @ 0x540;
};

/* Interrupts */
#define TIMER_INT_COMPARE0 16
#define TIMER_INT_COMPARE1 17
#define TIMER_INT_COMPARE2 18
#define TIMER_INT_COMPARE3 19
#define TIMER_INT_COMPARE4 20
#define TIMER_INT_COMPARE5 21
/* Shortcuts */
#define TIMER_COMPARE0_CLEAR 0
#define TIMER_COMPARE1_CLEAR 1
#define TIMER_COMPARE2_CLEAR 2
#define TIMER_COMPARE3_CLEAR 3
#define TIMER_COMPARE4_CLEAR 4
#define TIMER_COMPARE5_CLEAR 5
#define TIMER_COMPARE0_STOP 8
#define TIMER_COMPARE1_STOP 9
#define TIMER_COMPARE2_STOP 10
#define TIMER_COMPARE3_STOP 11
#define TIMER_COMPARE4_STOP 12
#define TIMER_COMPARE5_STOP 13

INSTANCE timer TIMER0 @ 0x40008000;

INSTANCE timer TIMER1 @ 0x40009000;

INSTANCE timer TIMER2 @ 0x4000a000;

INSTANCE timer TIMER3 @ 0x4001a000;

INSTANCE timer TIMER4 @ 0x4001b000;


/* Random Number Generator */
DEVICE rng {
/* Tasks */
    REGISTER unsigned START @ 0x000;
    REGISTER unsigned STOP @ 0x004;
/* Events */
    REGISTER unsigned VALRDY @ 0x100;
/* Registers */
    REGISTER unsigned SHORTS @ 0x200;
    REGISTER unsigned INTEN @ 0x300;
    REGISTER unsigned INTENSET @ 0x304;
    REGISTER unsigned INTENCLR @ 0x308;
    REGISTER unsigned CONFIG @ 0x504;
#define RNG_CONFIG_DERCEN __BIT(0)
    REGISTER unsigned VALUE @ 0x508;
};

/* Interrupts */
#define RNG_INT_VALRDY 0

INSTANCE rng RNG @ 0x4000d000;


/* Temperature sensor */
DEVICE temp {
/* Tasks */
    REGISTER unsigned START @ 0x000;
    REGISTER unsigned STOP @ 0x004;
/* Events */
    REGISTER unsigned DATARDY @ 0x100;
/* Registers */
    REGISTER unsigned INTEN @ 0x300;
    REGISTER unsigned INTENSET @ 0x304;
    REGISTER unsigned INTENCLR @ 0x308;
    REGISTER unsigned VALUE @ 0x508;
};

/* Interrupts */
#define TEMP_INT_DATARDY 0

INSTANCE temp TEMP @ 0x4000c000;


/* Separate internal and external I2C buses */
#define I2C_INTERNAL 0
#define I2C_EXTERNAL 1
#define N_I2CS 2

DEVICE i2c {
/* Tasks */
    REGISTER unsigned STARTRX @ 0x000;
    REGISTER unsigned STARTTX @ 0x008;
    REGISTER unsigned STOP @ 0x014;
    REGISTER unsigned SUSPEND @ 0x01c;
    REGISTER unsigned RESUME @ 0x020;
/* Events */
    REGISTER unsigned STOPPED @ 0x104;
    REGISTER unsigned RXDREADY @ 0x108;
    REGISTER unsigned TXDSENT @ 0x11c;
    REGISTER unsigned ERROR @ 0x124;
    REGISTER unsigned BB @ 0x138;
    REGISTER unsigned SUSPENDED @ 0x148;
/* Registers */
    REGISTER unsigned SHORTS @ 0x200;
    REGISTER unsigned INTEN @ 0x300;
    REGISTER unsigned INTENSET @ 0x304;
    REGISTER unsigned INTENCLR @ 0x308;
    REGISTER unsigned ERRORSRC @ 0x4c4;
#define   I2C_ERRORSRC_OVERRUN __BIT(0)
#define   I2C_ERRORSRC_ANACK __BIT(1)
#define   I2C_ERRORSRC_DNACK __BIT(2)
#define   I2C_ERRORSRC_All 0x7
    REGISTER unsigned ENABLE @ 0x500;
#define   I2C_ENABLE_Disabled 0
#define   I2C_ENABLE_Enabled 5
    REGISTER unsigned PSELSCL @ 0x508;
    REGISTER unsigned PSELSDA @ 0x50c;
    REGISTER unsigned RXD @ 0x518;
    REGISTER unsigned TXD @ 0x51c;
    REGISTER unsigned FREQUENCY @ 0x524;
#define   I2C_FREQUENCY_100kHz 0x01980000
    REGISTER unsigned ADDRESS @ 0x588;
    REGISTER unsigned POWER @ 0xffc;
};

/* Interrupts */
#define I2C_INT_STOPPED 1
#define I2C_INT_RXDREADY 2
#define I2C_INT_TXDSENT 7
#define I2C_INT_ERROR 9
#define I2C_INT_BB 14
/* Shortcuts */
#define I2C_BB_SUSPEND 0
#define I2C_BB_STOP 1

INSTANCE i2c I2C0 @ 0x40003000;

INSTANCE i2c I2C1 @ 0x40004000;


/* UART */
DEVICE uart {
/* Tasks */
    REGISTER unsigned STARTRX @ 0x000;
    REGISTER unsigned STARTTX @ 0x008;
/* Events */
    REGISTER unsigned RXDRDY @ 0x108;
    REGISTER unsigned TXDRDY @ 0x11c;
/* Registers */
    REGISTER unsigned INTENSET @ 0x304;
    REGISTER unsigned INTENCLR @ 0x308;
    REGISTER unsigned ENABLE @ 0x500;
#define   UART_ENABLE_Disabled 0
#define   UART_ENABLE_Enabled 4
    REGISTER unsigned PSELTXD @ 0x50c;
    REGISTER unsigned PSELRXD @ 0x514;
    REGISTER unsigned RXD @ 0x518;
    REGISTER unsigned TXD @ 0x51c;
    REGISTER unsigned BAUDRATE @ 0x524;
#define   UART_BAUDRATE_1200   0x0004f000
#define   UART_BAUDRATE_2400   0x0009d000
#define   UART_BAUDRATE_4800   0x0013b000
#define   UART_BAUDRATE_9600   0x00275000
#define   UART_BAUDRATE_14400  0x003af000
#define   UART_BAUDRATE_19200  0x004ea000
#define   UART_BAUDRATE_28800  0x0075c000
#define   UART_BAUDRATE_31250  0x00800000
#define   UART_BAUDRATE_38400  0x009d0000
#define   UART_BAUDRATE_56000  0x00e50000
#define   UART_BAUDRATE_57600  0x00eb0000
#define   UART_BAUDRATE_76800  0x013a9000
#define   UART_BAUDRATE_115200 0x01d60000
#define   UART_BAUDRATE_230400 0x03b00000
#define   UART_BAUDRATE_250000 0x04000000
#define   UART_BAUDRATE_460800 0x07400000
#define   UART_BAUDRATE_921600 0x0f000000
#define   UART_BAUDRATE_1M     0x10000000
    REGISTER unsigned CONFIG @ 0x56c;
#define   UART_CONFIG_HWFC __BIT(0)
#define   UART_CONFIG_PARITY __FIELD(1, 3)
#define     UART_PARITY_None 0
#define     UART_PARITY_Even 7
};

/* Interrupts */
#define UART_INT_RXDRDY 2
#define UART_INT_TXDRDY 7

INSTANCE uart UART @ 0x40002000;


/* UARTE -- UART with EasyDMA */
DEVICE uarte {
/* Tasks */
    REGISTER unsigned STARTRX @ 0x000;
    REGISTER unsigned STOPRX @ 0x004;
    REGISTER unsigned STARTTX @ 0x008;
    REGISTER unsigned STOPTX @ 0x00c;
    REGISTER unsigned FLUSHRX @ 0x02c;
/* Events */
    REGISTER unsigned CTS @ 0x100;
    REGISTER unsigned NTCS @ 0x104;
    REGISTER unsigned RXDRDY @ 0x108;
    REGISTER unsigned ENDRX @ 0x110;
    REGISTER unsigned TXDRDY @ 0x11c;
    REGISTER unsigned ENDTX @ 0x120;
    REGISTER unsigned ERROR @ 0x124;
    REGISTER unsigned RXTO @ 0x144;
    REGISTER unsigned RXSTARTED @ 0x14c;
    REGISTER unsigned TXSTARTED @ 0x150;
    REGISTER unsigned TXSTOPPED @ 0x158;
/* Registers */
    REGISTER unsigned SHORTS @ 0x200;
    REGISTER unsigned INTEN @ 0x300;
    REGISTER unsigned INTENSET @ 0x304;
    REGISTER unsigned INTENCLR @ 0x308;
    REGISTER unsigned ERRORSRC @ 0x480;
#define   UARTE_ERROR_OVERRUN __BIT(0)
#define   UARTE_ERROR_PARITY __BIT(1)
#define   UARTE_ERROR_FRAMING __BIT(2)
#define   UARTE_ERROR_BREAK __BIT(3)
    REGISTER unsigned ENABLE @ 0x500;
#define   UARTE_ENABLE_Disabled 0
#define   UARTE_ENABLE_Enabled 8
    REGISTER unsigned PSELRTS @ 0x508;
    REGISTER unsigned PSELTXD @ 0x50c;
    REGISTER unsigned PSELCTS @ 0x510;
    REGISTER unsigned PSELRXD @ 0x514;
    REGISTER unsigned BAUDRATE @ 0x524;
#define   UARTE_BAUDRATE_1200   0x0004f000
#define   UARTE_BAUDRATE_2400   0x0009d000
#define   UARTE_BAUDRATE_4800   0x0013b000
#define   UARTE_BAUDRATE_9600   0x00275000
#define   UARTE_BAUDRATE_14400  0x003af000
#define   UARTE_BAUDRATE_19200  0x004ea000
#define   UARTE_BAUDRATE_28800  0x0075c000
#define   UARTE_BAUDRATE_31250  0x00800000
#define   UARTE_BAUDRATE_38400  0x009d0000
#define   UARTE_BAUDRATE_56000  0x00e50000
#define   UARTE_BAUDRATE_57600  0x00eb0000
#define   UARTE_BAUDRATE_76800  0x013a9000
#define   UARTE_BAUDRATE_115200 0x01d60000
#define   UARTE_BAUDRATE_230400 0x03b00000
#define   UARTE_BAUDRATE_250000 0x04000000
#define   UARTE_BAUDRATE_460800 0x07400000
#define   UARTE_BAUDRATE_921600 0x0f000000
#define   UARTE_BAUDRATE_1M     0x10000000
    REGISTER dma_param RXD @ 0x534;
    REGISTER dma_param TXD @ 0x544;
    REGISTER unsigned CONFIG @ 0x56c;
#define   UARTE_CONFIG_HWFC __BIT(0)
#define   UARTE_CONFIG_PARITY __FIELD(1, 3)
#define     UARTE_PARITY_Disabled 0
#define     UARTE_PARITY_Enabled 7
#define   UARTE_CONFIG_STOP __FIELD(4, 1)
#define     UARTE_STOP_1 0
#define     UARTE_STOP_2 1
#define   UARTE_CONFIG_PARITYTYPE __FIELD(8, 1)
#define     UARTE_PARITYTYPE_Even 0
#define     UARTE_PARITYTYPE_Odd 1
};

/* Interrupts */
#define UARTE_INT_CTS 0
#define UARTE_INT_NCTS 1
#define UARTE_INT_RXDRDY 2
#define UARTE_INT_ENDRX 4
#define UARTE_INT_TXDRDY 7
#define UARTE_INT_ENDTX 8
#define UARTE_INT_ERROR 9
#define UARTE_INT_RXTO 17
#define UARTE_INT_RXSTARTED 19
#define UARTE_INT_TXSTARTED 20
#define UARTE_INT_TXSTOPPED 22

/* Shortcuts */
#define UARTE_ENDRX_STARTRX 5
#define UARTE_ENDRX_STOPRX 6

INSTANCE uarte UARTE0 @ 0x40002000;

INSTANCE uarte UARTE1 @ 0x40028000;


/* SAADC -- Successive approximation ADC */
typedef struct {
    unsigned PSELP;
    unsigned PSELN;
    unsigned CONFIG;
    unsigned LIMIT;
} adc_chan;

DEVICE adc {
/* Tasks */
    REGISTER unsigned START @ 0x000;
    REGISTER unsigned SAMPLE @ 0x004;
    REGISTER unsigned STOP @ 0x008;
    REGISTER unsigned CALIBRATE @ 0x00c;
/* Events */
    REGISTER unsigned STARTED @ 0x100;
    REGISTER unsigned END @ 0x104;
    REGISTER unsigned DONE @ 0x108;
    REGISTER unsigned RESULTDONE @ 0x10c;
    REGISTER unsigned CALDONE @ 0x110;
    REGISTER unsigned STOPPED @ 0x114;
/* Registers */
    REGISTER unsigned INTEN @ 0x300;
    REGISTER unsigned INTENSET @ 0x304;
    REGISTER unsigned INTENCLR @ 0x308;
    REGISTER unsigned BUSY @ 0x400;
    REGISTER unsigned ENABLE @ 0x500;
    REGISTER adc_chan CHAN[8] @ 0x510;
#define   ADC_CONFIG_RESP __FIELD(0, 2)
#define   ADC_CONFIG_RESN __FIELD(4, 2)
#define     ADC_RES_Bypass 0
#define     ADC_RES_Pulldown 1
#define     ADC_RES_Pullup 2
#define     ADC_RES_VDD_1_2 3
#define   ADC_CONFIG_GAIN __FIELD(8, 3)
#define     ADC_GAIN_1_6 0
#define     ADC_GAIN_1_5 1
#define     ADC_GAIN_1_4 2
#define     ADC_GAIN_1_3 3
#define     ADC_GAIN_1_2 4
#define     ADC_GAIN_1 5
#define     ADC_GAIN_2 6
#define     ADC_GAIN_3 7
#define   ADC_CONFIG_REFSEL __FIELD(12, 1)
#define     ADC_REFSEL_Internal 0
#define     ADC_REFSEL_VDD_1_4 1
#define   ADC_CONFIG_TACQ __FIELD(16, 3)
#define     ADC_TACQ_3us 0
#define     ADC_TACQ_5us 1
#define     ADC_TACQ_10us 2
#define     ADC_TACQ_15us 3
#define     ADC_TACQ_20us 4
#define     ADC_TACQ_40us 5
#define  ADC_CONFIG_MODE __FIELD(20, 1)
#define     ADC_MODE_SE 0
#define     ADC_MODE_Diff 1
#define  ADC_CONFIG_BURST __FIELD(24, 1)
#define     ADC_BURST_Disabled 0
#define     ADC_BURST_Enabled 1
    REGISTER unsigned RESOLUTION @ 0x5f0;
#define   ADC_RESOLUTION_8bit 0
#define   ADC_RESOLUTION_10bit 1
#define   ADC_RESOLUTION_12bit 2
#define   ADC_RESOLUTION_14bit 3
    REGISTER unsigned OVERSAMPLE @ 0x5f4;
    REGISTER unsigned SAMPLERATE @ 0x5f8;
    REGISTER dma_param RESULT @ 0x62c;
};

/* Interrupts */
#define ADC_INT_STARTED 0
#define ADC_INT_END 1
#define ADC_INT_DONE 2
#define ADC_INT_RESULTDONE 3
#define ADC_INT_CALDONE 4
#define ADC_INT_STOPPED 5

INSTANCE adc ADC @ 0x40007000;


/* PWM */
typedef struct {
    void *PTR;
    unsigned CNT;
    unsigned REFRESH;
    unsigned ENDDELAY;
    unsigned char filler[16];
} pwm_sequence;

DEVICE pwm {
/* Tasks */
    REGISTER unsigned STOP @ 0x004;
    REGISTER unsigned SEQSTART[2] @ 0x008;
    REGISTER unsigned NEXTSTEP @ 0x010;
/* Events */
    REGISTER unsigned STOPPED @ 0x104;
    REGISTER unsigned SEQSTARTED[2] @ 0x108;
    REGISTER unsigned SEQEND[2] @ 0x110;
    REGISTER unsigned PWMPERIODEND @ 0x118;
    REGISTER unsigned LOOPSDONE @ 0x11c;
/* Registers */
    REGISTER unsigned SHORTS @ 0x200;
    REGISTER unsigned INTEN @ 0x300;
    REGISTER unsigned INTENSET @ 0x304;
    REGISTER unsigned INTENCLR @ 0x308;
    REGISTER unsigned ENABLE @ 0x500;
    REGISTER unsigned MODE @ 0x504;
#define   PWM_MODE_Up 0
#define   PWM_MODE_UpAndDown 1
    REGISTER unsigned COUNTERTOP @ 0x508;
    REGISTER unsigned PRESCALER @ 0x50c;
    REGISTER unsigned DECODER @ 0x510;
#define   PWM_DECODER_LOAD __FIELD(0, 2)
#define     PWM_LOAD_Common 0
#define     PWM_LOAD_Grouped 1
#define     PWM_LOAD_Individual 2
#define     PWM_LOAD_Waveform 3
#define   PWM_DECODER_MODE __FIELD(8, 1)
#define     PWM_MODE_RefreshCount 0
#define     PWM_MODE_NextStep 1
    REGISTER unsigned LOOP @ 0x514;
    REGISTER pwm_sequence SEQ[2] @ 0x520;
#define   PWM_SEQ_COMPARE __FIELD(0, 15)
#define   PWM_SEQ_POLARITY __FIELD(15, 1)
#define     PWM_POLARITY_RisingEdge 0
#define     PWM_POLARITY_FallingEdge 1
    REGISTER unsigned PSEL[4] @ 0x560;
};

/* Interrupts */
#define PWM_INT_STOPPED 1
#define PWM_INT_SEQSTARTED0 2
#define PWM_INT_SEQSTARTED1 3
#define PWM_INT_SEQEND0 4
#define PWM_INT_SEQEND1 5
#define PWM_INT_PWMPERIODEND 6
#define PWM_INT_LOOPSDONE 7

/* Shortcuts */
#define PWM_SEQEND0_STOP 0
#define PWM_SEQEND1_STOP 1
#define PWM_LOOPSDONE_SEQSTART0 2
#define PWM_LOOPSDONE_SEQSTART1 3
#define PWM_LOOPSDONE_STOP 4

/* Sample format */
#define PWM_SAMPLE(c, p) \
    (FIELD(PWM_SEQ_COMPARE, c) | FIELD(PWM_SEQ_POLARITY, p))
#define RISING PWM_POLARITY_RisingEdge
#define FALLING PWM_POLARITY_FallingEdge

INSTANCE pwm PWM0 @ 0x4001c000;

INSTANCE pwm PWM1 @ 0x40021000;

INSTANCE pwm PWM2 @ 0x40022000;

INSTANCE pwm PWM3 @ 0x4002d000;


/* NVIC stuff */

/* irq_priority -- set priority of an IRQ from 0 (highest) to 255 */
void irq_priority(int irq, unsigned priority);

/* enable_irq -- enable interrupts from an IRQ */
#define enable_irq(irq)  NVIC_ISER[0] = BIT(irq)

/* disable_irq -- disable interrupts from a specific IRQ */
#define disable_irq(irq)  NVIC_ICER[0] = BIT(irq)

/* clear_pending -- clear pending interrupt from an IRQ */
#define clear_pending(irq)  NVIC_ICPR[0] = BIT(irq)

/* reschedule -- request PendSV interrupt */
#define reschedule()  SCB_ICSR = BIT(SCB_ICSR_PENDSVSET)

/* active_irq -- find active interrupt: returns -16 to 31 */
#define active_irq()  (GET_FIELD(SCB_ICSR, SCB_ICSR_VECTACTIVE) - 16)

/* delay_loop -- timed delay */
void delay_loop(unsigned usec);


/* GPIO convenience */

#ifndef INLINE
#define INLINE inline
#endif

/* Some C language tricks are needed for us to refer to the two GPIO
   ports uniformly. */

extern unsigned volatile * const gpio_base[2];

#define _GPIO_REG(port, reg) \
    (* (gpio_base[port] + (&GPIO0_##reg - GPIO0_BASE)))

#define _GPIO_PINCNF(pin) \
    ((gpio_base[PORT(pin)] + (GPIO0_PINCNF - GPIO0_BASE))[PIN(pin)])

/* gpio_dir -- set GPIO direction */
INLINE void gpio_dir(unsigned pin, unsigned dir) {
    if (dir)
        _GPIO_REG(PORT(pin), DIRSET) = BIT(PIN(pin));
    else
        _GPIO_REG(PORT(pin), DIRCLR) = BIT(PIN(pin));
}

/* gpio_connect -- connect pin for input */
INLINE void gpio_connect(unsigned pin) {
    SET_FIELD(_GPIO_PINCNF(pin), GPIO_PINCNF_INPUT, GPIO_INPUT_Connect);
}

/* gpio_drive -- set GPIO drive strength */
INLINE void gpio_drive(unsigned pin, unsigned mode) {
    SET_FIELD(_GPIO_PINCNF(pin), GPIO_PINCNF_DRIVE, mode);
}

/* gpio_out -- set GPIO output value */
INLINE void gpio_out(unsigned pin, unsigned value) {
    if (value)
        _GPIO_REG(PORT(pin), OUTSET) = BIT(PIN(pin));
    else
        _GPIO_REG(PORT(pin), OUTCLR) = BIT(PIN(pin));
}

/* gpio_in -- get GPIO input bit */
INLINE unsigned gpio_in(unsigned pin) {
    return GET_BIT(_GPIO_REG(PORT(pin), IN), PIN(pin));
}


/* Image constants */

#define NIMG 10

typedef unsigned image[NIMG];

#define LED_MASK0 0xd1688800
#define LED_MASK1 0x00000020
#define LED_DOT0  0x50008800
#define LED_DOT1  0x00000020

#define _ROW(r, c1, c2, c3, c4, c5) \
    BIT(r) | (!c1<<28) | (!c2<<11) | (!c3<<31) | (!c5<<30), (!c4<<5)

#define IMAGE(x11, x12, x13, x14, x15, \
              x21, x22, x23, x24, x25, \
              x31, x32, x33, x34, x35, \
              x41, x42, x43, x44, x45, \
              x51, x52, x53, x54, x55) \
    { _ROW(ROW1, x11, x12, x13, x14, x15), \
      _ROW(ROW2, x21, x22, x23, x24, x25), \
      _ROW(ROW3, x31, x32, x33, x34, x35), \
      _ROW(ROW4, x41, x42, x43, x44, x45), \
      _ROW(ROW5, x51, x52, x53, x54, x55) }

#define led_init()  GPIO0_DIRSET = LED_MASK0, GPIO1_DIRSET = LED_MASK1
#define led_dot()   GPIO0_OUTSET = LED_DOT0, GPIO1_OUTSET = LED_DOT1
#define led_off()   GPIO0_OUTCLR = LED_MASK0, GPIO1_OUTCLR = LED_MASK1


/* CODERAM -- mark function for copying to RAM */
#define CODERAM  __attribute((noinline, section(".xram")))

/* A few assembler macros for single instructions. */
#define intr_disable()  asm volatile ("cpsid i")
#define intr_enable()   asm volatile ("cpsie i")
#define get_primask()   ({ unsigned x;                                   \
                           asm volatile ("mrs %0, primask" : "=r"(x)); x; })
#define set_primask(x)  asm volatile ("msr primask, %0" : : "r"(x))
#define nop()           asm volatile ("nop")
#define syscall(op)     asm volatile ("svc %0" : : "i"(op))

/* pause() -- disabled on V2 owing to long wakeup time */
#define pause()         /* asm volatile ("wfe") */
