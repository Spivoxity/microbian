// ubit-v2/hardware.h
// Copyright (c) 2018-20 J. M. Spivey

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

#define SET_FIELD(reg, field, val) __SET_FIELD(reg, field, val)
#define __SET_FIELD(reg, pos, wid, val) \
    reg = (reg & ~__MASK(pos, wid)) | __FIELD(pos, wid, val)

#define GET_FIELD(reg, field) __GET_FIELD(reg, field)
#define __GET_FIELD(reg, pos, wid)  ((reg >> pos) & __MASK0(wid))

#define FIELD(field, val) __FIELD(field, val)
#define __FIELD(pos, wid, val)  (((val) & __MASK0(wid)) << pos)

#define MASK(field) __MASK(field)
#define __MASK(pos, wid)  (__MASK0(wid) << pos)

#define __MASK0(wid)  (~((-2) << (wid-1)))

#define ADDR(x) (* (unsigned volatile *) (x))
#define POINTER(x) (* (void * volatile *) (x))
#define ARRAY(x) ((unsigned volatile *) (x))

/* Device pins */
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

#define I2C_INTERNAL 0
#define I2C_EXTERNAL 1
#define N_I2CS 2

/* TODO: Logo touch, mic power, mic input, speaker */

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

// For compatibility, allow UART as a synonym for UART0
#define UART_IRQ UART0_IRQ
#define uart_handler uart0_handler

/* System control block */
#define SCB_CPUID              ADDR(0xe000ed00)
#define SCB_ICSR               ADDR(0xe000ed04)
#define   SCB_ICSR_PENDSVSET 28
#define   SCB_ICSR_VECTACTIVE 0, 8
#define SCB_SCR                ADDR(0xe000ed10)
#define   SCB_SCR_SLEEPONEXIT 1
#define   SCB_SCR_SLEEPDEEP 2
#define   SCB_SCR_SEVONPEND 4
#define SCB_SHPR              ARRAY(0xe000ed1c)

/* Nested vector interupt controller */
#define NVIC_ISER             ARRAY(0xe000e100)
#define NVIC_ICER             ARRAY(0xe000e180)
#define NVIC_ISPR             ARRAY(0xe000e200)
#define NVIC_ICPR             ARRAY(0xe000e280)
#define NVIC_IPR              ARRAY(0xe000e400)

/* Systick timer */
#define SYST_CSR               ADDR(0xe000e010)
#define  SYST_CSR_COUNTFLAG 16
#define  SYST_CSR_CLKSOURCE 2, 1
#define    SYST_CLKSOURCE_External 0
#define    SYST_CLKSOURCE_Internal 1
#define  SYST_CSR_TICKINT 1
#define  SYST_CSR_ENABLE 0            
#define SYST_RVR               ADDR(0xe000e014)
#define SYST_CVR               ADDR(0xe000e018)
#define SYST_CALIB             ADDR(0xe000e01c)
#define   SYST_CALIB_NOREF 31
#define   SYST_CALIB_SKEW 30
#define   SYST_CALIB_TENMS 0, 24

#define SYSTICK_CLOCK 64000000

/* Power */
#define POWER_CONSTLAT         ADDR(0x40000078)

/* Clock control */
#define CLOCK_HFCLKSTART       ADDR(0x40000000)
#define CLOCK_LFCLKSTART       ADDR(0x40000008)
#define CLOCK_HFCLKSTARTED     ADDR(0x40000100)
#define CLOCK_LFCLKSTARTED     ADDR(0x40000104)
#define CLOCK_LFCLKSRC         ADDR(0x40000518)
#define CLOCK_XTALFREQ         ADDR(0x40000550)
     
#define CLOCK_LFCLKSRC_RC 0
#define CLOCK_XTALFREQ_16MHz 0xFF

#define MPU_DISABLEINDEBUG ADDR(0x40000608)

/* Factory information */
#define FICR_DEVICEID  	       ARRAY(0x10000060)
#define FICR_OVERRIDEEN 	ADDR(0x100000a0)
#define FICR_OVERRIDEEN_NRF 0
#define FICR_NRF_1MBIT ARRAY(0x100000b0)

/* Non-Volatile Memory Controller */
#define NVMC_READY              ADDR(0x4001e400)
#define NVMC_CONFIG             ADDR(0x4001e504)
#define   NVMC_CONFIG_WEN 0
#define   NVMC_CONFIG_EEN 1   
#define NVMC_ERASEPAGE           (* (void * volatile *) 0x4001e508)
#define NVMC_ICACHECONF         ADDR(0x4001e540)
#define   NVMC_ICACHECONF_CACHEEN 0


/* Device register structures */
#define _DEVUNION(tag, size) \
    union tag { \
        unsigned volatile reg[1]; \
        unsigned volatile arr[1][1]; \
        unsigned char strut[size]; }

_DEVUNION(_dev, 4);
#define REG(i) reg[i>>2]
#define ARR(i) arr[i>>2]


/* GPIO */
_DEVUNION(_gpio, 0x300);
#define GPIO ((union _gpio *) 0x50000500)

