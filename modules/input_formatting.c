#include <stdint.h>
#include "headers/Shell_2_0.h"
#include "headers/input_formatting.h"
#include <stdlib.h>
#include <stdio.h>

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

void clean_read_buf (char *buf, int32_t size)
{
	for (; size; size--, buf++)
		*buf = 0;
}

void set_buf (char *buf, int32_t size, char c)
{
	for(; size; size--, buf++)
		*buf = c;
}

uint32_t size_of_copa_part (const char *part) // without '\0' symbol
{
	uint32_t size = 0;
	for(; *part; part++, size++);
	return size;
}

void create_part_control (char *buf, copa **first, copa **last)
{
	copa *tmp = malloc(sizeof(*tmp));
	tmp->part = buf;
	tmp->next = NULL;
	if (*last) {
		(*last)->next = tmp;
		tmp->prev = *last;
		*last = tmp;
	} else {
		tmp->prev = NULL;
		*first = *last = tmp;
	}

}

void create_part_copa (const char *buf, int32_t size, copa **first, copa **last)
{
	copa *tmp = malloc(sizeof(*tmp));
	tmp->part = malloc(size + 1);
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

int32_t comp_last_part_for_background (const copa *last)
{
	if (!last || (last->part >= conveyor_part && last->part <= or_part)\
		|| (last->part >= and_part && last->part <= bracket_left_part)) return 2;
	if (last->part == background_part) return 1;
	return 0;
}

int32_t comp_last_part_for_train (const copa *last)
{
	if (!last || (last->part >= conveyor_part && last->part <= or_part)\
		|| (last->part >= and_part && last->part <= bracket_left_part)) return 1;
	return 0;
}

int32_t comp_last_part_for_conveyor (const copa *last)
{
	if (!last || (last->part >= train_part && last->part <= or_part)\
		|| (last->part >= and_part && last->part <= bracket_left_part)) return 2;
	if (last->part == conveyor_part) return 1;
	return 0;
}

int32_t comp_last_part_for_bracket_left (const copa *last)
{
	if (!last || last->part == conveyor_part || last->part == train_part\
		|| last->part == and_part || last->part == bracket_left_part) return 0;
	return 1;
}

int32_t comp_last_part_for_bracket_right (const copa *last)
{
	if (!last || (last->part >= conveyor_part && last->part <= bracket_left_part)) return 1;
	return 0;
}

int32_t comp_last_part_for_input_from (const copa *last)
{
	if (!last || (last->part >= conveyor_part && last->part <= bracket_left_part)) return 1;
	return 0;
}

int32_t comp_last_part_for_output_to_start (const copa *last)
{
	if (!last || (last->part >= conveyor_part && last->part <= and_part)\
		|| (last->part >= output_to_end_part && last->part <= bracket_left_part)) return 2;
	if (last->part == output_to_start_part) return 1;
	return 0;
}

void free_copa (copa *t)
{
	while (t) {
		copa *tmp = t;
		t = t->next;
		if (!(tmp->part >= conveyor_part && tmp->part <= shield_part))
			free(tmp->part);
		free(tmp);
	}
}

void middle_backspace_for_buf (char *buf, int32_t from, int32_t to)
{
	for (; from < to; from++)
		*(buf + from - 1) = *(buf + from);
	*(buf + from - 1) = 0;
}

void middle_backword_for_buf (char *buf, int32_t new_pos, int32_t from, int32_t to)
{
	for (; from < to;)
		*(buf + new_pos++) = *(buf + from++);
	for (; new_pos < to;) {
		*(buf + new_pos++) = 0;
	}
}

void middle_del_for_buf (char *buf, int32_t from, int32_t to)
{
	for (; from < to; from++)
		*(buf + from) = *(buf + from + 1);
}

void middle_insert_for_buf (char *buf, int32_t from, int32_t to)
{
	*(buf + to + 1) = 0;
	for (; to > from; to--)
		*(buf + to) = *(buf + to - 1);
}

void cp_buf1tobuf2 (const char *buf1, char *buf2)
{
	while(*buf1)
		*buf2++ = *buf1++;
	*buf2 = 0;
}

void clear_buf (char *buf)
{
	for (; *buf;)
		*buf++ = 0;
}

int32_t cmp_buf1withbuf2 (const char *buf1, const char *buf2)
{
	for (; *buf1 && *buf2; buf1++, buf2++)
		if (*buf1 != *buf2) return 0;
	if (!*buf1 && !*buf2) return 1;
	return 0;
}

int32_t size_buf (const char *buf)
{
	int32_t size = 0;
	for (; *buf; buf++)
		size++;
	return size;
}
