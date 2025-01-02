#ifndef MSG_H_
#define MSG_H_

#include "config.h"

typedef enum msg_type
{
    MSG_DEBUG,
    MSG_INFO,
    MSG_WARNING,
    MSG_ERROR,
    MSG_FATAL
} msg_type;

void msg_set_level(msg_type level);
void msg(msg_type type, const char* message, ...);

#define _STRING_LITERAL(a) #a
#define STRING_LITERAL(a) _STRING_LITERAL(a)

#define FILE_LINE() __FILE__ ":" STRING_LITERAL(__LINE__)

#define ERROR(str, ...) msg(MSG_ERROR, str " - " FILE_LINE(), __VA_ARGS__)
#define FATAL(str, ...) msg(MSG_FATAL, str " - " FILE_LINE(), __VA_ARGS__)
#define DEBUG_MSG(str, ...) msg(MSG_DEBUG, str " - " FILE_LINE(), __VA_ARGS__)

#ifdef DEBUG
#define UNIMPLEMENTED(str) FATAL("Unimplemented code reached: '%s'", str)
#else
#define UNIMPLEMENTED(str) static_assert(false, "Unimplemented code in non-debug build: '" str "'")
#endif

#undef assert
#define assert(cond, ...) ( \
	(cond) ? (void)0 		\
		 : msg(MSG_FATAL, 	\
"Assertion '" #cond "' failed - " FILE_LINE() ": " __VA_ARGS__)\
)

#define unreachable(...) \
	msg(MSG_FATAL, "Unreachable code - " FILE_LINE() ": " __VA_ARGS__)

#define shift_args(xs, xs_sz) (  \
    assert((xs_sz) > 0),         \
    (xs_sz)--,                   \
    *(xs)++                      \
)

#ifdef MSG_H_IMPL
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#define COLOUR_RED     "\x1b[31m"
#define COLOUR_YELLOW  "\x1b[33m"
#define COLOUR_BLUE    "\x1b[34m"
#define COLOUR_MAGENTA "\x1b[35m"

#define COLOUR_RESET   "\x1b[0m"

static msg_type log_level = MSG_INFO;

void msg_set_level(msg_type level)
{
    log_level = level;
}

void msg(msg_type type, const char* message, ...)
{
    if (type < log_level) return;
    va_list args;
    va_start(args, message);
    
    const char* prefix;
    const char* colour;

    switch (type)
    {
        case MSG_FATAL:
        case MSG_ERROR:
            prefix = "ERROR: ";
            colour = COLOUR_RED;
            break;
        case MSG_WARNING:
            prefix = "WARNING: ";
            colour = COLOUR_YELLOW;
            break;
        case MSG_INFO:
            prefix = "Info: ";
            colour = COLOUR_BLUE;
            break;
        case MSG_DEBUG:
#ifdef DEBUG
            prefix = "Debug: ";
            colour = COLOUR_MAGENTA;
            break;
#else
			return;
#endif
        default:
            prefix = "";
            colour = COLOUR_RESET;
            break;
    }

    printf("%s%s", colour, prefix);
    vprintf(message, args);
    printf("%s\n", COLOUR_RESET);

    va_end(args);
    if (type == MSG_FATAL)
    {
        exit(1);
    }
}
#endif //IMPLEMENTATION

#endif
