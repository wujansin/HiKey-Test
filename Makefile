# Linux
PCSC_CFLAGS := $(shell pkg-config --cflags libpcsclite)
LDLIBS := $(shell pkg-config --libs libpcsclite)

CFLAGS += $(PCSC_CFLAGS)

test_pcsc: test_pcsc.c

clean:
	rm -f test_pcsc
