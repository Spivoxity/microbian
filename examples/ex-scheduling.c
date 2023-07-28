//
// Created by joshua on 28/07/23.
//
#include "microbian.h"
#include "hardware.h"
#include "lib.h"

// Function to get a random unsigned integer between min and max (inclusive)
unsigned int random_unsigned_int(unsigned int min, unsigned int max) {
    unsigned int random_value = 0;
    unsigned int range = max - min + 1;
    unsigned int num_bits = 0;

    // Calculate the number of bits needed to represent the range
    while ((1 << num_bits) < range) {
        num_bits++;
    }

    // Generate random unsigned integers until we get one within the range
    do {
        random_value = 0;
        for (unsigned int i = 0; i < num_bits; i++) {
            random_value = (random_value << 8) | rng_get();
        }
        random_value = random_value % range;
    } while (random_value > range);

    return random_value + min;
}

static volatile int r = 0;

void proc1(int n) {
    for (int i = 0; i < n; i++)
        printf("r = %d\n", r);
}

void proc2(int n) {
    while (r < 100000)
        r++;

    for (int i = 0; i < n; i++) {
        nop();
        nop();
        nop();
    }

    dump();
}

void init(void) {
    serial_init();

    // Initialize 'n' processes randomly between proc1 and proc2 with aptly chosen random numbers
    unsigned int n = 10;
    unsigned int proc1_iterations = random_unsigned_int(5, 20); // Number of iterations for proc1 will be between 5 and 20
    unsigned int proc2_iterations = random_unsigned_int(100000, 2000000); // Number of iterations for proc2 will be between 100000 and 2000000

    // Start 'n' processes randomly between proc1 and proc2
    for (unsigned int i = 0; i < n; i++) {
        if (random_unsigned_int(0, 1) == 0) {
            start("Proc1", proc1, proc1_iterations, STACK);
        } else {
            start("Proc2", proc2, proc2_iterations, STACK);
        }
    }
}
