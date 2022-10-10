#include <stdint.h>
#include "headers/shell_2_0.h"
#include "headers/input_formatting.h"
#include <stdlib.h>

typedef struct command_parts copa;

extern uint8_t conveyor_part[];
extern uint8_t train_part[];
extern uint8_t or_part[];
extern uint8_t background_part[];
extern uint8_t and_part[];
extern uint8_t output_to_start_part[];
extern uint8_t output_to_end_part[];
extern uint8_t input_from_part[];
extern uint8_t bracket_left_part[];
extern uint8_t bracket_right_part[];

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

uint32_t size_of_copa_part (const uint8_t *part) // without '\0' symbol
{
	uint32_t size = 0;
	for(; *part; part++, size++);
	return size;
}

void create_part_control (uint8_t *buf, copa **first, copa **last)
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

void create_part_copa (const uint8_t *buf, int32_t size, copa **first, copa **last)
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
	if (!last || (last->part >= conveyor_part && last->part <= bracket_right_part)) return 1;
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

void fill_space_buffer (uint8_t *buf, int32_t length)
{
	buf[length--] = '\0';
	for(; length >= 0; length--)
		buf[length] = 0X20;
}

void free_copa (copa *t)
{
	while (t) {
		copa *tmp = t;
		t = t->next;
		if (tmp->part > bracket_right_part || tmp->part < conveyor_part)
			free(tmp->part);
		free(tmp);
	}
}

int32_t comp_str1_with_str2 (const uint8_t *str1, const uint8_t *str2)
{
	for (; *str1 && *str2; str1++, str2++)
		if (*str1 != *str2) return 0;
	if (!*str1 && !*str2)
		return 1;
	return 0;
}