// Registers
#define G_OUT       		 REG(0x004)
#define G_OUTSET    		 REG(0x008)
#define G_OUTCLR    		 REG(0x00c)
#define G_IN        		 REG(0x010)
#define G_DIR       		 REG(0x014)
#define G_DIRSET    		 REG(0x018)
#define G_DIRCLR    		 REG(0x01c)
#define G_PINCNF   		 ARR(0x200)
#define   GPIO_PINCNF_DIR 0, 1
#define     GPIO_DIR_Input 0
#define     GPIO_DIR_Output 1
#define   GPIO_PINCNF_INPUT 1, 1
#define     GPIO_INPUT_Connect 0
#define     GPIO_INPUT_Disconnect 1
#define   GPIO_PINCNF_PULL 2, 2
#define     GPIO_PULL_Disabled 0
#define     GPII_PULL_Pulldown 1    
#define     GPIO_PULL_Pullup 3
#define   GPIO_PINCNF_DRIVE 8, 3
#define     GPIO_DRIVE_S0S1 0
#define     GPIO_DRIVE_H0S1 1
#define     GPIO_DRIVE_S0H1 2
#define     GPIO_DRIVE_S0D1 6 // Open drain
#define   GPIO_PINCNF_SENSE 16, 2
#define     GPIO_SENSE_Disabled 0
#define     GPIO_SENSE_High 2
#define     GPIO_SENSE_Low 3

/* Synonyms for clarity */
#define GPIO0_OUT    GPIO[0].G_OUT
#define GPIO0_OUTSET GPIO[0].G_OUTSET
#define GPIO0_OUTCLR GPIO[0].G_OUTCLR
#define GPIO0_IN     GPIO[0].G_IN
#define GPIO0_DIR    GPIO[0].G_DIR
#define GPIO0_DIRSET GPIO[0].G_DIRSET
#define GPIO0_DIRCLR GPIO[0].G_DIRCLR
#define GPIO0_PINCNF GPIO[0].G_PINCNF

#define GPIO1_OUT    GPIO[1].G_OUT
#define GPIO1_OUTSET GPIO[1].G_OUTSET
#define GPIO1_OUTCLR GPIO[1].G_OUTCLR
#define GPIO1_IN     GPIO[1].G_IN
#define GPIO1_DIR    GPIO[1].G_DIR
#define GPIO1_DIRSET GPIO[1].G_DIRSET
#define GPIO1_DIRCLR GPIO[1].G_DIRCLR
#define GPIO1_PINCNF GPIO[1].G_PINCNF


/* GPIOTE */
// Tasks
#define GPIOTE_OUT    	       ARRAY(0x40006000)
#define GPIOTE_SET             ARRAY(0x40006030)
#define GPIOTE_CLR             ARRAY(0x40006060)
// Events
#define GPIOTE_IN      	       ARRAY(0x40006100)
#define GPIOTE_PORT    		ADDR(0x4000617c)
// Registers
#define GPIOTE_INTENSET 	ADDR(0x40006304)
#define GPIOTE_INTENCLR 	ADDR(0x40006308)
#define GPIOTE_CONFIG 	       ARRAY(0x40006510)
#define   GPIOTE_CONFIG_MODE 0, 2
#define     GPIOTE_MODE_Event 1
#define     GPIOTE_MODE_Task 3
#define   GPIOTE_CONFIG_PSEL 8, 6 // Note 6 bits including port number
#define   GPIOTE_CONFIG_POLARITY 16, 2
#define     GPIOTE_POLARITY_LoToHi 1
#define     GPIOTE_POLARITY_HiToLo 2
#define     GPIOTE_POLARITY_Toggle 3
#define   GPIOTE_CONFIG_OUTINIT 20, 1
// Interrupts
#define GPIOTE_INT_IN0 0
#define GPIOTE_INT_IN1 1
#define GPIOTE_INT_IN2 2
#define GPIOTE_INT_IN3 3
#define GPIOTE_INT_IN4 4
#define GPIOTE_INT_IN5 5
#define GPIOTE_INT_IN6 6
#define GPIOTE_INT_IN7 7
#define GPIOTE_INT_PORT 31

/* PPI */
struct _ppi_chg {
    unsigned volatile EN;
    unsigned volatile DIS;
};
    
struct _ppi_ch {
    unsigned volatile *EEP;
    unsigned volatile *TEP;
};

struct _ppi_frk {
    unsigned volatile *TEP;
};

#define PPI_CHG  ((struct _ppi_chg *) 0x4001f000)
#define PPI_CHEN       		ADDR(0x4001f500)
#define PPI_CHENSET    		ADDR(0x4001f504)
#define PPI_CHENCLR    		ADDR(0x4001f508)
#define PPI_CH    ((struct _ppi_ch *) 0x4001f510)
#define PPI_CHGRP 	       ARRAY(0x4001f800)
#define PPI_FORK ((struct _ppi_frk *) 0x4001f910)


