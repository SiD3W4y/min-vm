#ifndef CONSOLE_H
#define CONSOLE_H

#include "utils/list.h"

#define CONSOLE_LINE_SIZE 256

typedef struct console {
	list *tokens;
	uint8_t line[CONSOLE_LINE_SIZE];
	uint8_t *prompt;
}console;

console *new_console();
void console_setprompt(console *c,uint8_t *new_prompt);
uint8_t *console_getline(console *c); // Returns a line from stdin
list *console_tokenize(console *c); // Returns a list of space separated tokens
void console_free(console *c);

#endif
