#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "headers/shell_2_0.h"
#include "headers/help_functions.h"

typedef struct command_parts copa;

void write_copa (copa *t)
{
	const uint8_t s[] = "['";
	const uint8_t e[] = "']";
	uint32_t part_size;
	while (t) {
		part_size = size_of_copa_part(t->part);
		write(1, s, sizeof(s));
		write(1, t->part, part_size);
		write(1, e, sizeof(e));
		t = t->next;
	}
}

uint32_t size_of_copa_part (uint8_t *part)
{
	uint32_t size = 0;
	for(; *part; part++)
		size++;
	return size;
}

void change_last_copa_part (uint8_t *part, const uint8_t *new_part, int32_t s_new_part)
{
	free(part);
	part = malloc(s_new_part);
	for (; s_new_part; s_new_part--, part++, new_part++)
		*part = *new_part;
}

int32_t comp_last_copa_part (uint8_t *part, const uint8_t *buf, int32_t s_buf)
{
	int i;
	for (i = 0; s_buf; s_buf--, i++)
		if (part[i] != buf[i]) break;
	if (s_buf == 0)
		return 1;
	if (part[i] == 0x26 || part[i] == 0x3b\
			|| part[i] == 0x3c || part[i] == 0x3e\
			|| part[i] == 0x7c)
		return 2;
	return 0;
}
