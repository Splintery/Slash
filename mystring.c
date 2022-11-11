#include <stdlib.h>
#include <string.h>

#include "mystring.h"

struct string * string_new(size_t capacity){
  struct string* res = malloc(sizeof(struct string));
  res->capacity=capacity;
  res->length=0;
  res->data=malloc(sizeof(char)*capacity);
  res->data[0]='\0';
  return res;
}
// Crée une nouvelle chaîne de longueur 0 et de capacité donnée, allouée
// sur le tas (c'est-à-dire avec malloc).

void string_delete(struct string * str){
  free(str->data);
  free(str);
}
// Détruit une chaîne, en libérant la mémoire occupée.

int string_append (struct string * dest, char * src){
  size_t len = dest->length + strlen(src);
  if(len < dest->capacity){
    strcat(dest->data,src);
    dest->length=len;
    return 1;
  }
  return 0;
}
// Met à jour dest en ajoutant src à la *fin*.
// Renvoie 1 en cas de réussite, 0 en cas d'échec.
// Échoue sans rien modifier si le buffer n'est pas assez grand.

void string_truncate_from_the_end (struct string * str, size_t nchars){
  if(nchars>=str->length){
    str->data[0]='\0';
    str->length=0;
  }else{
    size_t len =str->length - nchars;
    str->data[len]='\0';
    str->length=len;
  }
}
// Tronque la chaîne en enlevant les nchars derniers caractères
// (ou tous les caractères s'il y en a moins de nchars).

void string_truncate_from_the_beginning (struct string * str, size_t nchars){
  if(nchars>=str->length){
    str->data[0]='\0';
    str->length=0;
  }else{
    size_t len =str->length - nchars;
    memmove(str->data,&str->data[nchars],len);
    str->data[len]='\0';
    str->length=len;
  }
}

// Tronque la chaîne en enlevant les nchars premiers caractères
// (ou tous les caractères s'il y en a moins de nchars).
