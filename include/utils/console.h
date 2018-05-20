#ifndef CONSOLE_H
#define CONSOLE_H

#include "utils/list.h"

#define CONSOLE_LINE_SIZE 256

typedef struct console {
    list *tokens;
    char line[CONSOLE_LINE_SIZE];
    char *prompt;
}console;

console *console_new();
void console_setprompt(console *c,char *new_prompt);
void console_getline(console *c,char *buff,uint32_t length); // Returns a line from stdin
list *console_tokenize(console *c); // Returns a list of space separated tokens
void console_free(console *c);

#endif
