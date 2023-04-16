SRCS := ${shell find src -type f -name *.c}
CFLAGS := -std=c17 -g -Wall -Wpedantic -I src
LDFLAGS := -lglfw -lGL -lGLEW
BINARY := bin/text-editor

.PHONY : run debug memcheck clean

run: ${BINARY}
	./$^

debug: ${BINARY}
	gdb $^

memcheck: ${BINARY}
	valgrind --log-file=valgrind-log.txt --leak-check=full  --show-leak-kinds=all --track-origins=yes ./$^
#

clean:
	rm -f ${BINARY}

${BINARY}: ${SRCS}
	@mkdir -p $(@D)
	gcc ${CFLAGS} $^ -o $@ ${LDFLAGS}
