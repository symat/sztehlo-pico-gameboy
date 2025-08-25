
ASRC += ${PICOLIBSDK_SDK_DIR}/crt0_rp2040.S


ASRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_divider.S
ASRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_memops_asm.S
ASRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_double_m0.S
ASRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_float_m0.S


ifeq (${OS},Windows_NT)
CSRC += ${PICOLIBSDK_SDK_DIR}/sdk.c
else
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_adc.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_bootrom.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_clocks.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_cpu.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_dma.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_double.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_fatal.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_fifo.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_flash.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_float.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_gpio.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_hstx.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_i2c.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_interp.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_irq.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_multicore.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_pio.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_pll.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_powman.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_psram.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_pwm.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_qmi.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_qspi.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_reset.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_rosc.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_rtc.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_runtime.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_spi.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_spinlock.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_ssi.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_systick.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_ticks.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_timer.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_uart.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_vreg.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_watchdog.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_xip.c
CSRC += ${PICOLIBSDK_SDK_DIR}/src/sdk_xosc.c

CSRC += ${PICOLIBSDK_SDK_DIR}/usb_src/sdk_usbport.c

CSRC += ${PICOLIBSDK_SDK_DIR}/usb_src/sdk_usb_dev.c
CSRC += ${PICOLIBSDK_SDK_DIR}/usb_src/sdk_usb_dev_cdc.c
CSRC += ${PICOLIBSDK_SDK_DIR}/usb_src/sdk_usb_dev_hid.c
CSRC += ${PICOLIBSDK_SDK_DIR}/usb_src/sdk_usb_host.c
CSRC += ${PICOLIBSDK_SDK_DIR}/usb_src/sdk_usb_host_cdc.c
CSRC += ${PICOLIBSDK_SDK_DIR}/usb_src/sdk_usb_host_hid.c
CSRC += ${PICOLIBSDK_SDK_DIR}/usb_src/sdk_usb_phy.c
endif
