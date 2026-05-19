from machine import Pin
import time

BTN_LEFT = 0
BTN_RIGHT = 1
BTN_DOWN = 2
BTN_UP = 3
BTN_X = 4
BTN_Y = 5
BTN_A = 6
BTN_B = 7

BOUNTING_CHECK_DELAY_MS = 20       # time the button change needs to last to be registered 
BUTTON_REPEAT_DELAY_MS = 150       # how much time the user needs to continously press the button to register the press event again
BUTTON_X_ACTIVATION_TIME_MS = 400  # time required to X button to activate (avoid accidents when it pressed together with arrow buttons)

buttons = (Pin(4, Pin.IN, Pin.PULL_UP),
           Pin(5, Pin.IN, Pin.PULL_UP),
           Pin(7, Pin.IN, Pin.PULL_UP),
           Pin(6, Pin.IN, Pin.PULL_UP),
           Pin(9, Pin.IN, Pin.PULL_UP),
           Pin(8, Pin.IN, Pin.PULL_UP),
           Pin(28, Pin.IN, Pin.PULL_UP),
           Pin(27, Pin.IN, Pin.PULL_UP))


button_last_read = (buttons[BTN_LEFT].value(), 
                    buttons[BTN_RIGHT].value(),
                    buttons[BTN_DOWN].value(), 
                    buttons[BTN_UP].value(),
                    buttons[BTN_X].value(), 
                    buttons[BTN_Y].value(),
                    buttons[BTN_A].value(), 
                    buttons[BTN_B].value())

button_press_events = [False, False, False, False, False, False, False, False]

button_last_changed_ms = time.ticks_ms()
button_arrows_last_time_activated_ms = time.ticks_ms() - BUTTON_X_ACTIVATION_TIME_MS
button_x_pushed_last_time_ms = time.ticks_ms() - BUTTON_X_ACTIVATION_TIME_MS

def check_buttons(repeat_delay_ms = BUTTON_REPEAT_DELAY_MS, debug=False):
    global button_last_changed_ms
    global button_last_read
    global button_press_events
    global button_arrows_last_time_activated_ms
    global button_x_pushed_last_time_ms
    button_current_read = (buttons[BTN_LEFT].value(), 
                           buttons[BTN_RIGHT].value(),
                           buttons[BTN_DOWN].value(), 
                           buttons[BTN_UP].value(),
                           buttons[BTN_X].value(), 
                           buttons[BTN_Y].value(),
                           buttons[BTN_A].value(), 
                           buttons[BTN_B].value())
    if button_current_read[BTN_UP] == 0 or button_current_read[BTN_DOWN] == 0 or button_current_read[BTN_LEFT] == 0 or button_current_read[BTN_RIGHT] == 0:
        button_arrows_last_time_activated_ms = time.ticks_ms()
    
    if button_last_read[BTN_X] == 1 and button_current_read[BTN_X]:
        button_x_pushed_last_time_ms = time.ticks_ms()
    
    time_since_changed = time.ticks_ms() - button_last_changed_ms
    if (button_last_read != button_current_read and time_since_changed > BOUNTING_CHECK_DELAY_MS) or (time_since_changed > repeat_delay_ms and (0 in button_current_read)):  
        for i in range(len(button_last_read)):
            if button_current_read[i] == 0 and button_press_events[i] == False:
                if i == BTN_X: 
                    # only register the X button, if the arrows are not selected for some time and x is pressed for some time (X can be pressed accidentally)
                    if time.ticks_ms() - button_arrows_last_time_activated_ms > BUTTON_X_ACTIVATION_TIME_MS and time.ticks_ms() - button_x_pushed_last_time_ms > BUTTON_X_ACTIVATION_TIME_MS:
                        button_press_events[BTN_X] = True
                else:
                    button_press_events[i] = True
            
        button_last_read = button_current_read
        button_last_changed_ms = time.ticks_ms()
        if debug:
            print(f"buttons changed: {button_press_events}")

def clear_button_events():
    for i in range(len(button_last_read)):
        button_press_events[i] = False
        
def button_pressed(btn):
    if btn == BTN_X:
        return buttons[BTN_X].value() == 0 and time.ticks_ms() - button_arrows_last_time_activated_ms > BUTTON_X_ACTIVATION_TIME_MS and time.ticks_ms() - button_x_pushed_last_time_ms > BUTTON_X_ACTIVATION_TIME_MS

    return buttons[btn].value() == 0