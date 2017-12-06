#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "asm/disas.h"
#include "core/minfile.h"
#include "utils/list.h"
#include "utils/console.h"
#include "utils/log.h"

static uint8_t *usage = "usage : ./min-ds <input file>\n";

static void set_prompt_offset(uint8_t *prompt,uint32_t offset)
{
	sprintf(prompt,"\x1b[33m[dis : 0x%08x]>\x1b[0m",offset);
}

int main(int argc,char **argv)
{
	uint8_t ins_buff[64];
	uint8_t buff[64];
	uint8_t prompt[64];
	uint32_t offset = 0;
	bool should_quit = false;
	
	list *ops;
	console *cli = console_new();
	minfile *file = minfile_new();

	if(argc != 2){
		log_error("First parameter must be a path\n");
		printf(usage);
		return -1;
	}

	if(minfile_load(file,argv[1]) == MINFILE_ERROR){
		log_error("There was an error opening the input file -> %s\n",argv[1]);
		return -1;
	}

	log_info("-- File info --\n");
	log_info("Binary size : 0x%08x\n",file->size);
	log_info("Entrypoint  : 0x%08x\n",file->entrypoint);

	set_prompt_offset(&prompt,file->entrypoint);
	console_setprompt(cli,&prompt);
	ops = console_tokenize(cli);

	while(!should_quit){
		ops = console_tokenize(cli);

		if(ops->head != NULL){
			node *cmd = ops->head;

			if(strcmp(cmd->data,"print") == 0){
				printf("WTF!!!\n");
			}
		}
	}

	return 0;
}
