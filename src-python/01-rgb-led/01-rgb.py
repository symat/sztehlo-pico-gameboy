import time
import neopixel
import machine

pixel = neopixel.NeoPixel(machine.Pin(16), 1)

def color(r, g, b):
    pixel[0] = (r, g, b)
    pixel.write()

while True:
    color(255, 0, 0)
    time.sleep(0.5)
    color(0, 255, 0)
    time.sleep(0.5)
    color(0, 0, 255)
    time.sleep(0.5)