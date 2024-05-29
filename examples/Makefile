CC = gcc
SOURCES = int_channels.c struct_channels.c
TARGETS = $(SOURCES:.c=)

all: $(TARGETS)

%: %.c
	$(CC) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean
