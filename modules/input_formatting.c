#include <stdint.h>
#include "headers/shell_2_0.h"
#include "headers/input_formatting.h"
#include <stdlib.h>
#include <stdio.h>

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
extern uint8_t quote_part[];
extern uint8_t shield_part[];

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

int32_t comp_last_part_for_and (const copa *last)
{
	if (!last || (last->part >= conveyor_part && last->part <= background_part)\
		|| (last->part >= and_part && last->part <= bracket_left_part)) return 1;
	return 0;
}

int32_t comp_last_part_for_or (const copa *last)
{
	if (!last || (last->part >= conveyor_part && last->part <= background_part)\
		|| (last->part >= and_part && last->part <= bracket_left_part)) return 1;
	return 0;
}

int32_t comp_last_part_for_output_to_end (const copa *last)
{
	if (!last || (last->part >= conveyor_part && last->part <= bracket_left_part)) return 1;
	return 0;
}

int32_t comp_last_part_for_new_part (const copa *last, int32_t quote_trigger)
{
	if (!last) return 0;
	if (last->part == bracket_right_part || (last->part == quote_part && quote_trigger)) return 1;
	return 0;
}

void fill_space_buffer (uint8_t *buf, int32_t length)
{
	buf[length--] = '\0';
	for(; length >= 0; length--)
		buf[length] = 0X20;
}

void copa_part_backspace (const uint8_t *part, uint8_t *new_part, int32_t size_for_copy)
{
	for (; size_for_copy >= 0; size_for_copy--)
		new_part[size_for_copy] = part[size_for_copy];
}

int32_t space_part_check (const uint8_t *part)
{
	for (; *part; part++)
		if (*part != 0x20) return 0;
	return 1;
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

int32_t comp_str1_with_str2 (const uint8_t *str1, const uint8_t *str2)
{
	for (; *str1 && *str2; str1++, str2++)
		if (*str1 != *str2) return 0;
	if (!*str1 && !*str2)
		return 1;
	return 0;
}

void copy_str1_to_str2 (const uint8_t *str1, uint8_t *str2, int32_t str1_size) // without '\0' symbol
{
	for (--str1_size; str1_size >= 0; str1_size--)
		str2[str1_size] = str1[str1_size];
}

void pbi_spaceCount_copaLastCompValue (uint8_t *part_buf, uint8_t *space_buf, copa **first\
	, copa **last, uint8_t *control_part1, uint8_t *control_part2, int32_t *pbi\
	, int32_t *space_count, int32_t *input_error_trigger, int32_t reset_space_count_for_andor_parts\
	, int32_t (*comp_last_part_for)(const copa*))
{
	int copa_last_comp_value = 0;
	if (*pbi > 0) {
		create_part_copa((const uint8_t*)part_buf, *pbi, first, last);
		set_buf(part_buf, *pbi, '\0');
		*pbi ^= *pbi;
	}
	if (*space_count) {
		fill_space_buffer(space_buf, *space_count);
		create_part_copa((const uint8_t*)space_buf, *space_count, first, last);
		set_buf(space_buf, *space_count, '\0');
		if (!reset_space_count_for_andor_parts) 
			*space_count ^= *space_count;
		copa_last_comp_value = comp_last_part_for((const copa*)(*last)->prev);
	} else
		copa_last_comp_value = comp_last_part_for((const copa*)(*last));
	if (reset_space_count_for_andor_parts) {
		if (copa_last_comp_value == 1) {
			if (space_count) {
				create_part_control(control_part1, first, last);
				*space_count ^= *space_count;
			} else
				(*last)->part = control_part2;
			return;
		}
		if (copa_last_comp_value == 2)
			(*input_error_trigger)++;
		create_part_control(control_part1, first, last);
		*space_count ^= *space_count;
	}
	if (copa_last_comp_value == 1)
		(*input_error_trigger)++;
	create_part_control(control_part1, first, last);
}
