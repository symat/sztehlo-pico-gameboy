# Example about mounting and using SD card
# After mount, standard filesystem operations work:
# - https://docs.micropython.org/en/latest/library/os.html#filesystem-access
# - https://docs.micropython.org/en/latest/library/vfs.html
# - standard python 'open' function for reading / writing files
#   (https://realpython.com/read-write-files-python/)


import time
import neopixel
from machine import SPI, Pin
import os
import vfs
import sdcard

# Constants
SPI_BUS = 0     # later, running on the gameboy: 1
SCK_PIN = 2     # later, running  on the gameboy: 10
MOSI_PIN = 3    # later, running  on the gameboy: 11
MISO_PIN = 4    # later, running  on the gameboy: 12
CS_PIN = 1      # later, running  on the gameboy: 13
SD_MOUNT_PATH = '/sd'


SPI_BUS = 1
SCK_PIN = 10
MOSI_PIN = 11
MISO_PIN = 12
CS_PIN = 13


def color_and_sleep(r, g, b, wait_time):
    pixel[0] = (r, g, b)
    pixel.write()
    time.sleep(wait_time)
    
def list_files_recursive(path='.', level=0):
    for (name, type, inode, size) in os.ilistdir(path):
        full_path = f"{path}/{name}"
        if type==0x4000:
            print('   ' * level, full_path+"/", "(folder)")
            list_files_recursive(full_path, level=level+1)
        else:
            print('   ' * level, full_path, f"(size: {size} bytes)")


pixel = neopixel.NeoPixel(Pin(16), 1)

# init: blue color
color_and_sleep(0, 0, 100, 0.5)

try:
    spi = SPI(SPI_BUS, 1_000_000, sck=Pin(SCK_PIN), mosi=Pin(MOSI_PIN), 
              miso=Pin(MISO_PIN), polarity=0, phase=0, bits=8, firstbit=SPI.MSB)
    cs_pin = Pin(CS_PIN, Pin.OUT)
    sd = sdcard.SDCard(spi, cs_pin)
    vfs.mount(vfs.VfsFat(sd), "/sd")  # should raise an error, if no SDCard inserted

    # init over, start to use the SDCard: yellow
    color_and_sleep(100, 100, 0, 0.5)
 
    filename = "/sd/test.txt"
    data = ""
    with open(filename, "w") as f:
        print("Writing data to file sdtest.txt...")
        f.write("This is a test for micro SD card\r\n")
        print("Writing to file completed")

    with open(filename, "r") as f:
        print("Reading data from file sdtest.txt...")
        data = f.read()
        print("Data read completed")
        print("Data:", data)
        
    if data == "This is a test for micro SD card\r\n":
        # read and write operations were working fine: green
        color_and_sleep(0, 100, 0, 1)
    else:
        # problem... 
        raise Exception("data mismatch when reading back the file")

    print("listing all the files on the sdcard:")
    list_files_recursive("/sd")

        
except Exception as e:
    print("Error:", e)
    color_and_sleep(100, 0, 0, 2)
    


color_and_sleep(0, 0, 0, 0.1)
