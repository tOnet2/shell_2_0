#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "../shell_2_0.h"

typedef struct command_parts copa;

void print_copa (copa *t)
{
	while (t) {
		printf("%s\n", t->part);
		t = t->next;
	}
	while (t) {
		printf("%s\n", t->part);
		t = t->prev;
	}
}
