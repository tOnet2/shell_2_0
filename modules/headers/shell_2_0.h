#ifndef SHELL_2_0_H_SENTRY
#define SHELL_2_0_H_SENTRY

typedef struct command_parts {
	uint32_t cps, cpe;			
	uint8_t *part;
	/*
	 * cps - cursor position start
	 * cpe - cursor position end
	 */
	struct command_parts *next, *prev;
} copa;

#endif
