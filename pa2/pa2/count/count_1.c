#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "count.h"

int main(int argc, char **argv){
  if(argc<2){
    printf("error");
    return 0;
  }
  char* fname = argv[1];
  char ch[BUFSIZ];
  FILE *fp = fopen(fname, "r");
  char *st;
  unsigned long long val;
  struct myhash *new = malloc(sizeof(struct myhash));
  new->size = 1000;
  new->count = 0;
  new->arr = calloc(new->size, sizeof(struct node *));
  int i;
  for(i=0; i<new->size; i++){
    new->arr[i] = NULL;
  }

  if(!fp){
    printf("error");
    return 0;
  }
  
  if(fp==NULL){
    printf("%d", new->count);
    return 0;
  }
  
  else{
    while(fgets(ch, sizeof(ch), fp)){
      st = strtok(ch, "\n");
      val = strtoll(st,NULL,16);
      struct node *newnode = calloc(1, sizeof(struct node));
      newnode->value = val;
      newnode->next = NULL;
      if(search(new,newnode)==0){
	  insert(new, newnode);
      }
    }
  }
  printf("%d", new->count);
  fclose(fp);
  free(new);
  return 0;
}
