#ifndef WILDCARDS_H
#define WILDCARDS_H

#define MAX_NAME_LENGTH 15
#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

#include "command.h"

void addArgsAt(int index, command* cmd, char** args, int num_args);

void replace_wildcard(char* wildcard,DIR* current_dir,int index,char* path,command * cmd,int argindex);

void expand_wildcard(command* cmd,int argindex ,char* path);

#endif