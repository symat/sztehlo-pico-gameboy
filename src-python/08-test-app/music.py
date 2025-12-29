import time
from machine import PWM, Pin
from random import randint

sound_pwm = PWM(Pin(0), freq=262, duty_u16=0)

# max should be 32.000, but that might be too much
volume_values = (0, 100, 200, 500, 1000, 2000, 5000, 10000, 15000, 20000, 25000)
sound_volume_idx = 0   #  the user can select between 11 volume levels: 0 ... 10

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


# the music: notes and duration
music_o_christmas_tree = (
    NOTE_D4,4, NOTE_G4,-8, NOTE_G4,16, NOTE_G4,4, NOTE_A4,4, NOTE_B4,-8, NOTE_B4,16, NOTE_B4,-4, NOTE_B4,8, NOTE_A4,8, NOTE_B4,8, NOTE_C5,4, NOTE_F4,4, NOTE_A4,4, NOTE_G4,4, REST,16,
    NOTE_G4,-8, NOTE_G4,16, NOTE_G4,4, NOTE_A4,4, NOTE_B4,-8, NOTE_B4,16, NOTE_B4,-4, NOTE_B4,8, NOTE_A4,8, NOTE_B4,8, NOTE_C5,4, NOTE_F4,4, NOTE_A4,4, NOTE_G4,-4, REST,4, NOTE_D5,8,
    NOTE_D5,8, NOTE_B4,8, NOTE_E5,-4, NOTE_D5,8, NOTE_D5,8, NOTE_C5,8, NOTE_C5,-4, NOTE_C5,8, NOTE_C5,8, NOTE_A4,8, NOTE_D5,-4, NOTE_C5,8, NOTE_C5,8, NOTE_B4,8, NOTE_B4,4, NOTE_D4,4,
    NOTE_G4,-8, NOTE_G4,16, NOTE_G4,4, NOTE_A4,4, NOTE_B4,-8, NOTE_B4,16, NOTE_B4,-4, NOTE_B4,8, NOTE_A4,8, NOTE_B4,8, NOTE_C5,4, NOTE_F4,4, NOTE_A4,4, NOTE_G4,2
)

music_jingle_bells = (
    NOTE_E5,4, NOTE_E5,4, NOTE_E5,2,
    NOTE_E5,4, NOTE_E5,4, NOTE_E5,2,
    NOTE_E5,4, NOTE_G5,4, NOTE_C5,4, NOTE_D5,4,
    NOTE_E5,-2, REST,8,
    NOTE_F5,4, NOTE_F5,4, NOTE_F5,-4, NOTE_F5,8,
    NOTE_F5,4, NOTE_E5,4, NOTE_E5,4, NOTE_E5,8, NOTE_E5,8,
    NOTE_E5,4, NOTE_D5,4, NOTE_D5,4, NOTE_E5,4,
    NOTE_D5,2, NOTE_G5,2,
    NOTE_E5,4, NOTE_E5,4, NOTE_E5,2,
    NOTE_E5,4, NOTE_E5,4, NOTE_E5,2,
    NOTE_E5,4, NOTE_G5,4, NOTE_C5,4, NOTE_D5,4,
    NOTE_E5,-2, REST,8,
    NOTE_F5,4, NOTE_F5,4, NOTE_F5,4, NOTE_F5,4,
    NOTE_F5,4, NOTE_E5,4, NOTE_E5,4, NOTE_E5,8, NOTE_E5,8,
    NOTE_G5,4, NOTE_G5,4, NOTE_F5,4, NOTE_D5,4,
    NOTE_C5,-2    
)