/* Radio */
// Tasks
#define RADIO_TXEN     		ADDR(0x40001000)
#define RADIO_RXEN     		ADDR(0x40001004)
#define RADIO_START    		ADDR(0x40001008)
#define RADIO_STOP     		ADDR(0x4000100c)
#define RADIO_DISABLE  		ADDR(0x40001010)
#define RADIO_RSSISTART		ADDR(0x40001014)
#define RADIO_RSSISTOP 		ADDR(0x40001018)
#define RADIO_BCSTART  		ADDR(0x4000101c)
#define RADIO_BCSTOP   		ADDR(0x40001020)
// Events
#define RADIO_READY    		ADDR(0x40001100)
#define RADIO_ADDRESS  		ADDR(0x40001104)
#define RADIO_PAYLOAD  		ADDR(0x40001108)
#define RADIO_END      		ADDR(0x4000110c)
#define RADIO_DISABLED 		ADDR(0x40001110)
#define RADIO_DEVMATCH 		ADDR(0x40001114)
#define RADIO_DEVMISS  		ADDR(0x40001118)
#define RADIO_RSSIEND  		ADDR(0x4000111c)
#define RADIO_BCMATCH  		ADDR(0x40001128)
// Registers
#define RADIO_SHORTS   		ADDR(0x40001200)
#define RADIO_INTENSET 		ADDR(0x40001304)
#define RADIO_INTENCLR 		ADDR(0x40001308)
#define RADIO_CRCSTATUS 	ADDR(0x40001400)
#define RADIO_RXMATCH  		ADDR(0x40001408)
#define RADIO_RXCRC    		ADDR(0x4000140c)
#define RADIO_DAI      		ADDR(0x40001410)
#define RADIO_PACKETPTR 	ADDR(0x40001504)
#define RADIO_FREQUENCY 	ADDR(0x40001508)
#define RADIO_TXPOWER  		ADDR(0x4000150c)
#define RADIO_MODE     		ADDR(0x40001510)
#define   RADIO_MODE_NRF_1Mbit 0
#define RADIO_PCNF0    		ADDR(0x40001514)
#define   RADIO_PCNF0_LFLEN 0, 3
#define   RADIO_PCNF0_S0LEN 8, 1
#define   RADIO_PCNF0_S1LEN 16, 4
#define RADIO_PCNF1    		ADDR(0x40001518)
#define   RADIO_PCNF1_MAXLEN 0, 8
#define   RADIO_PCNF1_STATLEN 8, 8
#define   RADIO_PCNF1_BALEN 16, 3
#define   RADIO_PCNF1_ENDIAN 24, 1
#define     RADIO_ENDIAN_Little 0
#define     RADIO_ENDIAN_Big 1
#define   RADIO_PCNF1_WHITEEN 25
#define RADIO_BASE0    		ADDR(0x4000151c)
#define RADIO_BASE1    		ADDR(0x40001520)
#define RADIO_PREFIX0  		ADDR(0x40001524)
#define RADIO_PREFIX1  		ADDR(0x40001528)
#define RADIO_TXADDRESS 	ADDR(0x4000152c)
#define RADIO_RXADDRESSES	ADDR(0x40001530)
#define RADIO_CRCCNF   		ADDR(0x40001534)
#define RADIO_CRCPOLY  		ADDR(0x40001538)
#define RADIO_CRCINIT  		ADDR(0x4000153c)
#define RADIO_TEST     		ADDR(0x40001540)
#define RADIO_TIFS     		ADDR(0x40001544)
#define RADIO_RSSISAMPLE 	ADDR(0x40001548)
#define RADIO_STATE    		ADDR(0x40001550)
#define RADIO_DATAWHITEIV 	ADDR(0x40001554)
#define RADIO_BCC      		ADDR(0x40001560)
#define RADIO_DAB     	       ARRAY(0x40001600)
#define RADIO_DAP     	       ARRAY(0x40001620)
#define RADIO_DACNF    	        ADDR(0x40001640)
#define RADIO_OVERRIDE 	       ARRAY(0x40001724)
#define RADIO_POWER    		ADDR(0x40001ffc)
// Interrupts
#define RADIO_INT_READY 0
#define RADIO_INT_END 3
#define RADIO_INT_DISABLED 4


/* TIMERS: Timers 0, 1, 2 are all 8/16/24/32 bit.  
   There are two more timers at different addresses, plus a SysTick timer. */

