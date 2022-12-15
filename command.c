#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "command.h"
#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096
#define SEPARATOR " \t\n"

command * command_new(char * str,int max_arg){
	command * res = malloc(sizeof(command));
	int len = strlen(str);
	res -> name = malloc(sizeof(char) * (len+1));
	memmove(res->name,str,sizeof(char)*(len+1));
	res -> args = malloc(max_arg * sizeof(char*));
	res -> arg_capacity = max_arg;
	res -> length = 0;
	return res;
}

void command_delete(command * cmd) {
	free(cmd -> name);
	for(int i = 0; i < cmd->length; i++) {
			 free(cmd->args[i]);
	 }
	free(cmd -> args);
	free(cmd);
}
command * command_parser (char * commande){
  int clen=strlen(commande);
  char * tmp = malloc(sizeof(char)*(clen+1));
  memmove(tmp,commande,sizeof(char)*(clen+1));
  char *strToken = strtok(tmp,SEPARATOR);
	if(strToken==NULL){
		strToken="";
	}
  command *res = command_new(strToken,MAX_ARGS_NUMBER);
  int i=0;
  while((strToken=strtok(NULL,SEPARATOR))!=NULL){
    int len=strlen(strToken);
    res->args[i]=malloc(sizeof(char)*(len+1));
    memmove(res->args[i],strToken,sizeof(char)*(len+1));
		i++;
    res->length=i;
  }
  res -> args[res -> length + 1] = (char *) NULL;
  free(tmp);
  return res;
}

void command_print(command * cmd) {
	printf("Command name : %s\n", cmd -> name);
	for (int i = 0; i < cmd->length; i++) { // temporaire pour que les tests soient lisibles
		printf("\tArgument[%d] = %s\n", i, cmd -> args[i]);
	}
}
