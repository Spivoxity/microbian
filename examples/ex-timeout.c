/* ex-timeout.c */
/* Copyright (c) 2023 J. M. Spivey */

#include "hardware.h"
#include "microbian.h"
#include "lib.h"

int BUTTONS;

#define PRESS 10

#define N_BUTTONS 2
static const int but_pin[N_BUTTONS] = { BUTTON_A, BUTTON_B };

void button_task(int arg)
{
    int client = 0;
    unsigned history[N_BUTTONS];
    int state[N_BUTTONS];
    message m;

    for (int i = 0; i < N_BUTTONS; i++) {
        state[i] = history[i] = 0;
        gpio_connect(but_pin[i]);
    }

    timer_pulse(50);

    while (1) {
        receive(ANY, &m);
        switch (m.type) {
        case PING:
            for (int i = 0; i < N_BUTTONS; i++) {
                int x = gpio_in(but_pin[i]);
                history[i] = ((history[i] << 1) | x) & 0x7;
                if (state[i]) {
                    if (history[i] == 0x0) {
                        state[i] = 0;
                        if (client) send_int(client, PRESS, i);
                    }
                } else {
                    if (history[i] == 0x7)
                        state[i] = 1;
                }
            }
            break;

        case REGISTER:
            client = m.sender;
            break;

        default:
            badmesg(m.type);
        }
    }
}

void monitor_task(int arg)
{
    message m;

    send_msg(BUTTONS, REGISTER);
    printf("Hello\n");

    while (1) {
        receive_t(PRESS, &m, 2000);
        switch (m.type) {
        case PRESS:
            printf("Button %c\n", (!m.int1 ? 'A' : 'B'));
            break;

        case TIMEOUT:
            printf("Timeout\n");
            break;

        default:
            badmesg(m.type);
        }
    }
}

void init(void)
{
    serial_init();
    timer_init();
    BUTTONS = start("Buttons", button_task, 0, STACK);
    start("Monitor", monitor_task, 0, STACK);
}
