ifeq (${OS},Windows_NT)
CSRC += ${PICOLIBSDK_DISPLAY_DIR}/display.c
else
CSRC += ${PICOLIBSDK_DISPLAY_DIR}/st7789/st7789.c
endif
