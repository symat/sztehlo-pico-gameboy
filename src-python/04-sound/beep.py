from machine import Pin, PWM
import time

sound_pwm = PWM(Pin(0), freq=262, duty_u16=0)
sound_volume = 100   # 100 is low, 10.000 normal, 32.000 max

print("Beeping a C note...")
while True:
    try:
        sound_pwm.duty_u16(sound_volume)  
        print("tone")
        time.sleep_ms(1000)
        sound_pwm.duty_u16(0)
        print("no tone")
        time.sleep_ms(1000)
    except KeyboardInterrupt:
        break
sound_pwm.duty_u16(0)
sound_pwm.deinit()
print("Finished.")
