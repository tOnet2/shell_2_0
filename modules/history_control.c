#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "headers/Shell_2_0.h"
#include "headers/history_control.h"

void reset_history (const char *fp1, const char *fp2, const char *fp3, int32_t *fd,\
	int32_t *ex1, int32_t *ex2, int32_t *ex3)
{
	unlink(fp1);
	unlink(fp2);
	unlink(fp3);
	close(*fd);
	*fd = 0;
	*ex1 = *ex2 = *ex3 ^= *ex3;
}

int32_t create_history_file (const char *dir, const char *file)
{
	if (access(file, F_OK) == 0) {
		return 1;
	} else {
		int32_t dir_exists = mkdir(dir, 0775);
		if (dir_exists == 0 || (dir_exists == -1 && errno == EEXIST)) {
			int32_t fd_hist;
			if ((fd_hist = open(file, O_WRONLY | O_CREAT, 0664)) >= 0) {
				close(fd_hist);
				return 1;
			} else
				return 0;
		} else
			return 0;
	}
}

void full_history_stack (int16_t *stack, const int32_t shift, const int32_t stack_size)
{
	int i;
	for (i = 0; i < stack_size - shift; i++)
		stack[i] = stack[i + shift];
	for (; i < stack_size; i++)
		stack[i] = 0;
}

int32_t stack_bytes_for_history (const int16_t *stack, int32_t from, int32_t to)
{
	int32_t i, sum;
	sum = 0;
	for (i = from; i < to; i++)
		sum += stack[i] + 1;
	return sum;
}
