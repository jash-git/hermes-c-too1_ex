CC ?= cc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic
LDFLAGS ?= -lm

TARGET = build/hermes-calc

.PHONY: all clean test

all: $(TARGET)

$(TARGET): src/hermes_calc.c
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -rf build

test: $(TARGET)
	@test "$$(./$(TARGET) sum 1 2 3)" = '{"ok":true,"operation":"sum","count":3,"result":6}'
	@test "$$(printf '2 4 8\n' | ./$(TARGET) avg)" = '{"ok":true,"operation":"avg","count":3,"result":4.666666666666667}'
	@test "$$(./$(TARGET) min -2 5 1)" = '{"ok":true,"operation":"min","count":3,"result":-2}'
	@! ./$(TARGET) nope 1 >/dev/null 2>&1
	@echo 'tests: ok'
