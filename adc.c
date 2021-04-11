// microbian/adc.c
// Copyright (c) 2021 J. M. Spivey

#include "microbian.h"
#include "hardware.h"

static int ADC;

static void adc_task(int dummy) {
    int client, chan;
    short result;
    message m;

#ifdef UBIT_V1
    // Initialise the ADC: 10 bit resolution,
    // compare 1/3 of the input with 1/3 of Vdd
    ADC_CONFIG = FIELD(ADC_CONFIG_RES, ADC_RES_10bit)
        | FIELD(ADC_CONFIG_INPSEL, ADC_INPSEL_AIn_1_3)
        | FIELD(ADC_CONFIG_REFSEL, ADC_REFSEL_Vdd_1_3);
    ADC_INTEN = BIT(ADC_INT_END);
#endif

#ifdef UBIT_V2
    // Initialise the SAADC: 10 bit resolution,
    // compare 1/4 if the input with 1/4 of Vdd with
    // acquisition window of 10 microsec.  (Yes, micro not pico.)
    ADC_CHAN[0].CONFIG = FIELD(ADC_CONFIG_GAIN, ADC_GAIN_1_4)
        | FIELD(ADC_CONFIG_REFSEL, ADC_REFSEL_VDD_1_4)
        | FIELD(ADC_CONFIG_TACQ, ADC_TACQ_10us);
    ADC_RESOLUTION = ADC_RESOLUTION_10bit;
    ADC_INTEN = BIT(ADC_INT_END) | BIT(ADC_INT_CALDONE);
#endif

    connect(ADC_IRQ);
    enable_irq(ADC_IRQ);
    
#ifdef UBIT_V2
    // Run a calibration cycle to set zero point
    ADC_ENABLE = 1;
    ADC_CALIBRATE = 1;
    receive(INTERRUPT, NULL);
    assert(ADC_CALDONE);
    ADC_CALDONE = 0;
    clear_pending(ADC_IRQ);
    enable_irq(ADC_IRQ);
    ADC_ENABLE = 0;
#endif

    while (1) {
        receive(ANY, &m);
        assert(m.m_type == REQUEST);
        client = m.m_sender;
        chan = m.m_i1;

#ifdef UBIT_V1
        SET_FIELD(ADC_CONFIG, ADC_CONFIG_PSEL, BIT(chan));
        ADC_ENABLE = 1;
        ADC_START = 1;
        receive(INTERRUPT, NULL);
        assert(ADC_END);
        result = ADC_RESULT;
        ADC_END = 0;
        ADC_ENABLE = 0;
#endif

#ifdef UBIT_V2
        ADC_CHAN[0].PSELP = chan+1;
        ADC_ENABLE = 1;
        ADC_RESULT_PTR = &result;
        ADC_RESULT_MAXCNT = 1;
        ADC_START = 1;
        ADC_SAMPLE = 1;
        receive(INTERRUPT, NULL);
        assert(ADC_END);
        assert(ADC_RESULT_AMOUNT == 1);
        ADC_END = 0;
        ADC_ENABLE = 0;
        
        // Result can still be slightly negative even after calibration
        if (result < 0) result = 0;
#endif        
        
        clear_pending(ADC_IRQ);
        enable_irq(ADC_IRQ);

        m.m_i1 = result;
        send(client, REPLY, &m);
    }
}

/* chantab -- translate pin numbers to ADC channels */
static const int chantab[] = {
#ifdef UBIT_V1
    PAD0, 4, PAD1, 3, PAD2, 2, PAD3, 5, PAD4, 6, PAD10, 7,
#endif
#ifdef UBIT_V2
    PAD0, 0, PAD1, 1, PAD2, 2, PAD3, 7, PAD4, 4, PAD10, 6,
#endif
    0
};

int adc_reading(int pin) {
    int i, chan = -1;
    message m;

    for (i = 0; chantab[i] != 0; i += 2) {
        if (chantab[i] == pin) {
            chan = chantab[i+1];
            break;
        }
    }

    if (chan < 0)
        panic("Can't use pin %d for ADC", pin);

    m.m_i1 = chan;
    sendrec(ADC, REQUEST, &m);
    return m.m_i1;
}

void adc_init(void) {
    ADC = start("ADC", adc_task, 0, 256);
}