// Timer 0
// Tasks
#define TIMER0_START     	ADDR(0x40008000)
#define TIMER0_STOP      	ADDR(0x40008004)
#define TIMER0_COUNT     	ADDR(0x40008008)
#define TIMER0_CLEAR     	ADDR(0x4000800c)
#define TIMER0_SHUTDOWN  	ADDR(0x40008010)
#define TIMER0_CAPTURE         ARRAY(0x40008040)
// Events
#define TIMER0_COMPARE         ARRAY(0x40008140)
// Registers
#define TIMER0_SHORTS    	ADDR(0x40008200)
#define TIMER0_INTENSET  	ADDR(0x40008304)
#define TIMER0_INTENCLR  	ADDR(0x40008308)
#define TIMER0_MODE      	ADDR(0x40008504)
#define   TIMER_MODE_Timer 0
#define   TIMER_MODE_Counter 1
#define TIMER0_BITMODE   	ADDR(0x40008508)
#define   TIMER_BITMODE_16Bit 0
#define   TIMER_BITMODE_8Bit 1
#define   TIMER_BITMODE_24Bit 2
#define   TIMER_BITMODE_32Bit 3
#define TIMER0_PRESCALER 	ADDR(0x40008510)
#define TIMER0_CC              ARRAY(0x40008540)
// Interrupts
#define TIMER_INT_COMPARE0 16
#define TIMER_INT_COMPARE1 17
#define TIMER_INT_COMPARE2 18
#define TIMER_INT_COMPARE3 19
// Shortcuts
#define TIMER_COMPARE0_CLEAR 0
#define TIMER_COMPARE1_CLEAR 1
#define TIMER_COMPARE2_CLEAR 2
#define TIMER_COMPARE3_CLEAR 3
#define TIMER_COMPARE0_STOP 8
#define TIMER_COMPARE1_STOP 9
#define TIMER_COMPARE2_STOP 10
#define TIMER_COMPARE3_STOP 11

// Timer 1
#define TIMER1_START     	ADDR(0x40009000)
#define TIMER1_STOP      	ADDR(0x40009004)
#define TIMER1_COUNT     	ADDR(0x40009008)
#define TIMER1_CLEAR     	ADDR(0x4000900c)
#define TIMER1_SHUTDOWN  	ADDR(0x40009010)
#define TIMER1_CAPTURE         ARRAY(0x40009040)
#define TIMER1_COMPARE         ARRAY(0x40009140)
#define TIMER1_SHORTS    	ADDR(0x40009200)
#define TIMER1_INTENSET  	ADDR(0x40009304)
#define TIMER1_INTENCLR  	ADDR(0x40009308)
#define TIMER1_MODE      	ADDR(0x40009504)
#define TIMER1_BITMODE   	ADDR(0x40009508)
#define TIMER1_PRESCALER 	ADDR(0x40009510)
#define TIMER1_CC              ARRAY(0x40009540)

// Timer 2
#define TIMER2_START     	ADDR(0x4000a000)
#define TIMER2_STOP      	ADDR(0x4000a004)
#define TIMER2_COUNT     	ADDR(0x4000a008)
#define TIMER2_CLEAR     	ADDR(0x4000a00c)
#define TIMER2_SHUTDOWN  	ADDR(0x4000a010)
#define TIMER2_CAPTURE         ARRAY(0x4000a040)
#define TIMER2_COMPARE  	ARRAY(0x4000a140)
#define TIMER2_SHORTS    	ADDR(0x4000a200)
#define TIMER2_INTENSET  	ADDR(0x4000a304)
#define TIMER2_INTENCLR  	ADDR(0x4000a308)
#define TIMER2_MODE      	ADDR(0x4000a504)
#define TIMER2_BITMODE   	ADDR(0x4000a508)
#define TIMER2_PRESCALER 	ADDR(0x4000a510)
#define TIMER2_CC              ARRAY(0x4000a540)


/* Random Number Generator */
// Tasks
#define RNG_START      		ADDR(0x4000D000)
#define RNG_STOP       		ADDR(0x4000D004)
// Events
#define RNG_VALRDY     		ADDR(0x4000D100)
// Registers
#define RNG_SHORTS     		ADDR(0x4000D200)
#define RNG_INTEN      		ADDR(0x4000D300)
#define RNG_INTENSET   		ADDR(0x4000D304)
#define RNG_INTENCLR   		ADDR(0x4000D308)
#define RNG_CONFIG     		ADDR(0x4000D504)
#define   RNG_CONFIG_DERCEN 0
#define RNG_VALUE      		ADDR(0x4000D508)
// Interrupts
#define RNG_INT_VALRDY 0

/* Temperature sensor */
// Tasks
#define TEMP_START     		ADDR(0x4000C000)
#define TEMP_STOP      		ADDR(0x4000C004)
// Events
#define TEMP_DATARDY   		ADDR(0x4000C100)
// Registers
#define TEMP_INTEN     		ADDR(0x4000C300)
#define TEMP_INTENSET  		ADDR(0x4000C304)
#define TEMP_INTENCLR  		ADDR(0x4000C308)
#define TEMP_TEMP      		ADDR(0x4000C508)
// Interrupts
#define TEMP_INT_DATARDY 0

/* I2C */
_DEVUNION(_i2c, 0x1000);
#define I2C ((union _i2c *) 0x40003000)

