#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <inttypes.h>
#include <termios.h>
#include <stdlib.h>
#include "headers/Shell_2_0.h"
#include "headers/process_control.h"
#include <sys/wait.h>
#include <sys/stat.h>

typedef struct command_parts copa;

extern char conveyor_part[];
extern char train_part[];
extern char or_part[];
extern char background_part[];
extern char and_part[];
extern char output_to_start_part[];
extern char output_to_end_part[];
extern char input_from_part[];
extern char bracket_left_part[];
extern char bracket_right_part[];
extern char quote_part[];
extern char shield_part[];

char **copa_to_cmdline (const copa *t)
{
	int i = 0;
	char **cmdline = (char**)malloc(sizeof(char*) * copa_elements(t) + 1);
	for (; t; t = t->next, i++)
		cmdline[i] = t->part;
	cmdline[i] = NULL;
	return cmdline;
}

size_t copa_elements (const copa *t)
{
	size_t i = 0;
	for (; t; t = t->next, i++);
	return i;
}

int32_t execute_cmdline (char **cmdline, struct termios *ttty)
{
	tcsetattr(0, TCSADRAIN, ttty);
	int32_t main_pid = getpid();
	int32_t main_child = fork();
	if (main_child == -1) {
		perror("Main child fork");
		exit(1);
	}
	if (!main_child) {
		size_t i = 0;
		int32_t pid;
		int32_t conv_init = 1;
		int32_t pipe_fd[2];
		int32_t input;
		size_t start = 0;
		for (; cmdline[i]; i++) {
			if (cmdline[i] == conveyor_part) {
				pipe(pipe_fd);
				pid = fork();
				cmdline[i] = NULL;
				if (pid == -1) {
					perror("Fork");
					exit(1);
				}
				if (!pid) {
					if (conv_init)
						close(pipe_fd[0]);
					else {
						dup2(input, 0);
						close(input);
					}
					dup2(pipe_fd[1], 1);
					close(pipe_fd[1]);
					if (set_redirects((const char**)cmdline + start) == -1) {
						perror("Redirects");
						exit(1);
					}
					execvp((const char*)cmdline[start], (char* const*)cmdline + start);
					perror("Execvp");
					fflush(stderr);
					_exit(EXIT_FAILURE);
				}
				if (conv_init)
					conv_init = 0;
				else
					close(input);
				close(pipe_fd[1]);
				input = pipe_fd[0];
				start = i + 1;
			}
			if (cmdline[i + 1] == NULL) {
				pid = fork();
				if (pid == -1) {
					perror("Fork");
					exit(1);
				}
				if (!pid) {
					if (!conv_init) {
						dup2(input, 0);
						close(input);
					}
					if (set_redirects((const char**)cmdline + start) == -1) {
						perror("Redirects");
						exit(1);
					}
					execvp((const char*)cmdline[start], (char* const*)cmdline + start);
					perror("Execvp");
					fflush(stderr);
					_exit(EXIT_FAILURE);
				}
				if (!conv_init)
					close(input);
			}
		}
		pid_t wr;
		do {
			wr = wait(NULL);
		} while (wr != -1);
		exit(0);
	}
	setpgid(main_child, main_child);
	tcsetpgrp(0, main_child);
	pid_t wr;
	int32_t wstatus;
	do {
		wr = wait(&wstatus);
/*		if (WIFSIGNALED(wstatus))
			printf("Process with ID %d has stoped by signal %d\n", wr, WTERMSIG(wstatus));
		if (WIFEXITED(wstatus))
			printf("Process with ID %d has exited with code %d\n", wr, WEXITSTATUS(wstatus));*/
	} while (wr != main_child && wr != -1);
	tcsetpgrp(0, getpgid(main_pid));
	return 1;
}

int8_t cmdline_has_ (const char *_item, const char **cmdline)
{
	size_t i = 0;
	int8_t item = 0;
	for (; cmdline[i]; i++)
		if (cmdline[i] == _item) item++;
	return item;
}

int32_t set_redirects (const char **cmdline)
{
	size_t i = 0;
	int32_t fd;
	for (; cmdline[i];) {
		if (cmdline[i] == output_to_start_part) {
			cmdline[i] = NULL;
			if (!cmdline[i + 1]) return-1;
			fd = open(cmdline[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0664);
			if (fd == -1)
				return -1;
			dup2(fd, 1);
			close(fd);
		}
		if (cmdline[i] == output_to_end_part) {
			cmdline[i] = NULL;
			if (!cmdline[i + 1]) return-1;
			fd = open(cmdline[i + 1], O_WRONLY | O_CREAT | O_APPEND, 0664);
			if (fd == -1)
				return -1;
			dup2(fd, 1);
			close(fd);
		}
		if (cmdline[i] == input_from_part) {
			cmdline[i] = NULL;
			if (!cmdline[i + 1]) return-1;
			fd = open(cmdline[i + 1], O_RDONLY);
			if (fd == -1)
				return -1;
			dup2(fd, 0);
			close(fd);
		}
		i++;
	}
	return 1;
}
