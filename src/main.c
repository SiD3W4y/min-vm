#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"

int main(int argc,char **argv)
{
	char *usage = "usage: min-vm [--run/--debug] <file>\n\n"
		"	--run   : Simply runs the specified binary\n"
		"	--debug : Run the program with debug output\n";
	char *mode;
	vm_state *vm = vm_new();

	if(argc != 3){
		printf(usage);
		return -1;
	}

	mode = argv[1];

	if(strcmp(mode,"--run") == 0 || strcmp(mode,"--debug") == 0){
		if(strcmp(mode,"--debug")){
			vm->debug = 1;
		}

		vm_load_file(vm,argv[2]);
		vm_execute(vm);
	}else{
		printf("%s : Not a valid flag !\n",mode);
		return -1;
	}

	return 0;
}
