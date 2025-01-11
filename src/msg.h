#ifndef MSG_H_
#define MSG_H_

typedef enum msg_type
{
    MSG_DEBUG,
    MSG_INFO,
    MSG_WARNING,
    MSG_ERROR,
    MSG_FATAL
} msg_type;

#ifdef __cplusplus
extern "C"
{
#endif
void msg_set_level(msg_type level);
void msg(msg_type type, const char* message, ...);
#ifdef __cplusplus
}
#endif

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


#endif
