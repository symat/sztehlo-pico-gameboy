# simple, no timer / DMA, just single thrtead PWM: https://antirez.com/news/143

# audio download (ringtones): https://www.zedge.net/
# convert using https://www.audacityteam.org/download/
#    - 8000Hz sampling
#    - mono (single channel)
#    - unsigned 8 bit PMC  

import wave
import time
from machine import PWM, Pin
import machine



f = None
sound_pwm = None
fileName = "pirates_of_caribbean.wav"
#fileName = "cb_angels.wav"
bufferSize = 1024
oneSampleTime = 1_000_000 // 8000
sound_volume = 300   # 100 is low, 10.000 normal, 32768 max
sound_volumne_scale = 256  # the 0..255 sound sample will be multiplied with the scale 
                           # (max scale is 128, as it wil be resulting 32768 duty cycle)

try:
    sound_pwm = PWM(Pin(0), freq=32_000, duty_u16=0)

    f = wave.open(fileName, "rb")
    rate = f.getframerate()
    bytesDepth = f.getsampwidth()
    channels = f.getnchannels()
    frameCount = f.getnframes()
    duration = frameCount // 8000

    print(f"rate:{rate}, bytesDepth:{bytesDepth}, channels:{channels}, frameCount:{frameCount}")

    frameLeft = frameCount
    x = 0
    frame = 0
    next_time = time.ticks_us() + oneSampleTime
    while frameLeft > 0:
        readChunks = min(frameLeft, bufferSize)
        samples = f.readframes(readChunks)
        for i in range(readChunks):
            sound_pwm.duty_u16(int(samples[i]) * sound_volumne_scale)
            while time.ticks_us() < next_time:
                x += 1
            next_time += oneSampleTime
            frame += 1
            if frame % 8000 == 0:
                print(f"{frame // 8000}/{duration} (x:{x})")
        
        frameLeft -= bufferSize # each frame is exactly 1 byte


finally:
    if f != None:
        f.close()
    if sound_pwm != None:
        sound_pwm.duty_u16(0)
        sound_pwm.deinit()    

print("finished")