#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "command.h"
#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

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

command * command_parser (char * commande){
  int clen=strlen(commande);
  char * tmp = malloc(sizeof(char)*clen);
  memmove(tmp,commande,sizeof(char)*clen);
  tmp[clen]='\0';
  char *strToken = strtok(tmp," ");
  command *res = command_new(strToken,MAX_ARGS_NUMBER);
  strToken=strtok(NULL," ");
  int i=0;
  while(strToken!= NULL){
    int len=strlen(strToken);
    res->args[i]=malloc(sizeof(char)*(len+1));
    memmove(res->args[i],strToken,sizeof(char)*len);
    res->args[i][len]='\0';
    strToken=strtok(NULL," ");
    i++;
  }
  free(tmp);
  return res;
}

void command_print(command * cmd) {
	printf("Command name : %s\n", cmd -> name);
	for (int i = 0; i < cmd -> arg_capacity; i++) {
		printf("\tArgument[%d] = %s\n", i, cmd -> args[i]);
	}
}
