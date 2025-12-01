from machine import Pin, PWM
import time

# define the note frequencies used in the "Lawrence of Arabia" main theme
NOTE_4_CS = 277
NOTE_4_D = 294
NOTE_4_DS = 311
NOTE_4_FS = 370
NOTE_4_G = 392
NOTE_4_A = 440
NOTE_4_AS = 466
NOTE_4_B = 494
NOTE_5_C = 523
NOTE_5_CS = 554
NOTE_5_D = 587

NOTE_DURATION_MULTIPLIER = 100
NOTE_PAUSE_MS = 100

# the music (notes and duration)
notes = [
    NOTE_5_D, 8,
    NOTE_4_A, 12, 
    NOTE_4_FS, 4,
    NOTE_4_G, 2,
    NOTE_4_AS, 2,
    NOTE_5_CS, 2,
    NOTE_5_D, 8,
    NOTE_4_A, 12,
    NOTE_4_FS, 4,
    NOTE_4_G, 2,
    NOTE_4_DS, 2,
    NOTE_4_CS, 2,
    NOTE_4_D, 8,
    NOTE_4_A, 8,
    NOTE_4_G, 8,
    NOTE_5_C, 8,
    NOTE_4_AS, 8,
    NOTE_4_A, 16,
    NOTE_4_B, 2,
    NOTE_5_CS, 2
]


sound_pwm = PWM(Pin(0), freq=262, duty_u16=0)
sound_volume = 300   # 100 is low, 10.000 normal, 32.000 max

try:
    i = 0
    while True:
        note_freq = notes[i]
        i+= 1
        note_duration = notes[i]*NOTE_DURATION_MULTIPLIER
        i+= 1
        sound_pwm.freq(note_freq)
        sound_pwm.duty_u16(sound_volume)  
        print(f"tone {note_freq}Hz, for {note_duration}ms")
        time.sleep_ms(note_duration)
        sound_pwm.duty_u16(0)
        print("pause")
        time.sleep_ms(NOTE_PAUSE_MS)
        
        if i >= len(notes):
            i = 0

except KeyboardInterrupt:
    print("stopping...")
    sound_pwm.duty_u16(0)
    sound_pwm.deinit()
  