// Tasks
#define I_STARTRX    		 REG(0x000)
#define I_STARTTX    		 REG(0x008)
#define I_STOP       		 REG(0x014)
#define I_SUSPEND    		 REG(0x01c)
#define I_RESUME     		 REG(0x020)
// Events
#define I_STOPPED    		 REG(0x104)
#define I_RXDREADY   		 REG(0x108)
#define I_TXDSENT    		 REG(0x11c)
#define I_ERROR      		 REG(0x124)
#define I_BB         		 REG(0x138)
#define I_SUSPENDED  		 REG(0x148)
// Registers
#define I_SHORTS     		 REG(0x200)
#define I_INTEN      		 REG(0x300)
#define I_INTENSET   		 REG(0x304)
#define I_INTENCLR   		 REG(0x308)
#define I_ERRORSRC   		 REG(0x4c4)
#define   I2C_ERRORSRC_OVERRUN 0
#define   I2C_ERRORSRC_ANACK 1
#define   I2C_ERRORSRC_DNACK 2
#define   I2C_ERRORSRC_All 0x7
#define I_ENABLE     		 REG(0x500) 
#define   I2C_ENABLE_Disabled 0
#define   I2C_ENABLE_Enabled 5
#define I_PSELSCL    		 REG(0x508)
#define I_PSELSDA    		 REG(0x50c)
#define I_RXD        		 REG(0x518)
#define I_TXD        		 REG(0x51c) 
#define I_FREQUENCY  		 REG(0x524)
#define   I2C_FREQUENCY_100kHz 0x01980000
#define I_ADDRESS    		 REG(0x588)
#define I_POWER      		 REG(0xffc)
// Interrupts
#define I2C_INT_STOPPED 1
#define I2C_INT_RXDREADY 2
#define I2C_INT_TXDSENT 7
#define I2C_INT_ERROR 9
#define I2C_INT_BB 14
// Shortcuts
#define I2C_BB_SUSPEND 0
#define I2C_BB_STOP 1

/* UART */
// Tasks
#define UART_STARTRX   		ADDR(0x40002000)
#define UART_STARTTX   		ADDR(0x40002008)
// Events
#define UART_RXDRDY    		ADDR(0x40002108)
#define UART_TXDRDY    		ADDR(0x4000211c)
// Registers
#define UART_INTENSET  		ADDR(0x40002304)
#define UART_INTENCLR  		ADDR(0x40002308)
#define   UART_INT_RXDRDY 2
#define   UART_INT_TXDRDY 7
#define UART_ENABLE    		ADDR(0x40002500)
#define   UART_ENABLE_Disabled 0
#define   UART_ENABLE_Enabled 4
#define UART_PSELTXD   		ADDR(0x4000250c)
#define UART_PSELRXD   		ADDR(0x40002514)
#define UART_RXD       		ADDR(0x40002518)
#define UART_TXD       		ADDR(0x4000251c)
#define UART_BAUDRATE  		ADDR(0x40002524)
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
#define UART_CONFIG    		ADDR(0x4000256c)
#define   UART_CONFIG_HWFC 0
#define   UART_CONFIG_PARITY 1, 3
#define     UART_PARITY_None 0
#define     UART_PARITY_Even 7

/* UARTE -- UART with EasyDMA */
#define UARTE0 (* (union _dev *) 0x40002000)
#define UARTE1 (* (union _dev *) 0x40028000)

// Tasks
#define U_STARTRX  REG(0x000)
#define U_STOPRX   REG(0x004)
#define U_STARTTX  REG(0x008)
#define U_STOPTX   REG(0x00c)
#define U_FLUSHRX  REG(0x02c)

//Events
#define U_CTS      REG(0x100)
#define U_NTCS     REG(0x104)
#define U_RXDRDY   REG(0x108)
#define U_ENDRX    REG(0x110)
#define U_TXDRDY   REG(0x11c)
#define U_ENDTX    REG(0x120)
#define U_ERROR    REG(0x124)
#define U_RXTO     REG(0x144)
#define U_RXSTARTED REG(0x14c)
#define U_TXSTARTED REG(0x150)
#define U_TXSTOPPED REG(0x158)

// Shorts
#define U_SHORTS   REG(0x200)
#define   UARTE_ENDRX_STARTRX 5
#define   UARTE_ENDRX_STOPRX 6

// Registers
#define U_INTEN    REG(0x300)
#define U_INTENSET REG(0x304)
#define U_INTENCLR REG(0x308)
#define   UARTE_INT_CTS 0
#define   UARTE_INT_NCTS 1
#define   UARTE_INT_RXDRDY 2
#define   UARTE_INT_ENDRX 4
#define   UARTE_INT_TXDRDY 7
#define   UARTE_INT_ENDTX 8
#define   UARTE_INT_ERROR 9
#define   UARTE_INT_RXTO 17
#define   UARTE_INT_RXSTARTED 19
#define   UARTE_INT_TXSTARTED 20
#define   UARTE_INT_TXSTOPPED 22
#define U_ERRORSRC REG(0x480)
#define   UARTE_ERROR_OVERRUN 0
#define   UARTE_ERROR_PARITY 1
#define   UARTE_ERROR_FRAMING 2
#define   UARTE_ERROR_BREAK 3
#define U_ENABLE   REG(0x500)
#define   UARTE_ENABLE_Disabled 0
#define   UARTE_ENABLE_Enabled 8
#define U_PSELRTS  REG(0x508)
#define U_PSELTXD  REG(0x50c)
#define U_PSELCTS  REG(0x510)
#define U_PSELRXD  REG(0x514)
#define U_BAUDRATE REG(0x524)
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
#define U_RXDPTR    PTR(0x534)
#define U_RXDMAXCNT REG(0x538)
#define U_RXDAMOUNT REG(0x53c)
#define U_TXDPTR    PTR(0x544)
#define U_TXDMAXCNT REG(0x548)
#define U_TXDAMOUNT REG(0x54c)
#define U_CONFG     REG(0x56c)
#define   UARTE_CONFIG_HWFC 0
#define   UARTE_CONFIG_PARITY 1, 3
#define     UARTE_PARITY_Disabled 0
#define     UARTE_PARITY_Enabled 7
#define   UARTE_CONFIG_STOP 4, 1
#define     UARTE_STOP_1 0
#define     UARTE_STOP_2 1
#define   UARTE_CONFIG_PARITYTYPE 8
#define     UARTE_PARITYTYPE_Even 0
#define     UARTE_PARITYTYPE_Odd 1


