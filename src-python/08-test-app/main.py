import time
import neopixel
import machine
from machine import Pin, SoftSPI, PWM
from lcd import ST7789, RED, GREEN, BLUE, WHITE, BLACK, color565
import framebuf
import math
from random import randint
import music
import button

machine.freq(125_000_000)  # set the CPU and UART frequency to 125 MHz

LED_BRIGHTNESS = 0.2  # between 0 and 1

pixel = neopixel.NeoPixel(machine.Pin(16), 1)

def color_led(r, g, b):
    pixel[0] = (int(r*LED_BRIGHTNESS), int(g*LED_BRIGHTNESS), int(b*LED_BRIGHTNESS))
    pixel.write()


def paint_btn(btn_id, x, y, text, text_offset):
    w = 48
    h = 14
    bg_color = color565(24, 83, 232)
    frame_color = bg_color
    if button.button_pressed(btn_id):
        frame_color = color565(238, 228, 18)
    display.rect(x, y, w, h, frame_color, False)
    display.rect(x+1, y+1, w-2, h-2, frame_color, False)
    display.rect(x+2, y+2, w-4, h-4, bg_color, True)
    display.text(text, x+text_offset, y+4, WHITE)


display = None

try:
    display = ST7789()
    print("running...")
    
    backlight = 200
    display.backlight_level(backlight)

    phase = 1
    sleep_time_ms = 5
    r = 255
    g = 0
    b = 0
    color_led(r, g, b)
    while True:
        rgb_color=color565(r, g, b)
        
        music.play_all_musics_in_background()
        
        # the screen is 280x240, with rounded edges
        
        display.fill(BLACK)
        display.rect(0, 0, 280, 240, rgb_color) # we draw a rectangle, but it's edges won't be visible
        # drawing the rounded corners (only a single quadrant each time):
        radius=42
        display.ellipse(radius, radius, radius, radius, rgb_color, False, 0b00000010)
        display.ellipse(radius, 239-radius, radius, radius, rgb_color, False, 0b00000100)
        display.ellipse(279-radius, 239-radius, radius, radius, rgb_color, False, 0b00001000)
        display.ellipse(279-radius, radius, radius, radius, rgb_color, False, 0b00000001)
        
        y = 160
        x = 40
        display.text(f"up/down:", x, y, WHITE)
        display.text(f"  music volume ({music.get_volume()}/10)", x, y+15, WHITE)
        display.text(f"left/right:", x, y + 30, WHITE)
        display.text(f"  LCD brightness ({backlight}/255)", x, y + 45, WHITE)
        display.text("A: change song", x, y + 60, WHITE)

        display.text("RGB LED", x+100, 150, WHITE)
        display.ellipse(x+130, 120, 20, 20, rgb_color, True)
        
        paint_btn(button.BTN_LEFT, x-20, 65, "LEFT", 8)
        paint_btn(button.BTN_RIGHT, x+100, 65, "RIGHT", 4)
        paint_btn(button.BTN_UP, x+40, 40, "UP", 15)
        paint_btn(button.BTN_DOWN, x+40, 90, "DOWN", 8)
        
        paint_btn(button.BTN_X, x+40, 65, "X", 18)
        paint_btn(button.BTN_Y, x-28, 20, "Y", 18)
        paint_btn(button.BTN_A, x+180, 35, "A", 18)
        paint_btn(button.BTN_B, x+160, 60, "B", 18)


        display.show()
        
        
        button.check_buttons()

        if button.button_press_events[button.BTN_A]:
            music.restart_with_new_song()
            
        if button.button_press_events[button.BTN_LEFT]:
            backlight = max(backlight - 15, 10)
            display.backlight_level(backlight)

        if button.button_press_events[button.BTN_RIGHT]:
            backlight = min(backlight + 15, 255)
            display.backlight_level(backlight)
            
        if button.button_press_events[button.BTN_UP]:
            music.set_volume(min(10, music.get_volume()+1))
            
        if button.button_press_events[button.BTN_DOWN]:
            music.set_volume(max(0, music.get_volume()-1))
        
        button.clear_button_events()
            
        if phase == 1:
            r -= 1
            g += 1
            if r==0:
                phase = 2
        elif phase == 2:
            g -= 1
            b += 1
            if g==0:
                phase = 3
        else:
            b -= 1
            r += 1
            if b==0:
                phase = 1
            
        color_led(r, g, b)
        time.sleep_ms(sleep_time_ms)

except KeyboardInterrupt:
    print("stopping...")

except Exception as err:
    print(f"Unexpected {err=}, {type(err)=}")
    raise

finally:
    if display != None:
        display.deinit()
    color_led(0, 0, 0)
    music.pause_music()


print("stopped")
