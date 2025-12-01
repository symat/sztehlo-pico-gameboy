from lcd import ST7789, color565
from machine import Pin
import time

display = None
button = Pin(5, Pin.IN, Pin.PULL_UP)
korábban_lenyomva = False

háttér = color565(0, 0, 0) # r,g,b: 0...255
ecset = color565(255, 0, 0) # r,g,b: 0...255

x = 40

try:
    display = ST7789()
    display.fill(háttér)
    display.ellipse(x, 120, 20, 20, ecset, True)
    display.show()
    print("running...")
    while True:
        lenyomva = (button.value() == 0)
        if lenyomva and not korábban_lenyomva:
            display.fill(háttér)
            x = (x + 20) % 280
            display.ellipse(x, 120, 20, 20, ecset, True)
            display.show()
            
        korábban_lenyomva = lenyomva
        time.sleep(0.05)
        print(f"x: {x}")

except KeyboardInterrupt:
    print("stopping...")

if display != None:
    display.deinit()

print("stopped")
