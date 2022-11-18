#include <stdlib.h>
#include <stdio.h>
#include "command.h"

struct command * command_new(int max_arg){
	struct command * res = malloc(sizeof(struct command));
	res -> name = malloc(sizeof(char) * MAX_NAME_LENGTH);
	res -> args = malloc(max_arg * sizeof(char *));
	res -> arg_capacity = max_arg;
	return res;
}

void command_delete(struct command * cmd) {
	free(cmd -> name);
	free(cmd -> args);
	free(cmd);
}

void command_print(struct command * cmd) {
	printf("Command name : %s\n", cmd -> name);
	for (int i = 0; i < cmd -> arg_capacity; i++) {
		printf("\tArgument[%d] = %s\n", i, cmd -> args[i]);
	}
}