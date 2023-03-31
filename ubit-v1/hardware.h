// common/hardware.h
// Copyright (c) 2018-20 J. M. Spivey

#define UBIT 1
#define UBIT_V1 1

/* Hardware register definitions for nRF51822 */

#define BIT(i) (1 << (i))
#define GET_BIT(reg, n) (((reg) >> (n)) & 0x1)
#define SET_BIT(reg, n) reg |= BIT(n)
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
#define ARRAY(x) ((unsigned volatile *) (x))

#pragma GCC diagnostic ignored "-Warray-bounds"
#define REG(i) reg[i>>2]
#define ARR(i) arr[i>>2]

    
/* Device pins */
#define PAD19 0
#define  I2C_SCL PAD19
#define PAD2 1
#define PAD1 2
#define PAD0 3
/* LED columns are GPIO 4-12 */
#define PAD3 4
#define PAD4 5
#define PAD10 6
#define PAD9 10
#define PAD7 11
#define PAD6 12
/* LED rows are GPIO 13-15 */
#define PAD16 16
#define PAD5 17
#define  BUTTON_A PAD5
#define PAD8 18
#define PAD12 20
#define PAD15 21
#define   SPI_MOSI PAD15
#define PAD14 22
#define  SPI_MISO PAD14
#define PAD13 23
#define  SPI_SCK PAD13
#define USB_TX 24
#define USB_RX 25
#define PAD11 26
#define  BUTTON_B PAD11
#define PAD20 30
#define  I2C_SDA PAD20

// Only one I2C interface
#define I2C_INTERNAL 0
#define I2C_EXTERNAL 0
#define N_I2CS 1


/* Interrupts */
#define SVC_IRQ    -5
#define PENDSV_IRQ -2
#define RADIO_IRQ   1
#define UART_IRQ    2
#define I2C_IRQ     3
#define SPI_IRQ     4
#define GPIOTE_IRQ  6
#define ADC_IRQ     7
#define TIMER0_IRQ  8
#define TIMER1_IRQ  9
#define TIMER2_IRQ 10
#define RTC0_IRQ   11
#define TEMP_IRQ   12
#define RNG_IRQ    13
#define RTC1_IRQ   17

#define N_INTERRUPTS 32

/* System registers */
#define SCB_CPUID               ADDR(0xe000ed00)
#define SCB_ICSR                ADDR(0xe000ed04)
#define   SCB_ICSR_PENDSVSET 28
#define   SCB_ICSR_VECTACTIVE 0, 8
#define SCB_SCR                 ADDR(0xe000ed10)
#define   SCB_SCR_SLEEPONEXIT 1
#define   SCB_SCR_SLEEPDEEP 2
#define   SCB_SCR_SEVONPEND 4
#define SCB_SHPR               ARRAY(0xe000ed1c)

#define NVIC_ISER              ARRAY(0xe000e100)
#define NVIC_ICER              ARRAY(0xe000e180)
#define NVIC_ISPR              ARRAY(0xe000e200)
#define NVIC_ICPR              ARRAY(0xe000e280)
#define NVIC_IPR               ARRAY(0xe000e400)

#define POWER_RAMON             ADDR(0x40000524)


/* Clock control */
#define CLOCK_HFCLKSTART        ADDR(0x40000000)
#define CLOCK_LFCLKSTART        ADDR(0x40000008)
#define CLOCK_HFCLKSTARTED      ADDR(0x40000100)
#define CLOCK_LFCLKSTARTED      ADDR(0x40000104)
#define CLOCK_LFCLKSRC          ADDR(0x40000518)
#define CLOCK_XTALFREQ          ADDR(0x40000550)
     
#define CLOCK_LFCLKSRC_RC 0
#define CLOCK_XTALFREQ_16MHz 0xFF

#define MPU_DISABLEINDEBUG ADDR(0x40000608)


/* Factory information */
#define FICR_DEVICEID   ARRAY(0x10000060)
#define FICR_DEVICEADDR ARRAY(0x100000a4)
#define FICR_OVERRIDEEN  ADDR(0x100000a0)
#define FICR_OVERRIDEEN_NRF 0
#define FICR_NRF_1MBIT  ARRAY(0x100000b0)


