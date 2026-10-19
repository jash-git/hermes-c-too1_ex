CC ?= cc
CFLAGS ?= -O2 -Wall -Wextra -std=c11
SRC := src/filestat.c
BIN := bin/filestat

.PHONY: all clean install

all: $(BIN)

$(BIN): $(SRC)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf bin

install: $(BIN)
	install -d ~/.hermes/skills/filestat
	install -m 0755 $(BIN) ~/.hermes/skills/filestat/filestat
	install -m 0644 skill/filestat/SKILL.md ~/.hermes/skills/filestat/SKILL.md