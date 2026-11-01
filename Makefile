CC ?= cc
CPPFLAGS = -D_POSIX_C_SOURCE=200809L -Iinclude
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
LDFLAGS ?=

APP = file-manager
TEST_APP = test/file_manager_tests
CORE = src/file_manager.c
UI = ui/terminal_ui.c
APP_SRC = $(CORE) src/main.c $(UI)
TEST_SRC = $(CORE) test/test_file_manager.c

.PHONY: all test test-asan debug clean

all: $(APP)

$(APP): $(APP_SRC) include/file_manager.h include/terminal_ui.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $(APP_SRC)

$(TEST_APP): $(TEST_SRC) include/file_manager.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $(TEST_SRC)

test: $(TEST_APP)
	./$(TEST_APP)

test-asan:
	$(CC) $(CPPFLAGS) -std=c11 -g -O1 -Wall -Wextra -Wpedantic -fsanitize=address,undefined -fno-omit-frame-pointer -o $(TEST_APP) $(TEST_SRC)
	ASAN_OPTIONS=detect_leaks=1 ./$(TEST_APP)

debug:
	$(MAKE) CFLAGS="-std=c11 -O0 -g3 -Wall -Wextra -Wpedantic -Wconversion -Wshadow"

clean:
	rm -f $(APP) $(TEST_APP)