/* Non-Volatile Memory Controller */
#define NVMC_READY               ADDR(0x4001e400)
#define NVMC_CONFIG              ADDR(0x4001e504)
#define   NVMC_CONFIG_WEN 0
#define   NVMC_CONFIG_EEN 1   
#define NVMC_ERASEPAGE           (* (void * volatile *) 0x4001e508)


/* GPIO */
// Registers
#define GPIO_OUT       		 ADDR(0x50000504)
#define GPIO_OUTSET    		 ADDR(0x50000508)
#define GPIO_OUTCLR    		 ADDR(0x5000050c)
#define GPIO_IN        		 ADDR(0x50000510)
#define GPIO_DIR       		 ADDR(0x50000514)
#define GPIO_DIRSET    		 ADDR(0x50000518)
#define GPIO_DIRCLR    		 ADDR(0x5000051c)
#define GPIO_PINCNF   		ARRAY(0x50000700)
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


/* GPIOTE */
// Tasks
#define GPIOTE_OUT    		ARRAY(0x40006000)
// Events
#define GPIOTE_IN     		ARRAY(0x40006100)
#define GPIOTE_PORT    		 ADDR(0x4000617c)
// Registers
#define GPIOTE_INTEN   		 ADDR(0x40006300)
#define GPIOTE_INTENSET 	 ADDR(0x40006304)
#define GPIOTE_INTENCLR 	 ADDR(0x40006308)
#define GPIOTE_CONFIG 		ARRAY(0x40006510)
#define   GPIOTE_CONFIG_MODE 0, 2
#define     GPIOTE_MODE_Event 1
#define     GPIOTE_MODE_Task 3
#define   GPIOTE_CONFIG_PSEL 8, 5
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

#define PPI_CHG  ((struct _ppi_chg *) 0x4001f000)
#define PPI_CHEN       		 ADDR(0x4001f500)
#define PPI_CHENSET    		 ADDR(0x4001f504)
#define PPI_CHENCLR    		 ADDR(0x4001f508)
#define PPI_CH    ((struct _ppi_ch *) 0x4001f510)
#define PPI_CHGRP 		ARRAY(0x4001f800)


