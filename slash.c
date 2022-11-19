#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
// Ce n'est pas nécessaire, mais sympa pour rendre le code plus lisible
#include <stdbool.h>

#include "mystring.h"
#include "command.h"

#define MAX_ARGS_NUMBER 4096
#define MAX_ARGS_STRLEN 4096
// Pour le moment ce n'est utilisé que pour stocker le chemin courrant
#define PATH_MAX 64
#define WD_MEMORY 50 // Pour stocker les répertoires courants précédents
// Si jamais on veux changer la taille du prompt
#define PROMPT_LENGTH 52

struct string * prompt_line(int ret, char * path){
  if(ret<0||ret>99999999){
    return prompt_line(1,path);
  }
  struct string* res = string_new(PROMPT_LENGTH);
  if(ret==0){
    string_append(res,"\001\033[32m\002[");
  }else {
    string_append(res,"\001\033[91m\002[");
  }
  char ret_string[10];
  sprintf(ret_string,"%d",ret);
  string_append(res,ret_string);
  string_append(res,"]\001\033[34m\002");

  size_t path_len = strlen(path);
  if((res->length-14)+path_len>28){
    string_append(res,"...");
    struct string* tmp= string_new(path_len+1);
    string_append(tmp,path);
    string_truncate_from_the_beginning(tmp,path_len-(28-(res->length-14)));
    string_append(res,tmp->data);
    string_delete(tmp);
  }else{
    string_append(res,path);
  }
  string_append(res,"\001\033[00m\002$ ");
  return res;

}

//Cette fonction affiche le répertoire courant réel si l'option est -P,
//et affiche le chemin "logique" si l'option est -L.
int my_pwd(char * cwd, char * pwd, char * option){
  if(strcmp(option,"-P")==0){
    printf("%s\n",cwd);
    return 0;
  }else if((strcmp(option,"-L")==0)){
    printf("%s\n",pwd);
    return 0;
  }
    return 1;
}
// change le répertoire courant (pas encore complète)
int my_cd(char * dest, char * option, char memo[WD_MEMORY][PATH_MAX], char cwd[PATH_MAX]){
    int chd;
    int i = 0;
    /*while(memo[i] =! NULL ){
        i++;
    }*/
    if (strcmp(dest, "-") == 0){
        chd = chdir(memo[i - 1]);
        if (chd == 0){
            if(getcwd(cwd, PATH_MAX) == NULL) {
                return 1;
            }
            //memo[i - 1] = NULL;
            return 0;
        }
    }
    if (strcmp(option, "-L") == 0){
        chd = chdir(dest);
        if (chd == 0){
            if(getcwd(cwd, PATH_MAX) == NULL) {
                return 1;
            }
            //memo[i] = cwd;
            return 0;
        } else {
            return 1;
        }
    } if (strcmp(option, "-P") == 0){
        return 1;
    }
    return 1;
}

command * command_parser (char * commande){
  int clen=strlen(commande);
  char * tmp = malloc(sizeof(char)*clen);
  memmove(tmp,commande,sizeof(char)*clen);
  tmp[clen]='\0';
  char *strToken = strtok(tmp," ");
  command *res = command_new(strToken,MAX_ARGS_NUMBER);
  strToken=strtok(NULL, " ");
  int i=0;

  while(strToken!= NULL){
    int len=strlen(strToken);
    res->args[i]=malloc(sizeof(char)*(len+1));
    memmove(res->args[i],strToken,sizeof(char)*len);
    res->args[i][len]='\0';
    strToken=strtok(NULL, " ");
    i++;
  }
  free(tmp);
  return res;
}

//Tests temporaires
void test() {
  //test prompt
  char *test = "test";
  struct string * ligne = prompt_line(1,test);
  printf("%s\n",ligne->data);
  // J'aime beaucoup ce chemin de fichier
  char *test2 = "troplong/troptroplong/supermegalong/olalalala";
  struct string * ligne2 = prompt_line(0,test2);
  printf("%s\n",ligne2->data);

  //test command_parser
  char * commande = "pwd -L";
  command *commande_parsee=command_parser(commande);
  int i=0;
  printf("name : %s\n",commande_parsee->name);
  while(commande_parsee->args[i]!=NULL){
  printf("arg %d : %s\n",i,commande_parsee->args[i]);
  i++;
}


}

int main(){
  char * error_message = "main";
  //test();

  // On set up des variables pour la suite
  bool exit = false;
  // return_value sera la valeur de retour renvoyé lorsqu'une commande sera executé
  // du style "cd" ou "pwd", (ce qui entre crochet dans le prompt)
  int return_value = 0;
  char cwd[PATH_MAX];
  char memo [WD_MEMORY][PATH_MAX];
  char * user_entry;
  rl_outstream = stderr;

  do {

    // On récupère le répertoire courrant pour le stocker dans "cwd"
    // Je ne sais pas encore comment "cd" affectera le répertoire courrant
    // Mais si "cd" est bien fait, alors "pwd" n'aura qu'a print le contenu de cwd
    if (getcwd(cwd, sizeof(cwd))== NULL) {
      error_message = "getcwd() error";
      return_value = 1;
      goto error;
    }
    struct string * prompt = prompt_line(return_value, cwd);
    // Pour le moment les commandes sont stockés dans "user_entry"
    // on ajoute au buffer user_entry le résultat de readline
    user_entry = readline(prompt->data);
    if(strlen(user_entry)>0){
      add_history(user_entry); // Puis on ajoute la ligne à l'historique des commandes.
    }

    command * cmd = command_parser(user_entry);
    if(strcmp(cmd->name,"exit")==0){
      exit=true;
      return_value=0;
    }else if(strcmp(cmd->name,"pwd")==0){
      if(cmd->args[0]==NULL||(strcmp(cmd->args[0],"-L")==0)){
        my_pwd(getenv("PWD"),cwd,"-L");
      }else{
        my_pwd(getenv("PWD"),cwd,"-P");
      }
    }else if(strcmp(cmd->name,"cd")==0){
      //TODO implémenter cd
    }else{
      // TODO : Pour le moment les commandes sont juste recrachées dans le terminal
      if (write(1, user_entry, strlen(user_entry)) < 0) {
        error_message = "write() error";
        return_value = 1;
        goto error;
      }
    }
    string_delete(prompt);
    command_delete(cmd);

  } while(!exit);
  free(user_entry);

  return return_value;

  error :
    if (errno) {perror(error_message);}
    if (user_entry != NULL) {free(user_entry);}
    return return_value;
}
