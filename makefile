CC        := gcc
CFLAGS    := -Wall -Werror
SRC_FILES := $(wildcard *.c) $(wildcard */*.c)
CPPFLAGS  := -I./include
OUT_FILE  := cws_exec

$(OUT_FILE):
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SRC_FILES) -o $(OUT_FILE)

clean:
	rm -rf $(OUT_FILE)
