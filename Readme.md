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
    * [ ] **ldrb** {r} {r,a}
    * [ ] **str** {r} {r,a}
    * [ ] **strb** {r} {r,a}
    * [ ] **push** {r} 
    * [ ] **pop** {r}
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


