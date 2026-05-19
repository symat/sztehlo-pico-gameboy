from neopixel import NeoPixel
from machine import Pin

LED_BRIGHTNESS = 0.1  # between 0 and 1

pixel = NeoPixel(Pin(16), 1)

# light-up the RGB LED (r, g, b: 0...255)
# turn it off by using: color_led(0,0,0)
def color_led(r, g, b, brightness=LED_BRIGHTNESS):
    pixel[0] = (int(r*brightness), int(g*brightness), int(b*brightness))
    pixel.write()
