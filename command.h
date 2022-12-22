#ifndef COMMAND_H
#define COMMAND_H

#define MAX_NAME_LENGTH 15
#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

typedef struct command {
	char ** args; 
	int arg_capacity;
	int length;
} command ;
// Dans un "switch" plus tard ou pourras donc comparer command->[0] avec une commande que nous avons
// implémentée puis l'executer avec ce qui se trouve dans args

typedef struct commandList { // Pour implémenter le 1er wildcard et permettre l'exécution de toutes les commandes possibles
    command cmd;
	char * redirection;
	int length;
    struct commandList *suivant; // NULL si plus de commande à tenter d'exécuter
} commandList;

command * command_new(int max_arg);

void command_delete(command * cmd);
// Détruit la commande et libère la mémoire

command * command_parser (char * commande);
// crée une struct command à partir d'une chaine de caractères.

void command_print(command * cmd);

commandList *init_list();

commandList *addToList(commandList *l,command cmd);

void freeList(commandList *l);

#endif
