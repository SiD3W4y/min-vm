# Min-VM : Small virtual machine

This repo contains the virtual machine that runs min bytecode.It can run the bytecode produced by min-cc.

## Usage

```
usage: min-vm [--run/--debug] <file>

        --run   : Simply runs the specified binary
        --debug : Run the program with debug output
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
    * [x] **mov** {r} {r,n}
    * [x] **ldr** {r} {a}
    * [ ] **ldrb** {r} {a}
    * [ ] **str** {r} {a}
    * [ ] **strb** {r} {a}
    * [ ] **push** {r} 
    * [ ] **pop** {r}
    * [ ] **cmp** {r} {r,n}
    * [x] **jmp** {a}
    * [ ] **jne** {a}
    * [ ] **je** {a}
    * [ ] **jle** {a}
    * [ ] **jbe** {a}
    * [x] **sys**
    * [ ] **xor** {r} {r,n}
    * [ ] **and** {r} {r,n}
    * [ ] **or** {r} {r,n}
    * [ ] **shr** {r} {r,n}
    * [ ] **shl** {r} {r,n}