/* Radio */
// Tasks
#define RADIO_TXEN     		 ADDR(0x40001000)
#define RADIO_RXEN     		 ADDR(0x40001004)
#define RADIO_START    		 ADDR(0x40001008)
#define RADIO_STOP     		 ADDR(0x4000100c)
#define RADIO_DISABLE  		 ADDR(0x40001010)
#define RADIO_RSSISTART		 ADDR(0x40001014)
#define RADIO_RSSISTOP 		 ADDR(0x40001018)
#define RADIO_BCSTART  		 ADDR(0x4000101c)
#define RADIO_BCSTOP   		 ADDR(0x40001020)
// Events
#define RADIO_READY    		 ADDR(0x40001100)
#define RADIO_ADDRESS  		 ADDR(0x40001104)
#define RADIO_PAYLOAD  		 ADDR(0x40001108)
#define RADIO_END      		 ADDR(0x4000110c)
#define RADIO_DISABLED 		 ADDR(0x40001110)
#define RADIO_DEVMATCH 		 ADDR(0x40001114)
#define RADIO_DEVMISS  		 ADDR(0x40001118)
#define RADIO_RSSIEND  		 ADDR(0x4000111c)
#define RADIO_BCMATCH  		 ADDR(0x40001128)
// Registers
#define RADIO_SHORTS   		 ADDR(0x40001200)
#define   RADIO_READY_START 0
#define   RADIO_END_DISABLE 1
#define RADIO_INTENSET 		 ADDR(0x40001304)
#define RADIO_INTENCLR 		 ADDR(0x40001308)
#define RADIO_CRCSTATUS 	 ADDR(0x40001400)
#define RADIO_RXMATCH  		 ADDR(0x40001408)
#define RADIO_RXCRC    		 ADDR(0x4000140c)
#define RADIO_DAI      		 ADDR(0x40001410)
#define RADIO_PACKETPTR 	 ADDR(0x40001504)
#define RADIO_FREQUENCY 	 ADDR(0x40001508)
#define RADIO_TXPOWER  		 ADDR(0x4000150c)
#define RADIO_MODE     		 ADDR(0x40001510)
#define   RADIO_MODE_NRF_1Mbit 0
#define RADIO_PCNF0    		 ADDR(0x40001514)
#define   RADIO_PCNF0_LFLEN 0, 4
#define   RADIO_PCNF0_S0LEN 8, 1
#define   RADIO_PCNF0_S1LEN 16, 4
#define RADIO_PCNF1    		 ADDR(0x40001518)
#define   RADIO_PCNF1_MAXLEN 0, 8
#define   RADIO_PCNF1_STATLEN 8, 8
#define   RADIO_PCNF1_BALEN 16, 3
#define   RADIO_PCNF1_ENDIAN 24, 1
#define     RADIO_ENDIAN_Little 0
#define     RADIO_ENDIAN_Big 1
#define   RADIO_PCNF1_WHITEEN 25
#define RADIO_BASE0    		 ADDR(0x4000151c)
#define RADIO_BASE1    		 ADDR(0x40001520)
#define RADIO_PREFIX0  		 ADDR(0x40001524)
#define RADIO_PREFIX1  		 ADDR(0x40001528)
#define RADIO_TXADDRESS 	 ADDR(0x4000152c)
#define RADIO_RXADDRESSES	 ADDR(0x40001530)
#define RADIO_CRCCNF   		 ADDR(0x40001534)
#define RADIO_CRCPOLY  		 ADDR(0x40001538)
#define RADIO_CRCINIT  		 ADDR(0x4000153c)
#define RADIO_TEST     		 ADDR(0x40001540)
#define RADIO_TIFS     		 ADDR(0x40001544)
#define RADIO_RSSISAMPLE 	 ADDR(0x40001548)
#define RADIO_STATE    		 ADDR(0x40001550)
#define RADIO_DATAWHITEIV 	 ADDR(0x40001554)
#define RADIO_BCC      		 ADDR(0x40001560)
#define RADIO_DAB     		ARRAY(0x40001600)
#define RADIO_DAP     		ARRAY(0x40001620)
#define RADIO_DACNF    		 ADDR(0x40001640)
#define RADIO_OVERRIDE 		ARRAY(0x40001724)
#define RADIO_POWER    		 ADDR(0x40001ffc)
// Interrupts
#define RADIO_INT_READY 0
#define RADIO_INT_END 3
#define RADIO_INT_DISABLED 4


/* TIMERS: Timer 0 is 8/16/24/32 bit, Timers 1 and 2 are 8/16 bit. */
union _timer {
    unsigned volatile reg[1];
    unsigned volatile arr[1][1];
    unsigned char strut[0x1000];
};

#define TIMER ((union _timer *) 0x40008000)

// Tasks
#define T_START     	         REG(0x000)
#define T_STOP		      	 REG(0x004)
#define T_COUNT     	         REG(0x008)
#define T_CLEAR     	         REG(0x00c)
#define T_SHUTDOWN  	         REG(0x010)
#define T_CAPTURE  	         ARR(0x040)
// Events
#define T_COMPARE  	         ARR(0x140)
// Registers
#define T_SHORTS    	         REG(0x200)
#define   TIMER_COMPARE0_CLEAR 0
#define   TIMER_COMPARE1_CLEAR 1
#define   TIMER_COMPARE2_CLEAR 2
#define   TIMER_COMPARE3_CLEAR 3
#define   TIMER_COMPARE0_STOP 8
#define   TIMER_COMPARE1_STOP 9
#define   TIMER_COMPARE2_STOP 10
#define   TIMER_COMPARE3_STOP 11
#define T_INTENSET  	         REG(0x304)
#define T_INTENCLR  	 	 REG(0x308)
#define   TIMER_INT_COMPARE0 16
#define   TIMER_INT_COMPARE1 17
#define   TIMER_INT_COMPARE2 18
#define   TIMER_INT_COMPARE3 19
#define T_MODE      	 	 REG(0x504)
#define   TIMER_MODE_Timer 0
#define   TIMER_MODE_Counter 1
#define T_BITMODE   	 	 REG(0x508)
#define   TIMER_BITMODE_16Bit 0
#define   TIMER_BITMODE_8Bit 1
#define   TIMER_BITMODE_24Bit 2
#define   TIMER_BITMODE_32Bit 3
#define T_PRESCALER 	 	 REG(0x510)
#define T_CC       		 ARR(0x540)

