#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "command.h"

command * command_new(char * str,int max_arg){
	command * res = malloc(sizeof(command));
	int len = strlen(str);
	res -> name = malloc(sizeof(char) * (len+1));
	memmove(res->name,str,sizeof(char)*len);
	res->name[len]='\0';
	res -> args = malloc(max_arg * sizeof(char *));
	res -> arg_capacity = max_arg;
	return res;
}

void command_delete(command * cmd) {
	free(cmd -> name);
	int i = 0;
	while(cmd->args[i]!=NULL){
		free(cmd->args[i]);
		i++;
	}
	free(cmd -> args);
	free(cmd);
}

void command_print(command * cmd) {
	printf("Command name : %s\n", cmd -> name);
	for (int i = 0; i < cmd -> arg_capacity; i++) {
		printf("\tArgument[%d] = %s\n", i, cmd -> args[i]);
	}
}