/* SAADC */
struct __adc_chan {
    unsigned PSELP;
    unsigned PSELN;
    unsigned CONFIG;
    unsigned LIMIT;
};

// Tasks
#define ADC_START      		ADDR(0x40007000)
#define ADC_SAMPLE     		ADDR(0x40007004)
#define ADC_STOP		ADDR(0x40007008)
#define ADC_CALIBRATE		ADDR(0x4000700c)
// Events
#define ADC_STARTED    		ADDR(0x40007100)
#define ADC_END			ADDR(0x40007104)
#define ADC_DONE		ADDR(0x40007108)
#define ADC_RESULTDONE		ADDR(0x4000710c)
#define ADC_CALDONE		ADDR(0x40007110)
#define ADC_STOPPED		ADDR(0x40007114)
// Registers
#define ADC_INTEN      		ADDR(0x40007300)
#define ADC_INTENSET   		ADDR(0x40007304)
#define ADC_INTENCLR   		ADDR(0x40007308)
#define ADC_BUSY      		ADDR(0x40007400)
#define ADC_ENABLE     		ADDR(0x40007500)
#define ADC_CHAN ((struct __adc_chan *) 0x40007510)
#define   ADC_CONFIG_RESP 0, 2
#define   ADC_CONFIG_RESN 4, 2
#define     ADC_RES_Bypass 0
#define     ADC_RES_Pulldown 1
#define     ADC_RES_Pullup 2
#define     ADC_RES_VDD_1_2 3
#define   ADC_CONFIG_GAIN 8, 3
#define     ADC_GAIN_1_6 0
#define     ADC_GAIN_1_5 1
#define     ADC_GAIN_1_4 2
#define     ADC_GAIN_1_3 3
#define     ADC_GAIN_1_2 4
#define     ADC_GAIN_1 5
#define     ADC_GAIN_2 6
#define     ADC_GAIN_3 7
#define   ADC_CONFIG_REFSEL 12, 1
#define     ADC_REFSEL_Internal 0
#define     ADC_REFSEL_VDD_1_4 1
#define   ADC_CONFIG_TACQ 16, 3
#define     ADC_TACQ_3us 0
#define     ADC_TACQ_5us 1
#define     ADC_TACQ_10us 2
#define     ADC_TACQ_15us 3
#define     ADC_TACQ_20us 4
#define     ADC_TACQ_40us 5
#define  ADC_CONFIG_MODE 20, 1
#define     ADC_MODE_SE 0
#define     ADC_MODE_Diff 1
#define  ADC_CONFIG_BURST 24, 1
#define     ADC_BURST_Disabled 0
#define     ADC_BURST_Enabled 1
#define ADC_RESOLUTION		ADDR(0x400075f0)
#define   ADC_RESOLUTION_8bit 0
#define   ADC_RESOLUTION_10bit 1
#define   ADC_RESOLUTION_12bit 2
#define   ADC_RESOLUTION_14bit 3
#define ADC_OVERSAMPLE		ADDR(0x400075f4)
#define ADC_SAMPLERATE		ADDR(0x400075f8)
#define ADC_RESULT_PTR	     POINTER(0x4000762c)
#define ADC_RESULT_MAXCNT	ADDR(0x40007630)
#define ADC_RESULT_AMOUNT	ADDR(0x40007634)
// Interrupts
#define ADC_INT_STARTED 0
#define ADC_INT_END 1
#define ADC_INT_DONE 2
#define ADC_INT_RESULTDONE 3
#define ADC_INT_CALDONE 4
#define ADC_INT_STOPPED 5

    
/* PWM */
struct _pwm_seq {
    void *PTR;
    unsigned CNT;
    unsigned REFRESH;
    unsigned ENDDELAY;
    unsigned char filler[16];
};

union _pwm {                       
    unsigned volatile reg[1]; 
    unsigned volatile arr[1][1]; 
    struct _pwm_seq seq[1][1];
};
    
#define SEQ(i) seq[i>>5]

#define PWM0 (* (union _pwm *) 0x4001C000)
#define PWM1 (* (union _pwm *) 0x40021000)
#define PWM2 (* (union _pwm *) 0x40022000)
#define PWM3 (* (union _pwm *) 0x4002d000)

