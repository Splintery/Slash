#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdbool.h>

#include "mystring.h"
#include "command.h"
#include "wildcards.h"

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
  if(ret==255){
    string_append(res,"SIG");
  }else{
    char ret_string[10];
    sprintf(ret_string,"%d",ret);
    string_append(res,ret_string);
    string_append(res,"]\001\033[34m\002");
  }

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

//Les trois fonctions suivantes servent à séparer un chemin contenant un * en 3 parties
//pour l'implémentation du wildcard (pour l'instant juste le *)

char * split1st(char * src){ //renvoie le chemin éventuel avant le wildcard (peut être nul)
    if (src[0] == '*'){
        return "";
    }else {
        char *prems = strtok(src, "*");
        return prems;
    }
}

char * split2nd(char * src){//renvoie le morceau (fichier ou répertoire) qui contient le wildcard
    //on suppose d'office qu'on a bien un * dans le chemin à l'appel de cette fonction
    char * sec = strtok(src, "*");
    sec = strtok(NULL, "/");
    return strcat("*",  sec);
}

char * split3rd(char * src){ //renvoie la fin du chemin (la suite si *... désigne un répertoire)
    char * ter = strtok(src, "*");
    ter = strtok(NULL, "/");
    ter = strtok(NULL, " "); //à voir si pour la dernière itération on ne trouve pas plus joli
    return ter;
}

void exec_cmd(int* return_value,bool* exit,char cwd[PATH_MAX],command* cmd){
  if(strcmp(cmd->args[0],"exit")==0){
      *exit=true;
      if(cmd->length==2){
        char test_number[100];
        int exit_value=atoi(cmd->args[1]);
        sprintf(test_number,"%d",exit_value);
        if(strcmp(test_number,cmd->args[1])==0) {
            *return_value=exit_value;
        } else {
            *return_value=2;
        }
      }else if(cmd->length>2){
        *return_value=2;
      }
    }else if(strcmp(cmd->args[0],"pwd")==0){
      if((cmd->length==1)||strcmp(cmd->args[1],"-L")==0){
        *return_value=my_pwd(cwd,getenv("PWD"),"-L");
      }else{
        *return_value=my_pwd(cwd,getenv("PWD"),cmd->args[1]);
      }
      if(cmd->length>1){
        *return_value=1;
      }
    }else if(strcmp(cmd->args[0],"cd")==0){
      switch(cmd->length){
        case 1 : *return_value = my_cd(getenv("HOME"),"-L",cwd);  break;
        case 2 : *return_value = my_cd(cmd->args[1], "-L", cwd); break;
        case 3 : 
        if(strstr(cmd->args[2],"*")!=NULL){
          expand_wildcard(cmd,2,cmd->args[2]);
        }
        if(cmd->length==3){
          *return_value = my_cd(cmd->args[2], cmd->args[1], cwd);
        }else{
          *return_value = 1;
        } 
        break;
        default : *return_value = 1;
      }
    }else if (strcmp(cmd->args[0], "") != 0){
      // cmd->length +2 because we count the NULL at the end + we will add the name of the
      // command in front
      int fork_value;
      if ((fork_value = fork()) == 0) {
        for(int i = 0;i<cmd->length;i++){
          if(strstr(cmd->args[i],"*")!=NULL){
            expand_wildcard(cmd,i,cmd->args[i]);
          }
        }
        
        execvp(cmd->args[0], cmd->args);
        _exit(1);
      } else {
        int status;
        if (waitpid(fork_value,&status, WUNTRACED | WCONTINUED) == -1) {
          perror("waitpid");
          *return_value = 1;
        } else {
          if (WIFEXITED(status)) {
            *return_value = WEXITSTATUS(status);
          } else {
            *return_value = WSTOPSIG(status);
          }
        }
      }
    } else {
      // La commande vide ne change pas la valeur de "return_value"
      // Donc rien à faire
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
  if (getcwd(cwd, sizeof(cwd))== NULL) {
    error_message = "getcwd() error";
    return_value = 1;
    goto error;
  }
  do {
    struct string * prompt = prompt_line(return_value, getenv("PWD"));
    // on ajoute au buffer user_entry le résultat de readline
    char * user_entry = readline(prompt->data);
    if(user_entry==NULL){
      exit(return_value);
    }
    if(strlen(user_entry)>0){
      add_history(user_entry); // Puis on ajoute la ligne à l'historique des commandes.
    }

    command *cmd = command_parser(user_entry);
    exec_cmd(&return_value,&exitB,cwd,cmd);
    string_delete(prompt);
    free(user_entry);
    command_delete(cmd);

  } while(!exitB);
  exit(return_value);

  error :
    if (errno) {perror(error_message);}
    exit(return_value);
}
