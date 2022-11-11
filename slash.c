#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mystring.h"

struct string * prompt_line(int ret,char * path){
  struct string* res = string_new(31);
  string_append(res,"[");
  char ret_string[27];
  sprintf(ret_string,"%d",ret);
  string_append(res,ret_string);
  string_append(res,"]");

  size_t path_len = strlen(path);
  if(res->length+path_len>28){
    string_append(res,"...");
    struct string* tmp= string_new(path_len+1);
    string_append(tmp,path);
    string_truncate_from_the_beginning(tmp,path_len-(28-res->length));
    string_append(res,tmp->data);
    string_delete(tmp);
  }else{
    string_append(res,path);
  }
  string_append(res,"$ ");
  return res;

}

int main(){
  //Tests temporaires
	char *test = "test";
	struct string * ligne = prompt_line(1,test);
	printf("%s\n",ligne->data);
  char *test2 = "troplong/troptroplong/supermegalong/olalalala";
  struct string * ligne2 = prompt_line(0,test2);
	printf("%s\n",ligne2->data);



}
