import time
from rgb_led import color_led
import machine
from lcd import ST7789, RED, GREEN, BLUE, WHITE, BLACK, color565
from sound_player import SimpleSoundPlayer
import button

machine.freq(125_000_000)  # set the CPU and UART frequency to 125 MHz

display = None
sound_player = None

try:
    print("start init...")
    color_led(255, 255, 0)  # yellow
    display = ST7789()
    display.backlight_level(200)
    display.fill(BLACK)
    display.show()
    sound_player = SimpleSoundPlayer()
    sound_player.set_volume(2)
    print("running...")
    color_led(0, 255, 0) # green
    
    sleep_time_ms = 5
    i = 0
    while True:
        
        display.rect(50, 50, 200, 50, BLACK, True)
        display.text(f"Hello World! {i}", 50, 50, GREEN)
        display.show()
        
        button.check_buttons()
        if button.button_press_events[button.BTN_A]:
            i += 1
        button.clear_button_events()
            
        time.sleep_ms(sleep_time_ms)

except KeyboardInterrupt:
    print("stopping...")

except Exception as err:
    print(f"Unexpected {err=}, {type(err)=}")
    raise

finally:
    if display != None:
        display.deinit()
    if sound_player != None:
        sound_player.deinit()
    color_led(0, 0, 0)



print("stopped")