music_rudolf_the_red_nosed = (
    NOTE_G4,8, NOTE_A4,4, NOTE_G4,8, NOTE_E4,4, NOTE_C5,4, NOTE_A4,4, NOTE_G4,-2, NOTE_G4,8, NOTE_A4,8, NOTE_G4,8, NOTE_A4,8, NOTE_G4,4, NOTE_C5,4, NOTE_B4,1,
    NOTE_F4,8, NOTE_G4,4, NOTE_F4,8, NOTE_D4,4, NOTE_B4,4, NOTE_A4,4, NOTE_G4,-2, NOTE_G4,8, NOTE_A4,8, NOTE_G4,8, NOTE_A4,8, NOTE_G4,4, NOTE_A4,4, NOTE_E4,1,
    NOTE_G4,8, NOTE_A4,4, NOTE_G4,8, NOTE_E4,4, NOTE_C5,4, NOTE_A4,4, NOTE_G4,-2, NOTE_G4,8, NOTE_A4,8, NOTE_G4,8, NOTE_A4,8, NOTE_G4,4, NOTE_C5,4, NOTE_B4,1,
    NOTE_F4,8, NOTE_G4,4, NOTE_F4,8, NOTE_D4,4, NOTE_B4,4, NOTE_A4,4, NOTE_G4,-2, NOTE_G4,8, NOTE_A4,8, NOTE_G4,8, NOTE_A4,8, NOTE_G4,4, NOTE_D5,4, NOTE_C5,1,
    NOTE_A4,4, NOTE_A4,4, NOTE_C5,4, NOTE_A4,4, NOTE_G4,4, NOTE_E4,4, NOTE_G4,2, NOTE_F4,4, NOTE_A4,4, NOTE_G4,4, NOTE_F4,4, NOTE_E4,1,
    NOTE_D4,4, NOTE_E4,4, NOTE_G4,4, NOTE_A4,4, NOTE_B4,4, NOTE_B4,4, NOTE_B4,2, NOTE_C5,4, NOTE_C5,4, NOTE_B4,4, NOTE_A4,4, NOTE_G4,4, NOTE_F4,8, NOTE_D4,-2,
    NOTE_G4,8, NOTE_A4,4, NOTE_G4,8, NOTE_E4,4, NOTE_C5,4, NOTE_A4,4, NOTE_G4,-2, NOTE_G4,8, NOTE_A4,8, NOTE_G4,8, NOTE_A4,8, NOTE_G4,4, NOTE_C5,4, NOTE_B4,1,
    NOTE_F4,8, NOTE_G4,4, NOTE_F4,8, NOTE_D4,4, NOTE_B4,4, NOTE_A4,4, NOTE_G4,-2, NOTE_G4,8, NOTE_A4,8, NOTE_G4,8, NOTE_A4,8, NOTE_G4,4, NOTE_D5,4, NOTE_C5,1
)

music_silent_night = (
    NOTE_G4,-4, NOTE_A4,8, NOTE_G4,4,
    NOTE_E4,-2, 
    NOTE_G4,-4, NOTE_A4,8, NOTE_G4,4,
    NOTE_E4,-2, 
    NOTE_D5,2, NOTE_D5,4,
    NOTE_B4,-2,
    NOTE_C5,2, NOTE_C5,4,
    NOTE_G4,-2,
    NOTE_A4,2, NOTE_A4,4,
    NOTE_C5,-4, NOTE_B4,8, NOTE_A4,4,
    NOTE_G4,-4, NOTE_A4,8, NOTE_G4,4,
    NOTE_E4,-2, 
    NOTE_A4,2, NOTE_A4,4,
    NOTE_C5,-4, NOTE_B4,8, NOTE_A4,4,
    NOTE_G4,-4, NOTE_A4,8, NOTE_G4,4,
    NOTE_E4,-2, 
    NOTE_D5,2, NOTE_D5,4,
    NOTE_F5,-4, NOTE_D5,8, NOTE_B4,4,
    NOTE_C5,-2,
    NOTE_E5,-2,
    NOTE_C5,4, NOTE_G4,4, NOTE_E4,4,
    NOTE_G4,-4, NOTE_F4,8, NOTE_D4,4,
    NOTE_C4,-2    
)

