/* ex-valentine.c */
/* Copyright (c) 2020 J. M. Spivey */

#include "hardware.h"
#include "microbian.h"
#include "lib.h"

/* heart -- filled-in heart image */
const unsigned heart[] =
    IMAGE(0,1,0,1,0,
          1,1,1,1,1,
          1,1,1,1,1,
          0,1,1,1,0,
          0,0,1,0,0);

/* small -- small heart image */
const unsigned small[] =
    IMAGE(0,0,0,0,0,
          0,1,0,1,0,
          0,1,1,1,0,
          0,0,1,0,0,
          0,0,0,0,0);

/* show -- display a picture for a specified time */
void show(const unsigned *img, int t)
{
    display_show(img);
    timer_delay(t);
}    

/* heart_task -- show beating heart */
void heart_task(int n)
{
    priority(P_HIGH);

    while (1) {
        show(heart, 1050);
        show(small, 150);
        show(heart, 150);
        show(small, 150);
    }
}

/* prime -- test for primality */
int prime(int n)
{
    for (int k = 2; k * k <= n; k++) {
        if (n % k == 0)
            return 0;
    }

    return 1;
}

/* prime_task -- print primes on the serial port */
void prime_task(int arg)
{
    int n = 2, count = 0;

    while (1) {
        if (prime(n)) {
            count++;
            printf("prime(%d) = %d\n", count, n);
        }
        n++;
    }
}

void init(void)
{
    serial_init();
    timer_init();
    display_init();
    start("Heart", heart_task, 0, STACK);
    start("Prime", prime_task, 0, STACK);
}
