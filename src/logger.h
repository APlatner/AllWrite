#pragma once

#include "result.h"

result_t logger_startup(const char *filepath);
void logger_shutdown();

void fatal(char *message);
void error(char *message);
void warn(char *message);
void info(char *message);

void _debug(char *file, int line, char *message, ...);
void _trace(char *file, int line, char *message);

#ifndef NDEBUG
#define debug(...) _debug(__FILE__, __LINE__, __VA_ARGS__)
#define trace(message) _trace(__FILE__, __LINE__, message)
#else
#define debug(...)
#define trace(message)
#endif