// Timer 0
#define TIMER0_START	  TIMER[0].T_START
#define TIMER0_STOP	  TIMER[0].T_STOP
#define TIMER0_COUNT	  TIMER[0].T_COUNT
#define TIMER0_CLEAR	  TIMER[0].T_CLEAR
#define TIMER0_SHUTDOWN	  TIMER[0].T_SHUTDOWN
#define TIMER0_CAPTURE	  TIMER[0].T_CAPTURE
#define TIMER0_COMPARE	  TIMER[0].T_COMPARE
#define TIMER0_SHORTS	  TIMER[0].T_SHORTS
#define TIMER0_INTENSET	  TIMER[0].T_INTENSET
#define TIMER0_INTENCLR	  TIMER[0].T_INTENCLR
#define TIMER0_MODE	  TIMER[0].T_MODE
#define TIMER0_BITMODE	  TIMER[0].T_BITMODE
#define TIMER0_PRESCALER  TIMER[0].T_PRESCALER
#define TIMER0_CC	  TIMER[0].T_CC

// Timer 1
#define TIMER1_START	  TIMER[1].T_START
#define TIMER1_STOP	  TIMER[1].T_STOP
#define TIMER1_COUNT	  TIMER[1].T_COUNT
#define TIMER1_CLEAR	  TIMER[1].T_CLEAR
#define TIMER1_SHUTDOWN	  TIMER[1].T_SHUTDOWN
#define TIMER1_CAPTURE	  TIMER[1].T_CAPTURE
#define TIMER1_COMPARE	  TIMER[1].T_COMPARE
#define TIMER1_SHORTS	  TIMER[1].T_SHORTS
#define TIMER1_INTENSET	  TIMER[1].T_INTENSET
#define TIMER1_INTENCLR	  TIMER[1].T_INTENCLR
#define TIMER1_MODE	  TIMER[1].T_MODE
#define TIMER1_BITMODE	  TIMER[1].T_BITMODE
#define TIMER1_PRESCALER  TIMER[1].T_PRESCALER
#define TIMER1_CC	  TIMER[1].T_CC

// Timer 2
#define TIMER2_START	  TIMER[2].T_START
#define TIMER2_STOP	  TIMER[2].T_STOP
#define TIMER2_COUNT	  TIMER[2].T_COUNT
#define TIMER2_CLEAR	  TIMER[2].T_CLEAR
#define TIMER2_SHUTDOWN	  TIMER[2].T_SHUTDOWN
#define TIMER2_CAPTURE	  TIMER[2].T_CAPTURE
#define TIMER2_COMPARE	  TIMER[2].T_COMPARE
#define TIMER2_SHORTS	  TIMER[2].T_SHORTS
#define TIMER2_INTENSET	  TIMER[2].T_INTENSET
#define TIMER2_INTENCLR	  TIMER[2].T_INTENCLR
#define TIMER2_MODE	  TIMER[2].T_MODE
#define TIMER2_BITMODE	  TIMER[2].T_BITMODE
#define TIMER2_PRESCALER  TIMER[2].T_PRESCALER
#define TIMER2_CC	  TIMER[2].T_CC


/* Random Number Generator */
// Tasks
#define RNG_START      		 ADDR(0x4000D000)
#define RNG_STOP       		 ADDR(0x4000D004)
// Events
#define RNG_VALRDY     		 ADDR(0x4000D100)
// Registers
#define RNG_SHORTS     		 ADDR(0x4000D200)
#define RNG_INTEN      		 ADDR(0x4000D300)
#define RNG_INTENSET   		 ADDR(0x4000D304)
#define RNG_INTENCLR   		 ADDR(0x4000D308)
#define RNG_CONFIG     		 ADDR(0x4000D504)
#define   RNG_CONFIG_DERCEN 0
#define RNG_VALUE      		 ADDR(0x4000D508)
// Interrupts
#define RNG_INT_VALRDY 0


/* Temperature sensor */
// Tasks
#define TEMP_START     		 ADDR(0x4000C000)
#define TEMP_STOP      		 ADDR(0x4000C004)
// Events
#define TEMP_DATARDY   		 ADDR(0x4000C100)
// Registers
#define TEMP_INTEN     		 ADDR(0x4000C300)
#define TEMP_INTENSET  		 ADDR(0x4000C304)
#define TEMP_INTENCLR  		 ADDR(0x4000C308)
#define TEMP_TEMP      		 ADDR(0x4000C508)
// Interrupts
#define TEMP_INT_DATARDY 0


