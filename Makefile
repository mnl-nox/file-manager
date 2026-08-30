CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -Iinclude

APP = file-manager
TEST_APP = test/file_manager_tests

SRC = src/file_manager.c src/main.c ui/terminal_ui.c
TEST_SRC = src/file_manager.c test/test_file_manager.c

.PHONY: all test clean

all: $(APP)

$(APP): $(SRC)
	$(CC) $(CFLAGS) -o $@ $(SRC)

$(TEST_APP): $(TEST_SRC)
	$(CC) $(CFLAGS) -o $@ $(TEST_SRC)

test: $(TEST_APP)
	./$(TEST_APP)

clean:
	rm -f $(APP) $(TEST_APP)
