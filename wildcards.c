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

void addArgsAt(int index, command* cmd, char** args, int num_args) {
  if (index < 0 || index > cmd->length) {
    perror("addArgs");
    exit(1);
  }
  free(cmd->args[index]);

  for (int i = index + 1; i < cmd->length; i++) {
    cmd->args[i + num_args - 1] = cmd->args[i];
  }


  for (int i = 0; i < num_args; i++) {
    cmd->args[index + i] = strdup(args[i]);
  }
  cmd->length += num_args-1;
}


void expand_wildcard(command* cmd,int argindex ,const char* path) {
  // Allocate memory for the result array
  char** paths = malloc(sizeof(char*)*MAX_ARGS_NUMBER);
  int num_paths = 0;
  int index=0;

  // Split the path into its component parts
  char* copy = strdup(path);
  char* part = strtok(copy, "/");

  // Keep track of the current directory
  DIR* current_dir = opendir(".");

  // Iterate over the parts of the path
  while (part != NULL) {
    // Check if the part is a wildcard
    if (strcmp(part, "**") == 0) {
      //TODO implémenter la double étoile
    }else if (part[0]=='*') {
      // Read all the entries in the current directory
      struct dirent* entry;
      while ((entry = readdir(current_dir)) != NULL) {
        // Skip "." and ".." entries
        if (strlen(entry->d_name)>0 && entry->d_name[0]=='.' ) {
          continue;
        }
        int lenSuffixe=strlen(part)-1;
        int lenEntry=strlen(entry->d_name);

        if(lenSuffixe==0||(strcmp(&part[1],&(entry->d_name[lenEntry-lenSuffixe]))==0)){
        // Allocate memory for the new path
        char* new_path = malloc(sizeof(char)*(strlen(path) -(lenSuffixe+1)+ lenEntry + 1));
        // Replace the wildcard with the entry name
        memmove(new_path, path, sizeof(char)*index);
        memmove(&new_path[index], entry->d_name,sizeof(char)*lenEntry);
        memmove(&new_path[index + lenEntry], &path[index + lenSuffixe + 1],sizeof(char)*( strlen(path)-index-(lenSuffixe+1)+1));

        // Add the new path to the result array
        paths[num_paths] = new_path;
        num_paths++;
        }
      }

      break;
    }
    index+=strlen(part);

    // Move to the next part of the path
    part = strtok(NULL, "/");
  }
  // Close the current directory and free the copy of the path
  closedir(current_dir);
  free(copy);

  // Add a NULL element to the end of the array to mark the end
  paths[num_paths] = NULL;
  if(num_paths > 0){
    if(strstr(paths[0],"*")!=NULL){
      for(int i=0;i<num_paths;i++){
        expand_wildcard(cmd,argindex,paths[i]);
      }
    }else {
      addArgsAt(argindex,cmd,paths,num_paths);
    }
  }
  
  for(int i=0;i<num_paths;i++){
    free(paths[i]);
  }
  free(paths);
}