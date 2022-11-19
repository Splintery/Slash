#ifndef COMMAND_H
#define COMMAND_H

#define MAX_NAME_LENGTH 15

typedef struct command {
	char * name; // après l'appel à command_parser, name recevra le contenu du premier pointeur de char
	char ** args; // et args recevra ceux après
	int arg_capacity;
} command ;
// Dans un "switch" plus tard ou pourras donc comparer command->name avec une commande que nous avons
// Implétmentée puis l'executer avec ce qui se trouve dans args

command * command_new(char *str,int max_arg);

void command_delete(command * cmd);
// Détruit la commande et libère la mémoire

void command_print(command * cmd);

#endif
