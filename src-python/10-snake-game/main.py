import time
from rgb_led import color_led
import machine
from lcd import ST7789, RED, GREEN, BLUE, WHITE, BLACK, color565
from sound_player import SimpleSoundPlayer
import button
import random

machine.freq(125_000_000)  # set the CPU and UART frequency to 125 MHz

display = None
sound_player = None
MENU_HEIGHT = 40
BOX_SIZE = 10
X_BOXES = 280 // BOX_SIZE
Y_BOXES = (240-MENU_HEIGHT) // BOX_SIZE
DARK_GREEN = color565(0, 155, 0)
PURPLE = color565(128, 0, 128)

snake_x = [4, 4, 3]
snake_y = [2, 1, 1]
food_x = 10
food_y = 10
direction = "none"
state = "game"

def move_food():
    global food_x, food_y
    food_x = random.randint(0, X_BOXES - 1)
    food_y = random.randint(0, Y_BOXES - 1)

def snake_box(x, y, head):
    display.rect(x*BOX_SIZE, MENU_HEIGHT + y * BOX_SIZE, 10, 10, GREEN, True)
    if not head:
        display.rect(x*BOX_SIZE+1, MENU_HEIGHT + y * BOX_SIZE +1, 8, 8, DARK_GREEN, True)

def food_box(x, y):
    r = BOX_SIZE // 2
    display.ellipse(x*BOX_SIZE + r, 
                    MENU_HEIGHT + y * BOX_SIZE + r, 
                    r, r, RED, True)

def snake_move(new_head_x, new_head_y):
    snake_x.insert(0, new_head_x)
    snake_y.insert(0, new_head_y)
    if new_head_x == food_x and new_head_y == food_y:
        move_food()
    else:
        snake_x.pop()
        snake_y.pop()


def crash():
    head_x = snake_x[0]
    head_y = snake_y[0]
    if head_x < 0 or head_y < 0 or head_x >= X_BOXES or head_y >= Y_BOXES:
        return True
    
    for i in range(1, len(snake_x)):
        if head_x == snake_x[i] and head_y == snake_y[i]:
            return True

    return False


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
    move_food()
    
    while True:
        display.fill(BLACK)
        up_pressed = button.button_press_events[button.BTN_UP]
        down_pressed = button.button_press_events[button.BTN_DOWN]
        left_pressed = button.button_press_events[button.BTN_LEFT]
        right_pressed = button.button_press_events[button.BTN_RIGHT]
        a_pressed = button.button_press_events[button.BTN_A]
        b_pressed = button.button_press_events[button.BTN_B]

        if state == "game":
            for i in range(len(snake_x)):
                snake_box(snake_x[i], snake_y[i], i==0)
            display.rect(0, MENU_HEIGHT - 2, 280, 2, PURPLE, True)
            food_box(food_x, food_y)

            display.show()

            if up_pressed:
                direction = "up"
            elif down_pressed:
                direction = "down"
            elif left_pressed:
                direction = "left"
            elif right_pressed:
                direction = "right"

            if direction == "up":
                snake_move(snake_x[0], snake_y[0]-1)
            elif direction == "down":
                snake_move(snake_x[0], snake_y[0]+1)
            elif direction == "left":
                snake_move(snake_x[0]-1, snake_y[0])
            elif direction == "right":
                snake_move(snake_x[0]+1, snake_y[0])
        

            if crash():
                state = "gameover"
        else:
                display.text("GAME OVER", 120, 100, WHITE)
                display.show()

                if a_pressed or b_pressed:
                    snake_x = [4, 4, 3]
                    snake_y = [2, 1, 1]
                    direction = "none"
                    state = "game"

        button.clear_button_events()
        
        for _ in range(15):
            button.check_buttons()
            time.sleep_ms(10)

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
