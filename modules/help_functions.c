#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "headers/shell_2_0.h"
#include "headers/help_functions.h"
#include "headers/input_formatting.h"

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
