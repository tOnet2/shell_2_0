#ifndef HISTORY_CONTROL_H_SENTRY
#define HISTORY_CONTROL_H_SENTRY

int32_t create_history_file (const char *dir, const char *file);
void full_history_stack (int16_t *stack, const int32_t shift, const int32_t stack_size);
int32_t stack_bytes_for_history (const int16_t *stack, int32_t from, int32_t to);
void reset_history (const char *fp1, const char *fp2, const char *fp3, int32_t *fd,\
	int32_t *ex1, int32_t *ex2, int32_t *ex3);

#endif
