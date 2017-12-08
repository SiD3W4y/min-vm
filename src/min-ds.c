#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "asm/disas.h"
#include "core/minfile.h"
#include "utils/list.h"
#include "utils/console.h"
#include "utils/log.h"

static uint8_t *usage = "usage : ./min-ds <input file>\n";

static uint8_t *help_cmd = "info           : Displays various file info\n"
			"quit           : Exits the disassembler\n"
			"s <hex offset> : Seeks to the given offset\n";

static void set_prompt_offset(uint8_t *prompt,uint32_t offset)
{
	sprintf(prompt,"\x1b[33m[dis : 0x%08x]>\x1b[0m",offset);
}

static void disassemble(minfile *file,uint32_t offset,uint32_t count)
{
	uint8_t ins_buff[64];
	uint32_t len = 0;

	while(count > 0 && offset < file->size){
		len = ds_disassemble(&file->image[offset],&ins_buff);
		printf("0x%08x :  %s\n",offset,&ins_buff);
		offset += len;
		count -= 1;
	}
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
	
	offset = file->entrypoint;
	set_prompt_offset(&prompt,file->entrypoint);
	console_setprompt(cli,&prompt);
	ops = console_tokenize(cli);

	while(!should_quit){

		if(ops->head != NULL){
			node *cmd = ops->head;

			if(strcmp(cmd->data,"info") == 0){
				printf("Binary size : 0x%08x\n",file->size);
				printf("Entrypoint  : 0x%08x\n",file->entrypoint);
			}

			if(strcmp(cmd->data,"help") == 0){
				printf("%s",help_cmd);
			}

			if(strcmp(cmd->data,"quit") == 0){
				return 0;
			}

			if(strcmp(cmd->data,"disas")){
				uint32_t len;
				node *n = cmd->next;
				if(n != NULL){
					disassemble(file,offset,5);
				}
			}
		}
		
		ops = console_tokenize(cli);
	}

	return 0;
}
