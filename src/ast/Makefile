CC=gcc

#CFLAGS=-pedantic -Werror -Wall -Wextra -std=c99 -fsanitize=address

#CPPFLAGS=-D_DEFAULT_SOURCE -D_POSIX_SOURCE

#OBJS='parser.o'

TESTS = test_astconvert

.PHONY: check
check: $(TESTS)
	for t in $(TESTS); do \
		./$$t; \
	done;

test_%: %.o parser.o stringutils.o tests/test_%.o
		$(CC) -o $@ $^ -lcriterion

clean:
	rm -rf $(TESTS)
