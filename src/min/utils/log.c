#include <stdio.h>
#include <stdarg.h>

#include "min/utils/log.h"

static void log_raw(char* title,char* color,char *fmt,va_list args)
{
    printf("%s%s%s - ",color,title,"\x1b[0m");
    vprintf(fmt,args);
}

void log_info(char *fmt,...)
{
    va_list args;
    va_start(args,fmt);
    log_raw("INFO","\x1b[32m",fmt,args);
}

void log_warn(char *fmt,...)
{
    va_list args;
    va_start(args,fmt);
    log_raw("WARNING","\x1b[33m",fmt,args);
}

void log_error(char *fmt,...)
{
    va_list args;
    va_start(args,fmt);
    log_raw("ERROR","\x1b[31m",fmt,args);
}


void log_syscall(char *fmt,...)
{
    va_list args;
    va_start(args,fmt);
    log_raw("SYSCALL","\x1b[36m",fmt,args);
}

void log_tracing(char *fmt,...)
{
    va_list args;
    va_start(args,fmt);
    log_raw("TRACING","\x1b[94m",fmt,args);
}