/* I2C -- Interface 0 */
union _i2c {
    unsigned volatile reg[1];
    unsigned char strut[0x1000];
};

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
#define UART_STARTRX   		 ADDR(0x40002000)
#define UART_STARTTX   		 ADDR(0x40002008)
// Events
#define UART_RXDRDY    		 ADDR(0x40002108)
#define UART_TXDRDY    		 ADDR(0x4000211c)
// Registers
#define UART_INTEN     		 ADDR(0x40002300)
#define UART_INTENSET  		 ADDR(0x40002304)
#define UART_INTENCLR  		 ADDR(0x40002308)
#define UART_ENABLE    		 ADDR(0x40002500)
#define   UART_ENABLE_Disabled 0
#define   UART_ENABLE_Enabled 4
#define UART_PSELTXD   		 ADDR(0x4000250c)
#define UART_PSELRXD   		 ADDR(0x40002514)
#define UART_RXD       		 ADDR(0x40002518)
#define UART_TXD       		 ADDR(0x4000251c)
#define UART_BAUDRATE  		 ADDR(0x40002524)
#define   UART_BAUDRATE_1200   0x0004f000
#define   UART_BAUDRATE_2400   0x0009d000
#define   UART_BAUDRATE_4800   0x0013b000
#define   UART_BAUDRATE_9600   0x00275000
#define   UART_BAUDRATE_14400  0x003b0000
#define   UART_BAUDRATE_19200  0x004ea000
#define   UART_BAUDRATE_28800  0x0075f000
#define   UART_BAUDRATE_31250  0x00800000
#define   UART_BAUDRATE_38400  0x009d5000
#define   UART_BAUDRATE_56000  0x00e50000
#define   UART_BAUDRATE_57600  0x00ebf000
#define   UART_BAUDRATE_76800  0x013a9000
#define   UART_BAUDRATE_115200 0x01d7e000
#define   UART_BAUDRATE_230400 0x03afb000
#define   UART_BAUDRATE_250000 0x04000000
#define   UART_BAUDRATE_460800 0x075f7000
#define   UART_BAUDRATE_921600 0x0ebed000
#define   UART_BAUDRATE_1M     0x10000000
#define UART_CONFIG    		 ADDR(0x4000256c)
#define   UART_CONFIG_HWFC 0
#define   UART_CONFIG_PARITY 1, 3
#define     UART_PARITY_None 0
#define     UART_PARITY_Even 7
// Interrupts
#define UART_INT_RXDRDY 2
#define UART_INT_TXDRDY 7


/* ADC */
// Tasks
#define ADC_START      		 ADDR(0x40007000)
#define ADC_STOP       		 ADDR(0x40007004)
// Events
#define ADC_END        		 ADDR(0x40007100)
// Registers
#define ADC_INTEN      		 ADDR(0x40007300)
#define ADC_INTENSET   		 ADDR(0x40007304)
#define ADC_INTENCLR   		 ADDR(0x40007308)
#define ADC_BUSY       		 ADDR(0x40007400)
#define ADC_ENABLE     		 ADDR(0x40007500)
#define ADC_CONFIG     		 ADDR(0x40007504)
#define   ADC_CONFIG_RES 0, 2
#define     ADC_RES_8Bit 0
#define     ADC_RES_9bit 1
#define     ADC_RES_10bit 2
#define   ADC_CONFIG_INPSEL 2, 3
#define     ADC_INPSEL_AIn_1_1 0
#define     ADC_INPSEL_AIn_2_3 1
#define     ADC_INPSEL_AIn_1_3 2
#define     ADC_INPSEL_Vdd_2_3 5
#define     ADC_INPSEL_Vdd_1_3 6
#define   ADC_CONFIG_REFSEL 5, 2
#define     ADC_REFSEL_BGap 0
#define     ADC_REFSEL_Ext 1
#define     ADC_REFSEL_Vdd_1_2 2
#define     ADC_REFSEL_Vdd_1_3 3
#define   ADC_CONFIG_PSEL 8, 8
#define   ADC_CONFIG_EXTREFSEL 16, 2
#define     ADC_EXTREFSEL_Ref0 1
#define     ADC_EXTREFSEL_Ref1 2
#define ADC_RESULT     		 ADDR(0x40007508)
// Interrupts
#define ADC_INT_END 0

    
/* NVIC stuff */

