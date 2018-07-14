# Min-VM : Small virtual machine

This repo contains the virtual machine that runs min bytecode.It can run the bytecode produced by min-cc.

## Usage

```
usage: min-vm [--run/--debug/--tracing] <file>

        --run   : Simply runs the specified binary
        --debug : Run the program with debug output
        --tracing : Run the program with debug + trace
```

## Assemble a .min file
If you want for whatever reason to compile some min assembly, you can use the compiler [here](https://github.com/SiD3W4y/min-cc)

## Instruction coverage
* r = reg
* a = addr
* n = number
* Instructions :
    * [x] **add** {r} {r,n} 
    * [x] **sub** {r} {r,n} 
    * [x] **mul** {r} {r,n}
    * [x] **mov** {r} {r,n,a}
    * [x] **ldr** {r} {r,a}
    * [x] **ldrb** {r} {r,a}
    * [x] **str** {r} {r,a}
    * [x] **strb** {r} {r,a}
    * [x] **push** {r} 
    * [x] **pop** {r}
    * [x] **cmp** {r} {r,n}
    * [x] **jmp** {a}
    * [x] **jne** {a}
    * [x] **je** {a}
    * [x] **jle** {a}
    * [x] **jbe** {a}
    * [x] **sys**
    * [x] **xor** {r} {r,n}
    * [x] **and** {r} {r,n}
    * [x] **or** {r} {r,n}
    * [x] **shr** {r} {r,n}
    * [x] **shl** {r} {r,n}


## Disassembly
You can use the disassembler plugin in the r2-plugin folder to disassemble min bytecode with radare2.
To being able to build it you must first compile the min-vm project. Then build and install the plugin using
the following commands.

```
$ cd r2-plugin/
$ make
$ make install
# use "e asm.arch = min" in r2 or launch r2 with "r2 -a min <file.mx>"
```

Code analysis and binary loading plugins are not implemented for now but will be in the future.
