
#include<stdio.h>
#include<stdarg.h>
#include<assert.h>


static int NUM_OF_CALLING = 0;
static FILE* LOG_FILE = stderr;
static int USING_PRINT_TO_LOG_AFTER_FORGET_LOG = 0;


void remember_log_file(FILE* log_file = nullptr)
{
    NUM_OF_CALLING++;
    if(log_file != nullptr)
        LOG_FILE = log_file;
     if(NUM_OF_CALLING > 1)
     {
        fprintf(LOG_FILE, "WARNING: You write remember_log_file() twice - the information about your file will be lost if you will do it\n");
        NUM_OF_CALLING = 1;
     }
}

void forget_log_file()
{
    if(NUM_OF_CALLING <= 0)
        assert(!"You use forget_log_file() twice - you already close your log file or you use forget_log_file()"
               " without remember_log_file()");
    if(LOG_FILE != stderr)
    {
        fclose(LOG_FILE);
        USING_PRINT_TO_LOG_AFTER_FORGET_LOG = 1;
    }
    NUM_OF_CALLING--;
    LOG_FILE = stderr;
}

int print_to_log(const char* format, ...)
{
    if(USING_PRINT_TO_LOG_AFTER_FORGET_LOG) {
        printf("WARNING: you use print_to_log() after you used forget_log_file() "
                "the log file has been configured as stderr by default\n");
        USING_PRINT_TO_LOG_AFTER_FORGET_LOG = 0;
    }

    va_list arg_ptr;
    va_start(arg_ptr, format);
    int num_of_char_printed = vfprintf(LOG_FILE, format, arg_ptr);
    va_end(arg_ptr);
    return num_of_char_printed;
}
