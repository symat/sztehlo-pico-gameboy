
# more complicated, with PWM and DMA, stereo for headphones, SD card:
# - we don't need stereo, also no need for ground pin
# - we need to play it in the background, possibly use timer and interrupts to play the chunks
# https://github.com/danjperron/PicoAudioPWM?tab=readme-ov-file
# https://forums.raspberrypi.com/viewtopic.php?t=308919

# using the same, all files collected:
# https://github.com/Lucvolders/MicroPython/tree/main/Libraries/PicoAudio
# part 1, audacity: https://lucstechblog.blogspot.com/2024/10/pico-audio-part-3.html
# part 2, hw: https://lucstechblog.blogspot.com/2024/10/audio-on-pico-part-2-hardware.html
# part 3, software: https://lucstechblog.blogspot.com/2024/10/pico-audio-part-3.html


# in C, with interrupts to prepare chunks:
# https://www.hackster.io/news/learn-how-to-use-pwm-to-generate-an-audio-signal-from-your-raspberry-pi-pico-00347dece8b8


# AI anwer to "pico PWM DMA"
# Pico DMA PWM refers to using the RP2040 microcontroller's Direct Memory Access (DMA) engine to control Pulse Width Modulation (PWM) output. This technique offloads PWM control from the CPU, allowing for high-resolution PWM signals and enabling the generation of audio or complex waveforms without CPU intervention. By configuring DMA to transfer data from memory to the PWM hardware registers, you can automate the output of PWM signals.  
# How it works:
# 1. PWM Hardware:
# The Pico's RP2040 chip has a dedicated PWM hardware block that can generate various frequencies and duty cycles. 
# 2. DMA Controller:
# The DMA controller is a hardware component that can move data between memory and peripherals without CPU involvement. 
# 3. Configuration:
# You set up a PWM channel on a specific GPIO pin. 
# You then configure a DMA channel to read your desired PWM values (e.g., audio samples) from a memory buffer. 
# The DMA controller is configured to write these values into the PWM hardware's control registers. 
# A timer or another peripheral can trigger the DMA transfer. 
# 4. Operation:
# When a DMA transfer is triggered, the DMA controller sends the PWM value from the buffer to the PWM hardware, which then updates the PWM output signal. The DMA can be chained, allowing for continuous streams of data to be sent to the PWM without repeated CPU intervention.


# audio download (ringtones): https://www.zedge.net/
# convert using https://www.audacityteam.org/download/
#    - 8000Hz sampling
#    - mono (single channel)
#    - unsigned 8 bit PMC  