#include <stdio.h>
#include "pico/stdlib.h"
#include <display.h>


int main()
{
    stdio_init_all();

    while (true) {
        printf("Hello, world!\n");
        if(getOne() == 1) {
            printf("success!\n");
        }
        sleep_ms(1000);
    }
}