music_we_wish_you_a_merry_christmas = (
    NOTE_C5,4,
    NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
    NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
    NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
    NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
    NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,
    NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
    NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
  
    NOTE_F5,2, NOTE_C5,4,
    NOTE_F5,4, NOTE_F5,8, NOTE_G5,8, NOTE_F5,8, NOTE_E5,8,
    NOTE_D5,4, NOTE_D5,4, NOTE_D5,4,
    NOTE_G5,4, NOTE_G5,8, NOTE_A5,8, NOTE_G5,8, NOTE_F5,8,
    NOTE_E5,4, NOTE_C5,4, NOTE_C5,4,
    NOTE_A5,4, NOTE_A5,8, NOTE_AS5,8, NOTE_A5,8, NOTE_G5,8,
    NOTE_F5,4, NOTE_D5,4, NOTE_C5,8, NOTE_C5,8,
    NOTE_D5,4, NOTE_G5,4, NOTE_E5,4,
    NOTE_F5,2
)

# notes and tempo pairs
musics = (
    music_o_christmas_tree, 115,
    music_jingle_bells, 180,
    music_rudolf_the_red_nosed, 150,
    music_silent_night, 130,
    music_we_wish_you_a_merry_christmas, 160,
    music_rudolf_the_red_nosed, 150
)


music_current_song_index = randint(0, (len(musics) //2) -1)
music_current_note_index = -1
music_next_change_tick_ms = 0
music_now_delay = True


def play_in_background(notes, tempo):
    global music_now_delay, music_next_change_tick_ms, music_current_note_index
    current_time_ms = time.ticks_ms()
    #print(f"update: current_note:{music_current_note_index}, now_delay:{music_now_delay}, tempo:{tempo}")
    if current_time_ms > music_next_change_tick_ms:
        if music_now_delay:
            music_current_note_index += 1
        if music_current_note_index * 2 >= len(notes):
            return False
        
        whole_note = (60000 * 4) / tempo
        note_hz = notes[music_current_note_index * 2]
        note_type = notes[music_current_note_index * 2+1]
        if note_type > 0:
            note_duration = whole_note / note_type
        else:
            note_duration = whole_note / note_type * -1.5
        
        if music_now_delay:
            music_next_change_tick_ms = current_time_ms + int(note_duration * (1.0-NOTE_PAUSE_RATIO))
            if note_hz == REST:
                #print(f"freq: REST, duration:{note_duration}, current_ms:{current_time_ms}, next_ms:{music_next_change_tick_ms}")
                sound_pwm.duty_u16(0)
            else:
                #print(f"freq:{note_hz}, duration:{note_duration}, current_ms:{current_time_ms}, next_ms:{music_next_change_tick_ms}")
                sound_pwm.freq(note_hz)
                sound_pwm.duty_u16(volume_values[sound_volume_idx])
        else:
            sound_pwm.duty_u16(0)
            music_next_change_tick_ms = current_time_ms + int(note_duration * NOTE_PAUSE_RATIO)
            #print(f"pause, duration:{note_duration}, current_ms:{current_time_ms}, next_ms:{music_next_change_tick_ms}")
           
            
        music_now_delay = not music_now_delay
    return True

            
def pause_music():
    sound_pwm.duty_u16(0)
    
def get_volume():
    return sound_volume_idx  # 0...10

def set_volume(vol_idx):
    global sound_volume_idx
    sound_volume_idx = vol_idx
    
def restart_with_new_song():
    global music_current_song_index, music_now_delay, music_current_note_index
    pause_music()
    music_current_song_index = randint(0, (len(musics) //2) -1)
    music_now_delay = True
    music_current_note_index = -1
    print("new song")
        
def play_all_musics_in_background():
    global music_next_change_tick_ms
    #print("update music frequencies")
    if not play_in_background(musics[2*music_current_song_index], musics[2*music_current_song_index+1]):
        restart_with_new_song()
        music_next_change_tick_ms = music_next_change_tick_ms + 2000   # 2 sec pause between songs
        play_in_background(musics[2*music_current_song_index], musics[2*music_current_song_index+1])
        
        

    
    
#while True:
#    play_all_musics_in_background()
#    time.sleep_ms(100)
        