#ifndef WILDCARDS_H
#define WILDCARDS_H

#define MAX_NAME_LENGTH 15
#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

#include "command.h"

void addArgsAt(int index, command* cmd, char** args, int num_args);

void expand_wildcard(command* cmd,int argindex ,const char* path);

#endif