// Tasks
#define W_STOP			 REG(0x004)
#define W_SEQSTART		 ARR(0x008)
#define W_NEXTSTEP		 REG(0x010)
// Events
#define W_STOPPED		 REG(0x104)
#define W_SEQSTARTED		 ARR(0x108)
#define W_SEQEND		 ARR(0x110)
#define W_PWMPERIODEND		 REG(0x118)
#define W_LOOPSDONE		 REG(0x11c)
// Registers
#define W_SHORTS		 REG(0x200)
#define   PWM_SEQEND0_STOP 0
#define   PWM_SEQEND1_STOP 1
#define   PWM_LOOPSDONE_SEQSTART0 2
#define   PWM_LOOPSDONE_SEQSTART1 3
#define   PWM_LOOPSDONE_STOP 4
#define W_INTEN			 REG(0x300)
#define   PWM_INT_STOPPED 1
#define   PWM_INT_SEQSTARTED0 2
#define   PWM_INT_SEQSTARTED1 3
#define   PWM_INT_SEQEND0 4
#define   PWM_INT_SEQEND1 5
#define   PWM_INT_PWMPERIODEND 6
#define   PWM_INT_LOOPSDONE 7
#define W_INTENSET		 REG(0x304)
#define W_INTENCLR		 REG(0x308)
#define W_ENABLE		 REG(0x500)
#define W_MODE			 REG(0x504)
#define   PWM_MODE_Up 0
#define   PWM_MODE_UpAndDown 1
#define W_COUNTERTOP		 REG(0x508)
#define W_PRESCALER       	 REG(0x50c)
#define W_DECODER		 REG(0x510)
#define   PWM_DECODER_LOAD 0, 2
#define     PWM_LOAD_Common 0
#define     PWM_LOAD_Grouped 1
#define     PWM_LOAD_Individual 2
#define     PWM_LOAD_Waveform 3
#define   PWM_DECODER_MODE 8, 1
#define     PWM_MODE_RefreshCount 0
#define     PWM_MODE_NextStep 1
#define W_LOOP			 REG(0x514)
#define W_SEQ			 SEQ(0x520)
#define W_PSEL			 ARR(0x560)

#define PWM0_STOP 		PWM0.W_STOP
#define PWM0_SEQSTART 		PWM0.W_SEQSTART
#define PWM0_NEXTSTEP 		PWM0.W_NEXTSTEP
#define PWM0_STOPPED 		PWM0.W_STOPPED
#define PWM0_SEQSTARTED 	PWM0.W_SEQSTARTED
#define PWM0_SEQEND 		PWM0.W_SEQEND
#define PWM0_PWMPERIODEND 	PWM0.W_PWMPERIODEND
#define PWM0_LOOPSDONE 		PWM0.W_LOOPSDONE
#define PWM0_SHORTS 		PWM0.W_SHORTS
#define PWM0_INTEN 		PWM0.W_INTEN
#define PWM0_INTENSET 		PWM0.W_INTENSET
#define PWM0_INTENCLR 		PWM0.W_INTENCLR
#define PWM0_ENABLE 		PWM0.W_ENABLE
#define PWM0_MODE 		PWM0.W_MODE
#define PWM0_COUNTERTOP 	PWM0.W_COUNTERTOP
#define PWM0_PRESCALER 		PWM0.W_PRESCALER
#define PWM0_DECODER 		PWM0.W_DECODER
#define PWM0_LOOP 		PWM0.W_LOOP
#define PWM0_SEQ 		PWM0.W_SEQ
#define PWM0_PSEL 		PWM0.W_PSEL

#define PWM1_STOP 		PWM1.W_STOP
#define PWM1_SEQSTART 		PWM1.W_SEQSTART
#define PWM1_NEXTSTEP 		PWM1.W_NEXTSTEP
#define PWM1_STOPPED 		PWM1.W_STOPPED
#define PWM1_SEQSTARTED 	PWM1.W_SEQSTARTED
#define PWM1_SEQEND 		PWM1.W_SEQEND
#define PWM1_PWMPERIODEND 	PWM1.W_PWMPERIODEND
#define PWM1_LOOPSDONE 		PWM1.W_LOOPSDONE
#define PWM1_SHORTS 		PWM1.W_SHORTS
#define PWM1_INTEN 		PWM1.W_INTEN
#define PWM1_INTENSET 		PWM1.W_INTENSET
#define PWM1_INTENCLR 		PWM1.W_INTENCLR
#define PWM1_ENABLE 		PWM1.W_ENABLE
#define PWM1_MODE 		PWM1.W_MODE
#define PWM1_COUNTERTOP 	PWM1.W_COUNTERTOP
#define PWM1_PRESCALER 		PWM1.W_PRESCALER
#define PWM1_DECODER 		PWM1.W_DECODER
#define PWM1_LOOP 		PWM1.W_LOOP
#define PWM1_SEQ 		PWM1.W_SEQ
#define PWM1_PSEL 		PWM1.W_PSEL

// PWM sequence parameters
#define PWM_SEQ_COMPARE 0, 15
#define PWM_SEQ_POLARITY 15, 1
#define   PWM_POLARITY_RisingEdge 0
#define   PWM_POLARITY_FallingEdge 1

