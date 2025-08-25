CSRC += ${PICOLIBSDK_DEVICES_DIR}/picopad/picopad_bat.c
CSRC += ${PICOLIBSDK_DEVICES_DIR}/picopad/picopad_key.c
CSRC += ${PICOLIBSDK_DEVICES_DIR}/picopad/picopad_led.c
CSRC += ${PICOLIBSDK_DEVICES_DIR}/picopad/picopad_init.c
CSRC += ${PICOLIBSDK_DEVICES_DIR}/picopad/picopad_ss.c

DEFINE += -D USE_PICOPAD=1

ifeq (${DEVICE},picopad10)
DEFINE += -D USE_PICOPAD10=1
MCU=RP2040
FLASHSIZE=0x200000
endif

