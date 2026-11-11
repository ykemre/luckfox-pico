# Host-side verification, independent of the RV1103 SDK.
CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -Werror -std=c99
SRC = ../rtp_audio.c ../rtp_audio_sdp.c test_rtp_audio.c
.PHONY: all test clean
all: test_rtp_audio
test_rtp_audio: $(SRC)
	$(CC) $(CFLAGS) -I.. $(SRC) -o $@
test: test_rtp_audio
	./test_rtp_audio
clean:
	rm -f test_rtp_audio
