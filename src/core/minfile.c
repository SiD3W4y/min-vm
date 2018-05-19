#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/minfile.h"
#include "utils/memory.h"

minfile *minfile_new()
{
	minfile *f = malloc(sizeof(minfile));
	return f;
}


// TODO : Add logging when encountering errors
int32_t minfile_load(minfile *file, char *path)
{
	FILE *fp;
	
	fp = fopen(path,"rb");

	if(fp == NULL){
		return MINFILE_ERROR;
	}

	fseek(fp,0,SEEK_END);
	file->size = ftell(fp);
	rewind(fp);

	if(file->size < MINFILE_HEADER_SIZE){
		return MINFILE_ERROR;
	}

	file->image = malloc(file->size);

	if(file->image == NULL){
		return MINFILE_ERROR;
	}

	fread(file->image,file->size,1,fp);
	fclose(fp);
	
	if(file->image[0] != 'M' && file->image[0] != 'X'){
		return MINFILE_ERROR;
	}
	
	file->entrypoint = MEM_GET_U32(&file->image[2]);

	return MINFILE_SUCCESS;
}
