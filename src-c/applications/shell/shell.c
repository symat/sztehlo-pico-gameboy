#include <stdio.h>
#include "pico/stdlib.h"
#include <display.h>
#include <kernel.h>


int main()
{
    stdio_init_all();

    while (true) {
        printf("Hello, world!\n");
        if(getOne() == 1) {
            printf("success! (display lib)\n");
        }
        if(getTwo() == 2) {
            printf("success! (kernel lib)\n");
        }
        sleep_ms(1000);
    }
}
