#ifndef COMMAND_H
#define COMMAND_H

#define MAX_NAME_LENGTH 15
#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096

typedef struct command {
	char * name; // après l'appel à command_parser, name recevra le contenu du premier pointeur de char
	char ** args; // et args recevra ceux après
	int arg_capacity;
	int length;
} command ;
// Dans un "switch" plus tard ou pourras donc comparer command->name avec une commande que nous avons
// Implétmentée puis l'executer avec ce qui se trouve dans args

command * command_new(char *str,int max_arg);

void command_delete(command * cmd);
// Détruit la commande et libère la mémoire

command * command_parser (char * commande);
// crée une struct command à partir d'une chaine de caractères.

void command_print(command * cmd);

#endif
