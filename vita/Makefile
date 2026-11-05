# Initial VitaSDK scaffold. Verify toolchain/library names against the installed VitaSDK environment.
TARGET = neonbrawl_vita
TITLE = Neon Brawl Vita Prototype

SOURCES = src
INCLUDES =

# vitaSDL2 supplies SDL2 for the Vita homebrew toolchain.
LIBS = -lSDL2 -lSceDisplay_stub -lSceGxm_stub -lSceCtrl_stub -lSceSysmodule_stub

PREFIX = arm-vita-eabi
CC = $(PREFIX)-gcc
CFLAGS = -Wl,-q -Wall -O2

all: $(TARGET).velf

$(TARGET).velf: $(SOURCES)/main.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@ $(LIBS)

clean:
	rm -f $(TARGET).velf $(TARGET).vpk