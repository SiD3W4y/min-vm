#include <stdlib.h>

#include "utils/list.h"
#include "utils/console.h"


console *console_new()
{
	console *c = malloc(sizeof(console));
	c->tokens = list_new();

	return c;
}

void console_setprompt(console *c,uint8_t *new_prompt)
{
	c->prompt = new_prompt;
}
