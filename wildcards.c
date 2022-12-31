#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdbool.h>

#include "mystring.h"
#include "command.h"
#include "wildcards.h"

void addArgsAt(int index, command* cmd, char** args, int num_args) {
  if (index < 0 || index > cmd->length) {
    perror("addArgs");
    exit(1);
  }
  for (int i = cmd->length-1; i >= index; i--) {
      cmd->args[i + num_args] = cmd->args[i];
  }

  for (int i = 0; i < num_args; i++) {
    int len=strlen(args[i]);
    char *tmp=malloc(sizeof(char)*(len+1));
    memmove(tmp,args[i],sizeof(char)*(len+1));
    cmd->args[index+i]=tmp;
  }
  cmd->length += num_args;
  cmd->args[cmd->length]=NULL;
}

void replace_wildcard(char* wildcard,DIR* current_dir,int index,char* path,command * cmd,int argindex){
  // Read all the entries in the current directory
  struct dirent* entry;
  char* pathcpy=malloc(sizeof(char)*(strlen(path)+1));
  memmove(pathcpy,path,sizeof(char)*(strlen(path)+1));
  while ((entry = readdir(current_dir)) != NULL) {
    if (strlen(entry->d_name)>0 && entry->d_name[0]=='.' ) {
      continue;
    }
    int lenSuffixe=strlen(wildcard)-1;
    int lenEntry=strlen(entry->d_name);
    if(lenSuffixe==0||(strcmp(&wildcard[1],&(entry->d_name[lenEntry-lenSuffixe]))==0)){
      struct stat st;
      char *tmp = malloc(sizeof(char)*(index+lenEntry+1));
      memmove(tmp,pathcpy,sizeof(char)*index);
      memmove(&tmp[index],entry->d_name,sizeof(char)*lenEntry);
      tmp[index+lenEntry]='\0';
      if(stat(tmp,&st)<0){
        perror("stat1");
        exit(1);
      }
      free(tmp);
      if(!S_ISDIR(st.st_mode)&&(((int)strlen(pathcpy))-(lenSuffixe+1)+ lenEntry + 1)>(index+ lenEntry + 1)){
        continue;
      }
      // Allocate memory for the new path
      
      char* new_path = malloc(sizeof(char)*(strlen(pathcpy) -(lenSuffixe+1)+ lenEntry + 1)); 
      // Replace the wildcard with the entry name
      memmove(new_path, pathcpy, sizeof(char)*index);
      memmove(&new_path[index], entry->d_name,sizeof(char)*lenEntry);
      memmove(&new_path[index + lenEntry], &pathcpy[index + lenSuffixe + 1],sizeof(char)*( strlen(pathcpy)-index-(lenSuffixe+1)+1));
      expand_wildcard(cmd,argindex,new_path);
      free(new_path);
    } 
  }
  free(pathcpy);
}

void expand_wildcard(command* cmd,int argindex ,char* path) {
  char** paths = malloc(sizeof(char*)*MAX_ARGS_NUMBER);
  //chemins qu'on va ajouter à la commande.
  int num_paths = 0;
  //on utilise un index pour savoir où on en est dans le chemin.
  int index=0;

  char* copy = strdup(path);
  char* part = strtok(copy, "/");
  DIR* current_dir = opendir(".");

  if(path[0]=='/'){
    current_dir=opendir("/");
    index++;
  }

  while (part != NULL) {
    // On vérifie si part contient une wild card, * ou ** et on appelle les fonctions permettant de les remplacer.
    // Sinon, on vérifie si part est un répertoire ou un fichier valide.
    if (strcmp(part, "**") == 0) {
      //TODO implémenter la double étoile
    }else if (part[0]=='*') {
      replace_wildcard(part,current_dir,index,path,cmd,argindex);
      break;
    }else{
      //Si part ne contient pas d'étoile, on regarde dans le répertoire où l'on est si il existe.
      index+=strlen(part);
      struct stat st;
      struct dirent* entry;
      bool containsPart=false;
      while ((entry = readdir(current_dir)) != NULL) {
        if(strcmp(entry->d_name,part)==0){
          containsPart=true;
        }
      }
      if(containsPart==false){
        break;
      }
      //Si il existe, on vérifie avec stat si c'est un répertoire ou non.
      char *tmp = malloc(sizeof(char)*MAX_ARGS_STRLEN);
      memmove(tmp,path,sizeof(char)*index);
      tmp[index]='\0';
      if(stat(tmp,&st)<0){
        perror("stat");
        exit(1);
      }
      //Si il reste des choses dans le chemin, si ce n'est pas un répertoire, on sort de la boucle, si c'en est un,
      // on change le répertoire courant et on passe au suivant.
      if(((strlen(path))>strlen(tmp))){
        if(!S_ISDIR(st.st_mode)){
          free(tmp);
          break;
        }else{
          closedir(current_dir);
          current_dir=opendir(tmp);
          int slashs=0;
          while(path[index+slashs]=='/'){
            slashs++;
          }
          if(slashs==1){
            index++;
          }else if(slashs>1){
            int lenpath=strlen(path);
            memmove(&path[index+1],&path[index+slashs],sizeof(char)*strlen(&path[index+slashs]));
            path[lenpath-(slashs-1)]='\0';
            index++;
          }
          free(tmp);
          part = strtok(NULL, "/");
        }
      }else{
        //Si le chemin est terminé, on ajoute le chemin obtenu aux chemins à ajouter à la commande.
        paths[num_paths]=tmp;
        num_paths++;
      }
    }
  }  
  closedir(current_dir);
  free(copy);

  paths[num_paths] = NULL;
  //On ajoute tous les chemins aux arguments de cmd.
  if(num_paths > 0){   
    addArgsAt(argindex+1,cmd,paths,num_paths);
  }
  for(int i=0;i<num_paths;i++){
    free(paths[i]);
  }
  free(paths);
}