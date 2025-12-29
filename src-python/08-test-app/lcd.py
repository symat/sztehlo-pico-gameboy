# based on https://gist.github.com/phoreglad/b04f5673e62ac4bc1e8cefa8be740249

from micropython import const
import framebuf
from machine import Pin, SPI, PWM
import machine
import time


def color565(red, green=0, blue=0):
    """
    Convert red, green and blue values (0-255) into a 16-bit 565 encoding.
    """
    try:
        red, green, blue = red  # see if the first var is a tuple/list
    except TypeError:
        pass
    return (red & 0xF8) << 8 | (green & 0xFC) << 3 | blue >> 3


BLACK = const(0x0000)
BLUE = const(0x001F)
RED = const(0xF800)
GREEN = const(0x07E0)
CYAN = const(0x07FF)
MAGENTA = const(0xF81F)
YELLOW = const(0xFFE0)
WHITE = const(0xFFFF)


class ST7789(framebuf.FrameBuffer):
    def __init__(
        self,
        screen_width=280,   # this is the actual width of the screen (positioned to the middle of the framebuffer)
        screen_height=240,
        fb_width=320,       # this is the framebuffer width used in the background
        fb_height=240,
        spi=0,
        sck=2,
        mosi=3,
        cs=1,
        dc=14,
        bl=15,
        reset=26,
        init_color=0x0,
        bl_level=128,
        rotation_mode = 3
    ):
        self.spi = None  # init the spi variable so that we can deinit even if SPI constructor fails
        machine.freq(
            125_000_000, 125_000_000
        )  # set the CPU and UART frequency to 125 MHz

        # SPI baudrate can not be higher than the half of the machine frequency (62.5MHz)
        # for higher stability we can decrease it later if needed to 50 or 30 MHz...
        self.spi = SPI(
            spi, 62_500_000, sck=Pin(sck), mosi=Pin(mosi), polarity=1, phase=1
        )

        # BACKLIGHT_FREQ:   22050 [Hz] (use the same frequency as the audio output,
        #                   to reduce interference to the audio output)
        self.bl_pwm = PWM(Pin(bl), freq=22050, duty_u16=bl_level * 256)

        self.rotation_mode = rotation_mode
        self.bl_level = bl_level
        self.cs = Pin(cs, Pin.OUT)
        self.dc = Pin(dc, Pin.OUT)
        self.width = fb_width
        self.height = fb_height
        self.offset_x = int((fb_width - screen_width) / 2)
        self.offset_y = int((fb_height - screen_height) / 2)
        self.bl = bl
        self.reset = Pin(reset, Pin.OUT)
        self.buffer = bytearray(
            fb_width * fb_height * 2
        )  # 2 bytes needed for every pixel (RGB565 format)
        super().__init__(self.buffer, self.width, self.height, framebuf.RGB565)
        self.fill(init_color)
        self.cs.init(self.cs.OUT, value=1)
        self.dc.init(self.dc.OUT, value=0)
        self.reset.init(self.dc.OUT, value=1)
        self.hard_reset()
        self.backlight_on()
        self.init_display()
        self.show()

    def hard_reset(self):
        self.cs(1)
        self.reset(1)
        time.sleep_ms(50)
        self.reset(0)
        time.sleep_ms(50)
        self.reset(1)
        time.sleep_ms(150)

    def init_display(self):
        self.write_cmd(0x01)  # soft reset
        time.sleep_ms(100)
        self.write_cmd(0x11)  # exit sleep mode
        time.sleep_ms(50)
        self.write_reg(0x3A, 0x55)  # Set colour mode to 16 bit, 65K
        time.sleep_ms(10)

        # x_max = self.width - 1
        # y_max = self.height - 1
        # self.write_reg(0x2A, [0, 0, x_max >> 8, x_max & 0xFF])
        # self.write_reg(0x2B, [0, 0, y_max >> 8, y_max & 0xFF])
        
        _rowstart = 0
        _rowend = self.height
        _colstart = 0
        _colend = self.width
        
        self.write_reg(
            0x2A, [_colstart >> 8, _colstart & 0xFF, _colend >> 8, _colend & 0xFF]
        )  # CASET: column addresses
        self.write_reg(
            0x2B, [_rowstart >> 8, _rowstart & 0xFF, _rowend >> 8, _rowend & 0xFF]
        )  # RASET: row addresses

        self.write_cmd(0x21)  # Inversion on: 0x21, Inversion off: 0x20
        time.sleep_ms(10)

        self.write_reg(0xB0, [0x00, 0b11101000])  # RAMCTRL
        time.sleep_ms(10)

        self.rotation(self.rotation_mode)

        self.write_cmd(0x13)  # Normal display on
        time.sleep_ms(10)

        self.display_on()

    def show(self):
        self.write_reg(0x2C, self.buffer)

    def display_on(self):
        self.write_cmd(0x29)  # Main screen turn on
        time.sleep_ms(10)

    def display_off(self):
        self.write_cmd(0x28)  # Main screen turn off
        time.sleep_ms(10)

    def rotation(self, mode):
        """
        Setting the rotation mode:
                                       0: Portrait
                                       1: Landscape
                                       2: Inverted Portrait
                                       3: Inverted Landscape
        """
        # MADCTL
        #
        # MADCTL_MY   0x80  // B7: page address order (0=top to bottom, 1=bottom to top)
        # MADCTL_MX   0x40  // B6: column address order (0=left to right, 1=right to left)
        # MADCTL_MV   0x20  // B5: page/column order (0=normal mode, 1=reverse mode)
        # MADCTL_ML   0x10  // B4: line address order (LCD refresh 0=top to bottom, 1=bottom to top)
        # MADCTL_RGB  0x08  // B3: RGB/BGR order (0=RGB, 1=BGR)
        # MADCTL_MH   0x04  // B2: display latch order (LD refresh 0=left to right, 1=right to left)
        #
        # rotation modes:
        # 	                 0x00 -->	0: Portrait
        # 	                 0x60 -->	1: Landscape (ST7789_MADCTL_MX + ST7789_MADCTL_MV)
        # 	                 0xc0 -->	2: Inverted Portrait (ST7789_MADCTL_MY + ST7789_MADCTL_MX)
        # 	                 0xa0 -->	3: Inverted Landscape (ST7789_MADCTL_MY + ST7789_MADCTL_MV)        #
        if mode == 0:
            self.write_reg(0x36, 0x00)
        elif mode == 1:
            self.write_reg(0x36, 0x60)
        elif mode == 2:
            self.write_reg(0x36, 0xC0)
        else:
            self.write_reg(0x36, 0xA0)

    def backlight_on(self):
        self.bl_pwm.duty_u16(self.bl_level * 256)

    def backlight_off(self):
        self.bl_pwm.duty_u16(0)

    def backlight_level(self, level):
        """
        setting brightness level: 0...255
        """
        self.bl_level = level
        self.backlight_on()

    def load_img_file(self, filepath, startX, startY, width, height, a=-1):
        with open(filepath, "rb") as f:
            for y in range(height):
                tmp_buf = framebuf.FrameBuffer(
                    bytearray(f.read(width * 2)), width, 1, framebuf.RGB565
                )
                self.blit(tmp_buf, startX, startY + y, a)

    def write_cmd(self, cmd):
        self.dc(0)
        self.cs(0)
        if isinstance(cmd, bytes) or isinstance(cmd, list):
            self.spi.write(bytearray(cmd))
        elif isinstance(cmd, bytearray):
            self.spi.write(cmd)
        else:
            self.spi.write(bytearray([cmd]))
        self.cs(1)

    def write_data(self, data):
        self.dc(1)
        self.cs(0)
        if isinstance(data, bytes) or isinstance(data, list):
            self.spi.write(bytearray(data))
        elif isinstance(data, bytearray):
            self.spi.write(data)
        else:
            self.spi.write(bytearray([data]))
        self.cs(1)

    def write_reg(self, reg, data):
        self.write_cmd(reg)
        self.write_data(data)

    def deinit(self):
        self.fill(BLACK)
        self.show() # upload black screen to LCD memory, so on next start it init from black
        self.dc(1)
        self.cs(1)
        self.backlight_off()
        self.bl_pwm.deinit()
        if self.spi != None:
            self.spi.deinit()
                        
    def pixel(self, x, y, c=None):
        return super().pixel(x + self.offset_x, y + self.offset_y, c)

    def hline(self, x, y, w, c):
        super().hline(x + self.offset_x, y + self.offset_y, w, c)

    def vline(self, x, y, h, c):
        super().hline(x + self.offset_x, y + self.offset_y, h, c)

    def line(self, x1, y1, x2, y2, c):
        super().line(x1 + self.offset_x, y1 + self.offset_y, x2 + self.offset_x, y2 + self.offset_y, c)

    def rect(self, x, y, w, h, c, f=False):
        super().rect(x + self.offset_x, y + self.offset_y, w, h, c, f)

    def ellipse(self, x, y, xr, yr, c, f=False, m=0x0f):
        super().ellipse(x + self.offset_x, y + self.offset_y, xr, yr, c, f, m)

    def text(self, s, x, y, c=WHITE):
        super().text(s, x + self.offset_x, y + self.offset_y, c)

    def poly(self, x, y, coords, c, f=False):
        super().poly(x + self.offset_x, y + self.offset_y, coords, c, f)

    def blit(self, fbuf, x, y, key=-1, palette=None):
        super().blit(fbuf, x + self.offset_x, y + self.offset_y, key, palette)

