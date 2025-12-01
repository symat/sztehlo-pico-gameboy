from machine import Pin
from utime import sleep

button = Pin(14, Pin.IN, Pin.PULL_UP)
led = Pin(6, Pin.OUT)
led.off()
korábban_lenyomva = False

while True:
    try:
        lenyomva = button.value() == 0
        if lenyomva and not korábban_lenyomva:
            led.toggle()
        korábban_lenyomva = lenyomva
        sleep(0.1) # sleep 0.1sec
    except KeyboardInterrupt:
        break

led.off()
print("kilépés")
