#ifndef SHELL_2_0_H_SENTRY
#define SHELL_2_0_H_SENTRY

typedef struct command_parts {
	char *part;
	struct command_parts *next, *prev;
} copa;

#endif
