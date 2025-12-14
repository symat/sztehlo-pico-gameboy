# hamster-pico

Small handheld computer based on Raspberry Pi pico 2. Made for Nintendo Game Boy Color emulation and other demonstration, hobby use-cases.

# v1


- meant for students to solder, mostly using THT components and modules (RP2040-Zero + TP-4056 module + SD card module + LCD module)
- limitations: no headphone support, no extender, no link between consoles

# v1.1
- use the same modules, but have resistor ladders and use two 12bit ADC channels to handle 4-4 buttons
- we could use more SMD components (for resistors, transistors, LEDs, dioes, etc - to have smaller footprint) - try to use larger smd components still, enabling easier soldering)
- this frees up GPIO 4-9 (i2c0 and i2c1 and uart1), allowing extender and gameboy link implementation 
- implement game boy link (i2c should be able to handle 4-500Kbps, although it is slower... alternatively UART is faster, 921.6Kbps by default; but i2c is async and supports multiple slaves on a single channel via addressing - so multiple periperials / players could be connected)

# v2

- fully done with smd components (maybe LCD can still be a module?)
- load sharing implemented by wiring the TP-4056 module with extra components for better load sharing: https://forum.arduino.cc/t/tp4056-circuit-schematic/1404367/9 
- we could add larger flash if needed (rp2040-zero uses 2MB, some GB color cartridge might use up to 8MB - or we can fallback to the sd card in this case)
- extra connections (GP17-25) can be used for monitoring audio jack, add extender ports (other i2c and GPIOs) - so no resistor ladders are needed
- boot, reset and debug pins can be made available, even uart0 can be make available for testing (but this requires backward-incompatible changes with current v1 schema)

# Peripherials:
- high speed: based on uart, only one is supported at a time: printer, monitor or fast p2p Network (need a switch)
- low speed: based on i2c, max 128 devices (practically max 20-30): keyboard, mouse, slow Network with no 
- adc channel: microphone
- TODO: RTC (as integrated chip DS1302?, or as i2c device https://www.hestore.hu/search.php?q=DS1302 )

# refs

https://www.copetti.org/writings/consoles/game-boy/





