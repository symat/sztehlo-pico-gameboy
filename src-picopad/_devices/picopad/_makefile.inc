CSRC += ${PICOLIBSDK_DEVICES_DIR}/picopad/picopad_bat.c
CSRC += ${PICOLIBSDK_DEVICES_DIR}/picopad/picopad_key.c
CSRC += ${PICOLIBSDK_DEVICES_DIR}/picopad/picopad_led.c
CSRC += ${PICOLIBSDK_DEVICES_DIR}/picopad/picopad_init.c
CSRC += ${PICOLIBSDK_DEVICES_DIR}/picopad/picopad_ss.c

DEFINE += -D USE_PICOPAD=1

DEFINE += -D USE_PICOPAD20=1
MCU=RP2350A
FLASHSIZE=0x400000
