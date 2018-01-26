#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "utils/list.h"
#include "utils/console.h"


console *console_new()
{
	console *c = malloc(sizeof(console));
	c->tokens = list_new();
	c->prompt = ">>>"; // Default prompt

	return c;
}

void console_setprompt(console *c,uint8_t *new_prompt)
{
	c->prompt = new_prompt;
}

void console_getline(console *c,uint8_t *buff,uint32_t length)
{
	printf("%s ",c->prompt);
	fgets(buff,length,stdin);
}

list *console_tokenize(console *c)
{
	uint8_t buff[CONSOLE_LINE_SIZE];
	uint8_t token[CONSOLE_LINE_SIZE];
	uint8_t *ptr = &buff;
	uint32_t index = 0;

	// Cleaning our list
	list_empty(c->tokens);

	console_getline(c,buff,CONSOLE_LINE_SIZE-1);
	
	while(*ptr != '\x00' && *ptr != '\n'){
		if(*ptr != ' ' && *ptr != '\n'){
			token[index] = *ptr;
			index += 1;
			ptr++;
		}else{
			if(index != 0){
				token[index] = '\x00';
				list_append(c->tokens,node_new(strdup(token)));
				index = 0;
				ptr++;
			}else{
				ptr++;
			}
		}
	}

	if(index > 0){
		token[index] = '\x00';
		list_append(c->tokens,node_new(strdup(token)));
	}

	return c->tokens;
}
