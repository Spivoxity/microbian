// microbian/radio.c
// Copyright (c) 2020 J. M. Spivey

#include "microbian.h"
#include "hardware.h"
#include <string.h>

/* RADIO_TASK -- process id for device driver */
static int RADIO_TASK;

/* Operating modes */
#define DISABLED 0              /* Doing nothing */
#define READY 1                 /* Initialised for reception */
#define LISTENING 2             /* Waiting for a packet, DMA set up */

#define FREQ 7                  /* Frequency 2407 MHz */

/* We use a packet format that agrees with the standard micro:bit
runtime.  That means prefixing the packet with three bytes (version,
group, protocol) and counting these three in the length: the STATLEN
feature of the radio is not used. */

static struct {
    byte length;                /* Packet length, including 3-byte prefix */
    byte version;               /* Version: always 1 */
    byte group;                 /* Radio group */
    byte protocol;              /* Protocol identifier: always 1 */
    byte data[RADIO_PACKET];    /* Payload */
} packet_buffer;

/* group -- group id for radio messages */
static volatile int group = 0;

/* init_radio -- initialise radio hardware */
static void init_radio() {
    RADIO_TXPOWER = 0;          // Default transmit power
    RADIO_FREQUENCY = FREQ;     // Transmission frequency
    RADIO_MODE = RADIO_MODE_NRF_1Mbit; // 1Mbit/sec data rate
    RADIO_BASE0 = 0x75626974;   // That spells 'ubit'
    RADIO_TXADDRESS = 0;        // Use address 0 for transmit
    RADIO_RXADDRESSES = BIT(0); //   and also (just one) for receive.

    // Basic configuration
    RADIO_PCNF0 = FIELD(RADIO_PCNF0_LFLEN, 8); // One 8-bit length field
    RADIO_PCNF1 = BIT(RADIO_PCNF1_WHITEEN) // Whitening enabled
        | FIELD(RADIO_PCNF1_BALEN, 4) // Base address is 4 bytes
        | FIELD(RADIO_PCNF1_MAXLEN, RADIO_PACKET+3)
                                // Max packet length allowing for 3 byte prefix
        | FIELD(RADIO_PCNF1_ENDIAN, RADIO_ENDIAN_Little);
                                // Fields transmitted LSB first

    // CRC and whitening settings -- match micro_bit runtime
    RADIO_CRCCNF = 2;           // CRC is 2 bytes
    RADIO_CRCINIT = 0xffff;
    RADIO_CRCPOLY = 0x11021;
    RADIO_DATAWHITEIV = 0x18;
}

/* radio_await -- wait for expected interrupt */
static void radio_await(unsigned volatile *event) {
    receive(INTERRUPT, NULL);
    assert(*event);
    *event = 0;
    clear_pending(RADIO_IRQ);
    enable_irq(RADIO_IRQ);
}

/* radio_task -- device driver for radio */
static void radio_task(int dummy) {
    int mode = DISABLED;
    int listener = 0;
    int n;
    void *buffer = NULL;
    message m;

    init_radio();

    // Configure interrupts
    RADIO_INTENSET =
        BIT(RADIO_INT_READY) | BIT(RADIO_INT_END) | BIT(RADIO_INT_DISABLED);
    connect(RADIO_IRQ);
    enable_irq(RADIO_IRQ);

    // Set packet buffer
    RADIO_PACKETPTR = (unsigned) &packet_buffer;

    while (1) {
        receive(ANY, &m);
        switch (m.m_type) {
        case INTERRUPT:
            // A packet has been received
            if (!RADIO_END || mode != LISTENING)
                panic("unexpected radio interrrupt");
            RADIO_END = 0;
            clear_pending(RADIO_IRQ);
            enable_irq(RADIO_IRQ);

            if (RADIO_CRCSTATUS == 0 || packet_buffer.group != group) {
                // Ignore the packet and listen again
                RADIO_START = 1;
                break;
            }

            n = packet_buffer.length-3;
            memcpy(buffer, packet_buffer.data, n);

            m.m_i1 = n;
            send(listener, REPLY, &m);
            mode = READY;
            break;

        case RECEIVE:
            if (mode == LISTENING)
                panic("radio supports only one listener at a time");
            listener = m.m_sender;
            buffer = m.m_p1;

            if (mode == DISABLED) {
                RADIO_RXEN = 1;
                radio_await(&RADIO_READY);
            }

            RADIO_PREFIX0 = group;
            RADIO_START = 1;
            mode = LISTENING;
            break;

        case SEND:
            if (mode != DISABLED) {
                // The radio was set up for receiving: disable it
                RADIO_DISABLE = 1;
                radio_await(&RADIO_DISABLED);
            }

            // Assemble the packet
            n = m.m_i2;
            packet_buffer.length = n+3;
            packet_buffer.version = 1;
            packet_buffer.group = group;
            packet_buffer.protocol = 1;
            memcpy(packet_buffer.data, m.m_p1, n);

            // Enable for sending and transmit the packet
            RADIO_TXEN = 1;
            radio_await(&RADIO_READY);
            RADIO_PREFIX0 = group;
            RADIO_START = 1;
            radio_await(&RADIO_END);

            // Disable the transmitter -- otherwise it jams the airwaves
            RADIO_DISABLE = 1;
            radio_await(&RADIO_DISABLED);

            if (mode != LISTENING)
                mode = DISABLED;
            else {
                // Go back to listening
                RADIO_RXEN = 1;
                radio_await(&RADIO_READY);
                RADIO_START = 1;
            }

            send(m.m_sender, REPLY, NULL);
            break;

        default:
            badmesg(m.m_type);
        }
    }
}

/* radio_group -- set group id for radio messages */
void radio_group(int grp) {
    group = grp;
}

/* radio_send -- send radio packet */
void radio_send(void *buf, int n) {
    message m;
    m.m_p1 = buf;
    m.m_i2 = n;
    sendrec(RADIO_TASK, SEND, &m);
}

/* radio_receive -- receive radio packet and return length */
int radio_receive(void *buf) {
    // buf must have space for RADIO_PACKET bytes
    message m;
    m.m_p1 = buf;
    sendrec(RADIO_TASK, RECEIVE, &m);
    return m.m_i1;
}
    
/* radio_init -- start device driver */
void radio_init(void) {
    RADIO_TASK = start("Radio", radio_task, 0, 256);
}
