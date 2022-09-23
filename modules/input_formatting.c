#include <stdint.h>
#include "headers/shell_2_0.h"
#include "headers/input_formatting.h"
#include <stdlib.h>

typedef struct command_parts copa;

void clean_read_buf (uint8_t *buf, int32_t size)
{
	for (; size; size--, buf++)
		*buf = 0;
}

void set_buf (uint8_t *buf, int32_t size, uint8_t c)
{
	for(; size; size--, buf++)
		*buf = c;
}

void create_part_copa (const uint8_t *buf, int32_t size, copa **first, copa **last)
{
	copa *tmp = malloc(sizeof(*tmp));
	tmp->part = malloc(size + 1);
//	set_buf(tmp->part, size--, '\0');
	tmp->part[size--] = '\0';
	tmp->next = NULL;
	for (; size >= 0; size--)
		tmp->part[size] = buf[size];
	if (*last) {
		(*last)->next = tmp;
		tmp->prev = *last;
		*last = tmp;
	} else {
		tmp->prev = NULL;
		*first = *last = tmp;
	}
}

void free_copa (copa *t)
{
	while (t) {
		copa *tmp = t;
		t = t->next;
		free(tmp->part);
		free(tmp);
	}
}

void change_last_copa_part (uint8_t *part, const uint8_t *new_part, int32_t s_new_part)
{
	free(part);
	part = malloc(s_new_part);
	for (; s_new_part; s_new_part--, part++, new_part++)
		*part = *new_part;
}

int32_t comp_last_copa_part_for_background_conveyor_train (const copa *t, const uint8_t *buf, int32_t s_buf)
{
	if (!t) return 2;
	int i;
	for (i = 0; s_buf; s_buf--, i++)
		if (t->part[i] != buf[i]) break;
	if (s_buf == 0)
		return 1;
	if (t->part[i] == 0x26 || t->part[i] == 0x3b\
			|| t->part[i] == 0x3c || t->part[i] == 0x3e\
			|| t->part[i] == 0x7c)
		return 2;
	return 0;
}

int32_t check_read_buf_control_symbol(const uint8_t *read_buf, int32_t read_buf_size, uint8_t symbol)
{
	for (; read_buf_size; read_buf_size--, read_buf++)
		if (*read_buf == symbol) return 1;
	return 0;
}
