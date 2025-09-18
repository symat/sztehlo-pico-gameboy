# this is a test program, that allows you to find an optional resolution and offsets
# for different programs and see which pixels are visible on the rounded display
# (you need the LCD and button A and B connected)

from lcd import ST7789, RED, GREEN, BLUE, WHITE, BLACK
import time
from machine import Pin

display = None
pin_a = Pin(7, Pin.IN, Pin.PULL_UP)
pin_b = Pin(6, Pin.IN, Pin.PULL_UP)

last_button_event = time.ticks_ms()

def button_bouncing():
    global last_button_event
    if last_button_event + 200 > time.ticks_ms():
        return True
    last_button_event = time.ticks_ms()
    return False

def handle_a_button():
    if button_bouncing():
        return
    global selected
    selected = (selected + 1) % 5

def handle_b_button():
    if button_bouncing():
        return
    global x1, x2, y1, y2, shrink
    if selected == 0:
        x1 += 1 if shrink else -1    
    elif selected == 1:
        x2 += -1 if shrink else 1
    elif selected == 2:
        y1 += 1 if shrink else -1
    elif selected == 3:
        y2 += -1 if shrink else 1
    else:
        shrink = not shrink
            

pin_a.irq(lambda pin: handle_a_button(), trigger=Pin.IRQ_FALLING)
pin_b.irq(lambda pin: handle_b_button(), trigger=Pin.IRQ_FALLING)

x1 = 10
x2 = 300
y1 = 10
y2 = 220

selected = 0
shrink = True

try:
    display = ST7789()
    print("running...")
    display.backlight_level(255)
    while True:
        time.sleep(0.01)
        display.fill(RED)
        display.fill_rect(x1, y1, x2-x1, y2-y1, WHITE)
        display.fill_rect(x1+1, y1+1, x2-x1-2, y2-y1-2, BLUE)


        display.fill_rect(75, 75+(selected*20), 120, 18, WHITE)
        display.fill_rect(76, 76+(selected*20), 118, 16, BLUE)


        display.text(f"LEFT: {x1}", 80, 80, WHITE)
        display.text(f"RIGHT: {x2}", 80, 100, WHITE)
        display.text(f"TOP: {y1}", 80, 120, WHITE)
        display.text(f"BOTTOM: {y2}", 80, 140, WHITE)
        display.text(f"delta: {"shrink" if shrink else "extend"}", 80, 160, WHITE)

        display.text(f"resolution: {x2-x1}x{y2-y1}", 100, 50, WHITE)

        display.show()


except KeyboardInterrupt:
    print("stopping...")

except Exception as err:
    print(f"Unexpected {err=}, {type(err)=}")
    raise

finally:
    if display != None:
        display.deinit()


print("stopped")
