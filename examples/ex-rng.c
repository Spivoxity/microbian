//
// Created by joshua on 23/05/23.
//

#include "hardware.h"
#include "../microbian.h"
#include "../lib.h"


void random_numbers_task(int n){

    for (int i = 0; i<n; i ++){
        printf("random(%d): ", i+1);// send it to the serial port to be printed.

        unsigned char rn = rng_get();
        printf("%u", rn);
        printf("\n");
    }

}
void die_task(int n);

void init(void)
{

    serial_init();
    rng_init();


    start("Random", random_numbers_task, 5, STACK);


    display_init();
    timer_init();
    start("Die", die_task,1,STACK);

}


 const unsigned die_faces[][6] =
        {
        IMAGE(0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0,
                      0, 0, 1, 0, 0,
                      0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0), // Face 1 (Dot)

            IMAGE(0, 0, 0, 0, 0,
                      0, 1, 0, 0, 0,
                      0, 0, 0, 0, 0,
                      0, 0, 0, 1, 0,
                      0, 0, 0, 0, 0), // Face 2 (Two Dots)

            IMAGE(0, 0, 0, 0, 0,
                      0, 1, 0, 0, 0,
                      0, 0, 1, 0, 0,
                      0, 0, 0, 1, 0,
                      0, 0, 0, 0, 0), // Face 3 (Three Dots)

            IMAGE(0, 0, 0, 0, 0,
                      0, 1, 0, 1, 0,
                      0, 0, 0, 0, 0,
                      0, 1, 0, 1, 0,
                      0, 0, 0, 0, 0), // Face 4 (Four Dots)

            IMAGE(0, 0, 0, 0, 0,
                      0, 1, 0, 1, 0,
                      0, 0, 1, 0, 0,
                      0, 1, 0, 1, 0,
                      0, 0, 0, 0, 0), // Face 5 (Five Dots)

            IMAGE(0, 0, 0, 0, 0,
                      0, 1, 0, 1, 0,
                      0, 1, 0, 1, 0,
                      0, 1, 0, 1, 0,
                      0, 0, 0, 0, 0)  // Face 6 (Six Dots)
        };

void die_task(int n){

    display_show(die_faces[n%6]);// print it for n seconds
    timer_delay(n*1000);

    while(1){
        unsigned char rn = rng_get();// get a random number
        display_show(die_faces[rn%6]);// print it for n seconds
        timer_delay(n*1000);
    }

}
