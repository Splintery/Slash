#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "command.h"
#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096
#define SEPARATOR " \t\n"

command * command_new(int max_arg){
	command * res = malloc(sizeof(command));
	res -> args = malloc(max_arg * sizeof(char*));
	res -> arg_capacity = max_arg;
	res -> length = 0;
	return res;
}

void command_delete(command * cmd) {
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
  command *res = command_new(MAX_ARGS_NUMBER);
  res->args[0]=malloc(sizeof(char)*(strlen(strToken)+1));
  memmove(res->args[0],strToken,sizeof(char)*(strlen(strToken)+1));
  int i=1;
  res->length=1;
  while((strToken=strtok(NULL,SEPARATOR))!=NULL){
    int len=strlen(strToken);
    res->args[i]=malloc(sizeof(char)*(len+1));
    memmove(res->args[i],strToken,sizeof(char)*(len+1));
		i++;
    res->length=i;
  }
  res -> args[res -> length] = (char *) NULL;
  free(tmp);
  return res;
}

void command_print(command * cmd) {
	for (int i = 0; i < cmd->length; i++) { // temporaire pour que les tests soient lisibles
		printf("\tArgument[%d] = %s\n", i, cmd -> args[i]);
	}
}

commandList *init_list(){ //crée une liste avec la commande nulle en premier élément (peut-être à modifier par la suite)
    commandList *l = malloc(sizeof(commandList));
    if (l == NULL){
        return NULL;
    } else {
        l->cmd = *(command_new(MAX_ARGS_NUMBER));
        l->suivant = NULL;
        return l;
    }
}

commandList *addToList(commandList *l,command cmd){ // Pour ajouter une commande à notre liste
    while (l->suivant != NULL){
        l = l->suivant;
    }
    commandList *new = malloc(sizeof(commandList));
    if (new == NULL){
        return NULL;
    } else {
        new->cmd = cmd;
        new->suivant = NULL;
        l->suivant = new;
        return l;
    }
}

void freeList(commandList *l){ //Pour détruire notre liste après coup
    while (l != NULL){
        commandList *suivant = l->suivant;
        command_delete(&(l->cmd));
        free(l->redirection);
        free(l);
        l = suivant;
    }
}