#define PWM_SAMPLE(c, p) \
    (FIELD(PWM_SEQ_COMPARE, c) | FIELD(PWM_SEQ_POLARITY, p))
#define RISING PWM_POLARITY_RisingEdge
#define FALLING PWM_POLARITY_FallingEdge


/* NVIC stuff */

/* irq_priority -- set priority of an IRQ from 0 (highest) to 255 */
void irq_priority(int irq, unsigned priority);

/* enable_irq -- enable interrupts from an IRQ */
#define enable_irq(irq)  NVIC_ISER[(irq)>>5] = BIT((irq)&0x1f)

/* disable_irq -- disable interrupts from a specific IRQ */
#define disable_irq(irq)  NVIC_ICER[(irq)>>5] = BIT((irq)&0x1f)

/* clear_pending -- clear pending interrupt from an IRQ */
#define clear_pending(irq)  NVIC_ICPR[(irq)>>5] = BIT((irq)&0x1f)

/* reschedule -- request PendSC interrupt */
#define reschedule()  SCB_ICSR = BIT(SCB_ICSR_PENDSVSET)

/* active_irq -- find active interrupt: -16 to 63 */
#define active_irq()  (GET_FIELD(SCB_ICSR, SCB_ICSR_VECTACTIVE) - 16)

/* delay_loop -- timed delay */
void delay_loop(unsigned usec);


/* GPIO convenience */

/* gpio_dir -- set GPIO direction */
inline void gpio_dir(unsigned pin, unsigned dir) {
    if (dir)
        GPIO[PORT(pin)].G_DIRSET = BIT(PIN(pin));
    else
        GPIO[PORT(pin)].G_DIRCLR = BIT(PIN(pin));
}

/* gpio_connect -- connect pin for input */
inline void gpio_connect(unsigned pin) {
    SET_FIELD(GPIO[PORT(pin)].G_PINCNF[PIN(pin)],
              GPIO_PINCNF_INPUT, GPIO_INPUT_Connect);
}

/* gpio_drive -- set GPIO drive strength */
inline void gpio_drive(unsigned pin, unsigned mode) {
    SET_FIELD(GPIO[PORT(pin)].G_PINCNF[PIN(pin)],
              GPIO_PINCNF_DRIVE, mode);
}

/* gpio_out -- set GPIO output bit */
inline void gpio_out(unsigned pin, unsigned value) {
    if (value)
        GPIO[PORT(pin)].G_OUTSET = BIT(PIN(pin));
    else
        GPIO[PORT(pin)].G_OUTCLR = BIT(PIN(pin));
}

/* gpio_in -- get GPIO input bit */
inline unsigned gpio_in(unsigned pin) {
    return GET_BIT(GPIO[PORT(pin)].G_IN, PIN(pin));
}


/* Image constants */

#define NIMG 10

typedef unsigned image[NIMG];

#define LED_MASK0 0xd1688800 // 1101 0001 0110 1000 1000 1000 0000 0000
#define LED_MASK1 0x00000020
#define LED_DOT0  0x50008800 // 0101 0000 0000 0000 1000 1000 0000 0000
#define LED_DOT1  0x00000020

#define __ROW(r, c1, c2, c3, c4, c5) \
    BIT(r) | (!c1<<28) | (!c2<<11) | (!c3<<31) | (!c5<<30), (!c4<<5)

#define IMAGE(x11, x12, x13, x14, x15, \
              x21, x22, x23, x24, x25, \
              x31, x32, x33, x34, x35, \
              x41, x42, x43, x44, x45, \
              x51, x52, x53, x54, x55) \
    { __ROW(ROW1, x11, x12, x13, x14, x15), \
      __ROW(ROW2, x21, x22, x23, x24, x25), \
      __ROW(ROW3, x31, x32, x33, x34, x35), \
      __ROW(ROW4, x41, x42, x43, x44, x45), \
      __ROW(ROW5, x51, x52, x53, x54, x55) }
    
#define led_init()  GPIO0_DIRSET = LED_MASK0, GPIO1_DIRSET = LED_MASK1
#define led_dot()   GPIO0_OUTSET = LED_DOT0, GPIO1_OUTSET = LED_DOT1
#define led_off()   GPIO0_OUTCLR = LED_MASK0, GPIO1_OUTCLR = LED_MASK1


/* CODERAM -- mark function for copying to RAM */
#define CODERAM  __attribute((noinline, section(".xram")))

/* A few assembler macros for single instructions. */
#define intr_disable()  asm volatile ("cpsid i")
#define intr_enable()   asm volatile ("cpsie i")
#define get_primask()   ({unsigned x; asm ("mrs %0, primask" : "=r"(x)); x;})
#define set_primask(x)  asm ("msr primask, %0" : : "r"(x))
#define nop()           asm volatile ("nop")
#define syscall(op)     asm volatile ("svc %0" : : "i"(op))

/* pause() -- disabled on V2 owing to long wakeup time */
#define pause()         /* asm volatile ("wfe") */
