# this is a test program, that allows you to find an optional resolution and offsets
# for different programs and see which pixels are visible on the rounded display
# (you need the LCD and button A and B connected)

from lcd import ST7789, RED, GREEN, BLUE, WHITE, BLACK
import time
from machine import Pin, ADC

display = None
power_adc = None
pin_left = Pin(4, Pin.IN, Pin.PULL_UP)
pin_right = Pin(5, Pin.IN, Pin.PULL_UP)
diode_drop_vlue = 0.37

last_button_event = time.ticks_ms()

def button_bouncing():
    global last_button_event
    if last_button_event + 200 > time.ticks_ms():
        return True
    last_button_event = time.ticks_ms()
    return False

def handle_left_button():
    if button_bouncing():
        return
    global diode_drop_vlue
    diode_drop_vlue -= 0.01

def handle_right_button():
    if button_bouncing():
        return
    global diode_drop_vlue
    diode_drop_vlue += 0.01

            

pin_left.irq(lambda pin: handle_left_button(), trigger=Pin.IRQ_FALLING)
pin_right.irq(lambda pin: handle_right_button(), trigger=Pin.IRQ_FALLING)


try:
    display = ST7789()
    power_adc = ADC(Pin(29))
    print("running...")
    display.backlight_level(255)
    X = 50
    Y = 40
    while True:
        time.sleep(0.1)
        adc_value = power_adc.read_u16()
        display.fill(BLACK)
 
        raw = 3.3 * adc_value / (2**16 - 1)
        battery = (raw * 2) + diode_drop_vlue
        display.text(f"16bit ADC value: {round(adc_value,3)}", X, Y+40, WHITE)
        display.text(f"raw voltage read: {round(raw,3)}V", X, Y+60, WHITE)
        display.text(f"diode drop: {round(diode_drop_vlue,3)}V", X, Y+80, WHITE)
        display.text(f"battery voltage: {round(battery,3)}V", X, Y+100, WHITE)

        plugged_in = raw * 2 > 4.5
        display.fill_rect(X, Y, 170, 20, WHITE)
        if plugged_in:
            display.text(f"     power: USB", X, Y+5, RED)
        else:
            display.text(f"   power: BATTERY", X, Y+5, BLUE)

        battery_max = 4.2
        battery_min = 2.5
        battery_level = (battery - battery_min) / (battery_max-battery_min)
        display.text(f"battery range: {battery_min}V ... {battery_max}V", X, Y+120, WHITE)
        display.text(f"battery level ratio: {round(battery_level,3)}", X, Y+140, WHITE)

        battery_level_percentage = battery_level * 100
        battery_level_percentage = min(battery_level_percentage, 100)
        battery_level_percentage = max(battery_level_percentage, 0)
        if plugged_in:
            display.text(f"battery level: charging...", X, Y+160, WHITE)
        else:
            display.text(f"battery level: {round(battery_level_percentage,0)}%", X, Y+160, WHITE)
            


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
