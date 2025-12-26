CC = gcc
CC_FLAGS = -Wall -Werror -Wextra -std=c11 -lm
C_FILES = $(wildcard *.c)  # Найти все .c файлы в src
INCLUDE_DIR = .  # Путь к заголовочным файлам

TEST_DIR = test
TEST_SUITE_SOURCES = $(wildcard $(TEST_DIR)/*/*_suite.c)
TEST_EXECUTABLES = $(patsubst %.c,%,$(TEST_SUITE_SOURCES))
O_FILES = $(C_FILES:.c=.o)
EXE_FILE = s21_string.out

all: clean test

test: s21_string.a
	$(CC) $(CC_FLAGS) -I$(INCLUDE_DIR) $(TEST_DIR)/string_test.c s21_string.a -lcheck -lsubunit -lm -pthread -o $(EXE_FILE)
	./$(EXE_FILE)

s21_string.a: $(O_FILES)
	ar rcs s21_string.a $(O_FILES)
	ranlib s21_string.a

%.o: %.c
	$(CC) -c $(CC_FLAGS) -I$(INCLUDE_DIR) $< -o $@

gcov_report: s21_string.a $(TEST_DIR)/string_test.o
	$(CC) --coverage $(C_FILES) $(TEST_DIR)/string_test.o -o repor -lcheck -lsubunit -lrt -lpthread -lm
	./repor || true
	lcov -t "Report" -o $(TEST_DIR)/coverage.info -c -d .
	genhtml $(TEST_DIR)/coverage.info --output-directory $(TEST_DIR)/report/
	open $(TEST_DIR)/report/index.html

$(TEST_DIR)/string_test.o: $(TEST_DIR)/string_test.c
	$(CC) -c $(CC_FLAGS) --coverage -I$(INCLUDE_DIR) $(TEST_DIR)/string_test.c -o $(TEST_DIR)/string_test.o

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.out
	rm -f *.info
	rm -f *.gcda *.gcno
	rm -rf $(TEST_DIR)/report
	rm -rf $(TEST_DIR)/*.o
	rm -rf $(TEST_DIR)/*.gcda
	rm -rf $(TEST_DIR)/*.info
	rm -rf $(TEST_DIR)/*.gcno
	rm -f repor