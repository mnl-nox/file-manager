CC ?= cc
CPPFLAGS = -D_POSIX_C_SOURCE=200809L -Iinclude
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic -Wconversion -Wshadow
LDFLAGS ?=

APP = file-manager
TEST_APP = test/file_manager_tests
CORE = src/file_manager.c
UI = ui/terminal_ui.c
APP_SRC = $(CORE) src/command_parser.c src/main.c $(UI)
TEST_SRC = $(CORE) test/test_file_manager.c
PARSER_TEST_APP = test/command_parser_tests
PARSER_TEST_SRC = src/command_parser.c test/test_command_parser.c

.PHONY: all test test-asan debug clean

all: $(APP)

$(APP): $(APP_SRC) include/file_manager.h include/terminal_ui.h include/command_parser.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $(APP_SRC)

$(TEST_APP): $(TEST_SRC) include/file_manager.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $(TEST_SRC)

$(PARSER_TEST_APP): $(PARSER_TEST_SRC) include/command_parser.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $(PARSER_TEST_SRC)

test: $(TEST_APP) $(PARSER_TEST_APP)
	./$(TEST_APP)
	./$(PARSER_TEST_APP)

test-asan:
	$(CC) $(CPPFLAGS) -std=c11 -g -O1 -Wall -Wextra -Wpedantic -fsanitize=address,undefined -fno-omit-frame-pointer -o $(TEST_APP) $(TEST_SRC)
	ASAN_OPTIONS=detect_leaks=1 ./$(TEST_APP)
	$(CC) $(CPPFLAGS) -std=c11 -g -O1 -Wall -Wextra -Wpedantic -fsanitize=address,undefined -fno-omit-frame-pointer -o $(PARSER_TEST_APP) $(PARSER_TEST_SRC)
	ASAN_OPTIONS=detect_leaks=1 ./$(PARSER_TEST_APP)

debug:
	$(MAKE) CFLAGS="-std=c11 -O0 -g3 -Wall -Wextra -Wpedantic -Wconversion -Wshadow"

clean:
	rm -f $(APP) $(TEST_APP) $(PARSER_TEST_APP)
