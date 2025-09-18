from lcd import ST7789, RED, GREEN, BLUE, WHITE, BLACK
import time

display = None

try:
    display = ST7789()
    print("running...")
    bl_level = 32
    colors = [RED, GREEN, BLUE]
    text_colors = [WHITE, BLACK, WHITE]
    c = 0
    while True:
        time.sleep(1)
        print(f"color: {colors[c]}, brightness: {bl_level}")
        display.backlight_level(bl_level)
        display.fill(colors[c])

        display.text("MicroPython!", 20, 50, text_colors[c])
        display.fill_rect(150, 110, 50, 30, colors[(c + 1) % 3])
        display.fill_rect(210, 170, 70, 20, colors[(c + 2) % 3])

        display.show()
        bl_level += 64
        bl_level = bl_level % 256
        c += 1
        c = c % 3


except KeyboardInterrupt:
    print("stopping...")

except Exception as err:
    print(f"Unexpected {err=}, {type(err)=}")
    raise

finally:
    if display != None:
        display.deinit()


print("stopped")
