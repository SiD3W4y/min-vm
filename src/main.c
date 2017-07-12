#include <stdio.h>
#include <stdlib.h>

#include "vm.h"

int main(int argc,char **argv)
{
	vm_state *vm = vm_new();
	vm_load_file(vm,"samples/hello_world.mx");
	int exit_code = vm_execute(vm);

	return 0;
}
