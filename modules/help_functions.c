#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "headers/Shell_2_0.h"
#include "headers/help_functions.h"
#include "headers/input_formatting.h"

typedef struct command_parts copa;

void write_copa (copa *t)
{
	const char s[] = "['";
	const char e[] = "']";
	uint32_t part_size;
	while (t) {
		part_size = size_of_copa_part((const char*)t->part);
		write(1, s, sizeof(s));
		write(1, t->part, part_size);
		write(1, e, sizeof(e));
		t = t->next;
	}
}
