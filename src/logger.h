/*
	Copyright 2023 SentientCloud24

	 Licensed under the Apache License, Version 2.0 (the "License");
	 you may not use this file except in compliance with the License.
	 You may obtain a copy of the License at

			 http://www.apache.org/licenses/LICENSE-2.0

	 Unless required by applicable law or agreed to in writing, software
	 distributed under the License is distributed on an "AS IS" BASIS,
	 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	 See the License for the specific language governing permissions and
	 limitations under the License.
*/

#pragma once

#include "result.h"

result_t logger_startup(const char *filepath);
void logger_shutdown(void);

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
