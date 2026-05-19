
#   With the SimpleSoundPlayer, you can:
#       - define musics using notes
#       - add (register) musics in the sound player
#       - start / pause any music you registered 
#       - play music in the background by calling the play_current_music_in_background() regularly 
#       - set music volume
#   
#   Example:
# 
# music_example_silent_night = (
#     NOTE_G4,-4, NOTE_A4,8, NOTE_G4,4,
#     NOTE_E4,-2, 
#     NOTE_G4,-4, NOTE_A4,8, NOTE_G4,4,
#     NOTE_E4,-2, 
#     NOTE_D5,2, NOTE_D5,4,
#     NOTE_B4,-2,
#     NOTE_C5,2, NOTE_C5,4,
#     NOTE_G4,-2,
#     NOTE_A4,2, NOTE_A4,4,
#     NOTE_C5,-4, NOTE_B4,8, NOTE_A4,4,
#     NOTE_G4,-4, NOTE_A4,8, NOTE_G4,4,
#     NOTE_E4,-2, 
#     NOTE_A4,2, NOTE_A4,4,
#     NOTE_C5,-4, NOTE_B4,8, NOTE_A4,4,
#     NOTE_G4,-4, NOTE_A4,8, NOTE_G4,4,
#     NOTE_E4,-2, 
#     NOTE_D5,2, NOTE_D5,4,
#     NOTE_F5,-4, NOTE_D5,8, NOTE_B4,4,
#     NOTE_C5,-2,
#     NOTE_E5,-2,
#     NOTE_C5,4, NOTE_G4,4, NOTE_E4,4,
#     NOTE_G4,-4, NOTE_F4,8, NOTE_D4,4,
#     NOTE_C4,-2    
# )
# 
# sound = SimpleSoundPlayer()
# music_id = sound.add_music(music_example_silent_night, 130)
# sound.set_volume(2)
# 
# while True:
#     sound.play_current_music_in_background()
#     time.sleep_ms(100)
#     if not sound.is_music_playing():
#         time.sleep_ms(1000)
#         sound.play_music(music_id)
#         


import time
from machine import PWM, Pin


# max should be 32.000, but that might be too much
VOLUME_VALUES = (0, 100, 200, 500, 1000, 2000, 5000, 10000, 15000, 20000, 25000)

NOTE_B0  =  31
NOTE_C1  =  33
NOTE_CS1 =  35
NOTE_D1  =  37
NOTE_DS1 =  39
NOTE_E1  =  41
NOTE_F1  =  44
NOTE_FS1 =  46
NOTE_G1  =  49
NOTE_GS1 =  52
NOTE_A1  =  55
NOTE_AS1 =  58
NOTE_B1  =  62
NOTE_C2  =  65
NOTE_CS2 =  69
NOTE_D2  =  73
NOTE_DS2 =  78
NOTE_E2  =  82
NOTE_F2  =  87
NOTE_FS2 =  93
NOTE_G2  =  98
NOTE_GS2 = 104
NOTE_A2  = 110
NOTE_AS2 = 117
NOTE_B2  = 123
NOTE_C3  = 131
NOTE_CS3 = 139
NOTE_D3  = 147
NOTE_DS3 = 156
NOTE_E3  = 165
NOTE_F3  = 175
NOTE_FS3 = 185
NOTE_G3  = 196
NOTE_GS3 = 208
NOTE_A3  = 220
NOTE_AS3 = 233
NOTE_B3  = 247
NOTE_C4  = 262
NOTE_CS4 = 277
NOTE_D4  = 294
NOTE_DS4 = 311
NOTE_E4  = 330
NOTE_F4  = 349
NOTE_FS4 = 370
NOTE_G4  = 392
NOTE_GS4 = 415
NOTE_A4  = 440
NOTE_AS4 = 466
NOTE_B4  = 494
NOTE_C5  = 523
NOTE_CS5 = 554
NOTE_D5  = 587
NOTE_DS5 = 622
NOTE_E5  = 659
NOTE_F5  = 698
NOTE_FS5 = 740
NOTE_G5  = 784
NOTE_GS5 = 831
NOTE_A5  = 880
NOTE_AS5 = 932
NOTE_B5  = 988
NOTE_C6  =1047
NOTE_CS6 =1109
NOTE_D6  =1175
NOTE_DS6 =1245
NOTE_E6  =1319
NOTE_F6  =1397
NOTE_FS6 =1480
NOTE_G6  =1568
NOTE_GS6 =1661
NOTE_A6  =1760
NOTE_AS6 =1865
NOTE_B6  =1976
NOTE_C7  =2093
NOTE_CS7 =2217
NOTE_D7  =2349
NOTE_DS7 =2489
NOTE_E7  =2637
NOTE_F7  =2794
NOTE_FS7 =2960
NOTE_G7  =3136
NOTE_GS7 =3322
NOTE_A7  =3520
NOTE_AS7 =3729
NOTE_B7  =3951
NOTE_C8  =4186
NOTE_CS8 =4435
NOTE_D8  =4699
NOTE_DS8 =4978
REST     =   0

