#ifndef LOG_H
#define LOG_H

void log_info(char *fmt,...);
void log_warn(char *fmt,...);
void log_error(char *fmt,...);
void log_syscall(char *fmt,...);
void log_tracing(char *fmt,...);

#endif
