#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
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



bool simplifyPath(char *path){
  int clen=strlen(path);
  char * tmp = malloc(sizeof(char)*(clen+1));
  memmove(tmp,path,sizeof(char)*(clen+1));
  char **pathParse = malloc(sizeof(char*)*MAX_ARGS_NUMBER);

  char *strToken = strtok(tmp,"/");
    int i=0;
	if(strToken!=NULL){
		  pathParse[i]=malloc(sizeof(char)*(strlen(strToken)+1));
      memmove(pathParse[i],strToken,sizeof(char)*(strlen(strToken)+1));
      i++;
  }
  bool unvalid = false;
  while((strToken=strtok(NULL,"/"))!=NULL){
    int len=strlen(strToken);
    if(strcmp(strToken,"..")==0){
      if(i>0){
        i--;
        free(pathParse[i]);
      }else{
        unvalid=true;
      }
    }else if(strcmp(strToken,".")!=0){
      pathParse[i]=malloc(sizeof(char)*(len+1));
      memmove(pathParse[i],strToken,sizeof(char)*(len+1));
  		i++;
    }
  }
  if(!unvalid){
    if(i==0){
      path[0]='\0';
    }else{
      path[0]='/';
      memmove(&path[1],pathParse[0],(strlen(pathParse[0])+1)*sizeof(char));
      for(int j=1;j<i;j++){
        int len = strlen(path);
        path[len]='/';
        memmove(&path[len+1],pathParse[j],(strlen(pathParse[j])+1)*sizeof(char));
      }
    }
  }
  for(int k=0;k<i;k++){
    free(pathParse[k]);
  }
  free(pathParse);
  free(tmp);
  return unvalid;
}

// change le répertoire courant (pas encore complète)
int my_cd(char * dest, char * option, char cwd[PATH_MAX]){

    int chd;
    if (strcmp(option, "-L") == 0){
        char *chemin;
        if (strcmp(dest, "-") == 0){
          chemin = getenv("OLDPWD");
          setenv("OLDPWD",getenv("PWD"),1);
          chd = chdir(chemin);

        }else{
          setenv("OLDPWD",getenv("PWD"),1);
          char *pwd=getenv("PWD");
          int lenDest=strlen(dest);
          if(dest[0]!='/'){
            int lenPwd=strlen(pwd);
            chemin=malloc(sizeof(char)*(lenPwd+lenDest+2));
            memmove(chemin,pwd,sizeof(char)*lenPwd);
            chemin[lenPwd]='/';
            memmove(&chemin[lenPwd+1],dest,sizeof(char)*(lenDest+1));
          }else{
            chemin=malloc(sizeof(char)*(lenDest+1));
            memmove(chemin,dest,sizeof(char)*(lenDest+1));
          }
          bool invalidPath=simplifyPath(chemin);
          if(invalidPath){
            free(chemin);
            return my_cd(dest,"-P",cwd);
          }
          if(strlen(chemin)==0){
            chd = chdir(getenv("HOME"));
          }else{
            chd = chdir(chemin);
          }
        }
        if (chd == 0){
            if(getcwd(cwd, PATH_MAX) == NULL) {
              if(strcmp(dest, "-")!=0){
                free(chemin);
              }
                return 1;
            }
            setenv("PWD",chemin,1);
            if(strcmp(dest, "-")!=0){
              free(chemin);
            }
            return 0;
        } else {
          if(strcmp(dest, "-")!=0){
            free(chemin);
          }
          return my_cd(dest,"-P",cwd);
        }
    } if (strcmp(option, "-P") == 0){
      if (strcmp(dest, "-") == 0){

        chd=chdir(getenv("OLDPWD"));
        setenv("OLDPWD",getenv("PWD"),1);
      }else{
        setenv("OLDPWD",getenv("PWD"),1);

        chd = chdir(dest);
      }
      if (chd == 0){
          if(getcwd(cwd, PATH_MAX) == NULL) {
              return 1;
          }
          setenv("PWD",cwd,1);
          return 0;
      } else {
          return 1;
      }
    }
    return 1;
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
  bool exitB = false;
  // return_value sera la valeur de retour renvoyé lorsqu'une commande sera executé
  // du style "cd" ou "pwd", (ce qui entre crochet dans le prompt)
  int return_value = 0;
  char cwd[PATH_MAX];
  rl_outstream = stderr;

  // On récupère le répertoire courrant pour le stocker dans "cwd"
  // Je ne sais pas encore comment "cd" affectera le répertoire courrant
  // Mais si "cd" est bien fait, alors "pwd" n'aura qu'a print le contenu de cwd
  if (getcwd(cwd, sizeof(cwd))== NULL) {
    error_message = "getcwd() error";
    return_value = 1;
    goto error;
  }
  do {
    struct string * prompt = prompt_line(return_value, getenv("PWD"));
    // Pour le moment les commandes sont stockés dans "user_entry"
    // on ajoute au buffer user_entry le résultat de readline
    char * user_entry = readline(prompt->data);
    if(user_entry==NULL){
      exit(0);
    }
    if(strlen(user_entry)>0){
      add_history(user_entry); // Puis on ajoute la ligne à l'historique des commandes.
    }

    command *cmd = command_parser(user_entry);
    if(strcmp(cmd->name,"exit")==0){
      exitB=true;
      if(cmd->length==1){
        char test_number[100];
        int exit_value=atoi(cmd->args[0]);
        sprintf(test_number,"%d",exit_value);
        if(strcmp(test_number,cmd->args[0])==0) {
            return_value=exit_value;
        } else {
            return_value=2;
        }
      }else if(cmd->length>1){
        return_value=2;
      }
    }else if(strcmp(cmd->name,"pwd")==0){
      if((cmd->length==0)||strcmp(cmd->args[0],"-L")==0){
        return_value=my_pwd(cwd,getenv("PWD"),"-L");
      }else{
        return_value=my_pwd(cwd,getenv("PWD"),cmd->args[0]);
      }
      if(cmd->length>1){
        return_value=1;
      }
    }else if(strcmp(cmd->name,"cd")==0){
      //TODO implémenter cd
      switch(cmd->length){
        case 0 : return_value = my_cd(getenv("HOME"),"-L",cwd);  break;
        case 1 : return_value = my_cd(cmd->args[0], "-L", cwd); break;
        case 2 : return_value = my_cd(cmd->args[1], cmd->args[0], cwd); break;
        default : return_value = 1;
      }
    }else{
      int pipes[2];
      pipe(pipes);
      int fork_value = fork();
      if (fork_value == 0) {
        close(pipes[0]);
        // cmd->length +2 because we count the NULL at the end + we will add the name of the
        // command in front
        int len = cmd->length + 2;
        char ** arguments_formatted = malloc (sizeof(char *) * len);
        if (arguments_formatted != NULL) {
          arguments_formatted[0] = cmd->name;
          for (int i = 1; i < len; i++) {
            arguments_formatted[i] = cmd->args[i - 1];
          }
          int res = execvp(cmd->name, arguments_formatted);

          write(pipes[1], &res, sizeof(res));
        }
      } else {
        close(pipes[1]);
        int tmp;
        read(pipes[0], &tmp, sizeof(tmp));
        waitpid(fork_value, NULL, 0);
        return_value = tmp;
      }
    }
    string_delete(prompt);
    free(user_entry);
    command_delete(cmd);

  } while(!exitB);
  return return_value;

  error :
    if (errno) {perror(error_message);}
    return return_value;
}