NOTE_PAUSE_RATIO = 0.1


class SimpleSoundPlayer():
    def __init__(self):
        
        # notes and tempo pairs
        self.musics = []

        self.music_current_id = -1
        self.music_current_note_index = -1
        self.music_next_change_tick_ms = 0
        self.music_now_delay = True

        self.sound_pwm = PWM(Pin(0), freq=262, duty_u16=0)
        self.sound_volume_idx = 0   #  the user can select between 11 volume levels: 0 ... 10


    def play_in_background(self, notes, tempo):
        current_time_ms = time.ticks_ms()
        #print(f"update: current_note:{music_current_note_index}, now_delay:{music_now_delay}, tempo:{tempo}")
        if current_time_ms > self.music_next_change_tick_ms:
            if self.music_now_delay:
                self.music_current_note_index += 1
            if self.music_current_note_index * 2 >= len(notes):
                return False
            
            whole_note = (60000 * 4) / tempo
            note_hz = notes[self.music_current_note_index * 2]
            note_type = notes[self.music_current_note_index * 2+1]
            if note_type > 0:
                note_duration = whole_note / note_type
            else:
                note_duration = whole_note / note_type * -1.5
            
            if self.music_now_delay:
                self.music_next_change_tick_ms = current_time_ms + int(note_duration * (1.0-NOTE_PAUSE_RATIO))
                if note_hz == REST:
                    #print(f"freq: REST, duration:{note_duration}, current_ms:{current_time_ms}, next_ms:{music_next_change_tick_ms}")
                    self.sound_pwm.duty_u16(0)
                else:
                    #print(f"freq:{note_hz}, duration:{note_duration}, current_ms:{current_time_ms}, next_ms:{music_next_change_tick_ms}")
                    self.sound_pwm.freq(note_hz)
                    self.sound_pwm.duty_u16(VOLUME_VALUES[self.sound_volume_idx])
            else:
                self.sound_pwm.duty_u16(0)
                self.music_next_change_tick_ms = current_time_ms + int(note_duration * NOTE_PAUSE_RATIO)
                #print(f"pause, duration:{note_duration}, current_ms:{current_time_ms}, next_ms:{music_next_change_tick_ms}")
                
            self.music_now_delay = not self.music_now_delay
        return True

                
    def pause_music(self):
        self.sound_pwm.duty_u16(0)
        
        
    def get_volume(self):
        return self.sound_volume_idx  # 0...10

    def set_volume(self, vol_idx):
        self.sound_volume_idx = vol_idx
    
            
    def play_current_music_in_background(self):
        if not self.is_music_playing():
            return
        #print("update music frequencies")
        if not self.play_in_background(self.musics[2*self.music_current_id], self.musics[2*self.music_current_id+1]):
            self.music_current_id = -1
            
            
    def add_music(self, notes, tempo):
        if len(notes) % 2 != 0:
            raise ValueError("each note must have a duration value")
        self.musics.append(notes)
        self.musics.append(tempo)
        return (len(self.musics) // 2 )- 1   # returns with the music ID

    def play_music(self, music_id):
        self.music_current_id = music_id;
        self.music_current_note_index = -1
        self.music_now_delay = False
        self.music_next_change_tick_ms = time.ticks_ms()
        

    def is_music_playing(self):
        return 0 <= self.music_current_id <= len(self.musics)
        
    def deinit(self):
        self.pause_music()
        self.sound_pwm.deinit()


