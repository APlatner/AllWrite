SRCS := ${shell find src -type f -name *.c}
CFLAGS := -std=c17 -g -Wall -Wpedantic -I src
LDFLAGS := 
BINARY := bin/text-editor

.PHONY : run debug memcheck clean

run: ${BINARY}
	./$^

debug: ${BINARY}
	gdb $^

memcheck: ${BINARY}
	valgrind --log-file=valgrind-log.txt --leak-check=full --track-origins=yes --show-leak-kinds=all ./${BINARY}

clean: ${BINARY}
	rm -f $^

${BINARY}: ${SRCS}
	gcc ${CFLAGS} $^ -o $@ ${LDFLAGS}
