SRCS := ${shell find src -type f -name *.c}
CFLAGS := -std=c17 -g -Wall -Wpedantic -Isrc -Iinc/stb -I/usr/include/freetype2 -fsanitize=address -D_POSIX_C_SOURCE=199309L
LDFLAGS := -lglfw -lGL -lGLEW -lm -lfreetype -lrt
BINARY := bin/text-editor

.PHONY : run debug memcheck clean

run: ${BINARY}
	./$^

debug: ${BINARY}
	lldb $^

memcheck: ${BINARY}
	valgrind --log-file=valgrind-log.txt --leak-check=full  --show-leak-kinds=all --track-origins=yes ./$^

clean:
	rm -f ${BINARY}

${BINARY}: ${SRCS}
	@mkdir -p $(@D)
	clang ${CFLAGS} $^ -o $@ ${LDFLAGS}
