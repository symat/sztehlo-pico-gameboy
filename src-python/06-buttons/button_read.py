from machine import Pin
from utime import sleep

pin = Pin(14, Pin.IN, Pin.PULL_UP)

while True:
    try:
        if pin.value() == 0:
            print("lenyomva")
        else:
            print("felengedve")
        sleep(0.1) # sleep 0.1sec
    except KeyboardInterrupt:
        break

print("kilépés")