/* irq_priority -- set priority of an IRQ from 0 (highest) to 255 */
void irq_priority(int irq, unsigned priority);

/* enable_irq -- enable interrupts from an IRQ */
#define enable_irq(irq)  NVIC_ISER[0] = BIT(irq)

/* disable_irq -- disable interrupts from a specific IRQ */
#define disable_irq(irq)  NVIC_ICER[0] = BIT(irq)

/* clear_pending -- clear pending interrupt from an IRQ */
#define clear_pending(irq)  NVIC_ICPR[0] = BIT(irq)

/* reschedule -- request PendSC interrupt */
#define reschedule()  SCB_ICSR = BIT(SCB_ICSR_PENDSVSET)

/* active_irq -- find active interrupt: -16 to 31 */
#define active_irq()  (GET_FIELD(SCB_ICSR, SCB_ICSR_VECTACTIVE) - 16)

/* delay_loop -- timed delay */
void delay_loop(unsigned usec);


/* GPIO convenience */

/* gpio_dir -- set GPIO direction */
inline void gpio_dir(unsigned pin, unsigned dir) {
    if (dir)
        GPIO_DIRSET = BIT(pin);
    else
        GPIO_DIRCLR = BIT(pin);
}

/* gpio_connect -- connect pin for input */
inline void gpio_connect(unsigned pin) {
    SET_FIELD(GPIO_PINCNF[pin], GPIO_PINCNF_INPUT, GPIO_INPUT_Connect);
}

/* gpio_drive -- set GPIO drive strength */
inline void gpio_drive(unsigned pin, unsigned mode) {
    SET_FIELD(GPIO_PINCNF[pin], GPIO_PINCNF_DRIVE, mode);
}

/* gpio_out -- set GPIO output value */
inline void gpio_out(unsigned pin, unsigned value) {
    if (value)
        GPIO_OUTSET = BIT(pin);
    else
        GPIO_OUTCLR = BIT(pin);
}

/* gpio_in -- get GPIO input bit */
inline unsigned gpio_in(unsigned pin) {
    return GET_BIT(GPIO_IN, pin);
}


/* Image constants */

#define NIMG 3

typedef unsigned image[NIMG];

#define __ROW(r, c1, c2, c3, c4, c5, c6, c7, c8, c9)                   \
    (BIT(r+13) | !c1<<4 | !c2<<5 | !c3<<6 | !c4<<7 | !c5<<8            \
     | !c6<<9 | !c7<<10 | !c8<<11 | !c9<<12)

#define IMAGE(x11, x24, x12, x25, x13,                               \
              x34, x35, x36, x37, x38,                               \
              x22, x19, x23, x39, x21,                               \
              x18, x17, x16, x15, x14,                               \
              x33, x27, x31, x26, x32)                               \
    { __ROW(0, x11, x12, x13, x14, x15, x16, x17, x18, x19),         \
      __ROW(1, x21, x22, x23, x24, x25, x26, x27, 0, 0),             \
      __ROW(2, x31, x32, x33, x34, x35, x36, x37, x38, x39) }

#define LED_MASK 0xfff0

#define led_init()  GPIO_DIRSET = LED_MASK
#define led_dot()   GPIO_OUTSET = 0x5fbf
#define led_off()   GPIO_OUTCLR = LED_MASK


/* CODERAM -- mark function for copying to RAM (disabled on V1) */
#define CODERAM

/* A few assembler macros for single instructions. */
#define pause()         asm volatile ("wfe")
#define intr_disable()  asm volatile ("cpsid i")
#define intr_enable()   asm volatile ("cpsie i")
#define get_primask()   ({unsigned x; asm ("mrs %0, primask" : "=r" (x)); x;})
#define set_primask(x)  asm ("msr primask, %0" : : "r" (x))
#define nop()           asm volatile ("nop")
#define syscall(op)    	asm volatile ("svc %0" : : "i"(op))
