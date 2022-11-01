#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "headers/Shell_2_0.h"
#include "headers/history_control.h"

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
