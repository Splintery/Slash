#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
// Ce n'est pas nécessaire, mais sympa pour rendre le code plus lisible
#include <stdbool.h>

#include "mystring.h"

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
    //écriture temporaire dans le terminal
    if(write(1,cwd,strlen(cwd))<0){
      return 1;
    }
    return 0;
  }else if((strcmp(option,"-L")==0)){
    if(write(1,pwd,strlen(pwd))<0){
      return 1;
    };
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
//Tests temporaires
void test() {
  char *test = "test";
  struct string * ligne = prompt_line(1,test);
  printf("%s\n",ligne->data);
  // J'aime beaucoup ce chemin de fichier
  char *test2 = "troplong/troptroplong/supermegalong/olalalala";
  struct string * ligne2 = prompt_line(0,test2);
  printf("%s\n",ligne2->data);
}

int main(){
  char * error_message = "main";
  //test();

  // On set up des variables pour la suite
  bool exit = false;
  int read_result;
  // return_value sera la valeur de retour renvoyé lorsqu'une commande sera executé
  // du style "cd" ou "pwd", (ce qui entre crochet dans le prompt)
  int return_value = 0;
  char cwd[PATH_MAX];
  char memo [WD_MEMORY][PATH_MAX];
  char * user_entry = malloc(sizeof(char) * MAX_ARGS_STRLEN);
  if (user_entry == NULL) {
    error_message = "malloc() for user_entry error";
    return_value = 1;
    goto error;
  }

  do {

    // On récupère le répertoire courrant pour le stocker dans "cwd"
    // Je ne sais pas encore comment "cd" affectera le répertoire courrant
    // Mais si "cd" est bien fait, alors "pwd" n'aura qu'a print le contenu de cwd
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
      // affichage du prompt
      struct string * prompt = prompt_line(return_value, cwd);
      if (write(1, prompt -> data, prompt -> length) < 0) {
        error_message = "prompt_line print error";
        return_value = 1;
        goto error;
      }
    } else {
      error_message = "getcwd() error";
      return_value = 1;
      goto error;
    }



    // Pour le moment les commandes sont stockés dans "user_entry"
    read_result = read(0, user_entry, MAX_ARGS_STRLEN);
    if (read_result < 0) {
      error_message = "read() error";
      return_value = 1;
      goto error;
    }

    // À changer (pour évaluer les commandes):
    // C'est juste pour pouvoir sortir de la boucle
    if (strstr(user_entry, "exit") != NULL) {
      //strstr(user_entry, "exit") check si "exit" est contenu dans user_entry et retourne l'index si c'est le cas, NULL sinon
      printf("We are exiting !\n");
      exit = true;
    }

    // TODO : Pour le moment les commandes sont juste recrachées dans le terminal
    if (write(1, user_entry, read_result) < 0) {
      error_message = "write() error";
      return_value = 1;
      goto error;
    }

  } while(!exit && read_result > 0);

  return return_value;

  error :
    if (errno) {perror(error_message);}
    if (user_entry != NULL) {free(user_entry);}
    return return_value;
}
