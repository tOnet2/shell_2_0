#ifndef SHELL_2_0_H_SENTRY
#define SHELL_2_0_H_SENTRY

typedef struct command_parts {
	uint8_t *part;
	struct command_parts *next, *prev;
} copa;

#endif
