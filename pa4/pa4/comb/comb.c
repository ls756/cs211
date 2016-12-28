#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include<malloc.h>
#include<math.h>
#include <ctype.h>
#include "comb.h"

int main(int argc, char ** argv)
{
  
  char* f1name = argv[1];
  char* f2name = argv[2];
  char ch[BUFSIZ];
  char ch2[BUFSIZ];
  FILE *fp1 = fopen(f1name, "r");
  FILE *fp2 = fopen(f2name, "r");
  char *token;
  int index = 0;
  int i = 0;
  int j = 0;
  int n = 0;
  char input[BUFSIZ];
  int inum, onum;
  char *iarr;
  char *oarr;
  char output;
  struct node **nl;
  
  if(argc<2){
    printf("error");
    return 0;
  }
  
  if(!fp1){
    printf("error");
    return 0;
  }
  
  if(!fp2){
    printf("error");
    return 0;
  }

  while(fgets(ch2, sizeof(ch2), fp2)!=NULL){
    struct link *head = calloc(1, sizeof(struct link));
    index = 0;
    fgets(ch, sizeof(ch), fp1);
    token = strtok(ch, " ");
    token = strtok(NULL, " ");
    inum = atoi(token);
    
    for(j=0; j<inum*2; j=j+2){
      index = j/2;
      input[index] = ch2[j];
      index++;
    }
    
    iarr = malloc(inum);
    nl = malloc(inum);
    for(i=0; i<inum; i++){
      nl[i]=calloc(1,sizeof(struct node));
    }
    j=0;
    token = strtok(NULL, " ");
    while(token!=NULL){
      for(i=0; i<strlen(token); i++){
	if(token[i]!=' '){
	  iarr[j] = token[i];
	  struct node *newnode = calloc(1, sizeof(struct node));
	  newnode->key = iarr[j];
	  newnode->val = input[j];
	  newnode->left = NULL;
	  newnode->right = NULL;
	  newnode->relation = NULL;
	  nl[j] = newnode;
	  j++;
	}
      }
      token = strtok(NULL, " ");
    }
    
    index = 0;
    fgets(ch, sizeof(ch), fp1);
    token = strtok(ch, " ");
    onum = token[10]-'0';
    oarr = malloc(onum);
    for(i = 12; i<12+onum*2; i = i+2){
      oarr[index] = ch[i];
      index++;
    }

    
    while(fgets(ch, sizeof(ch), fp1) != NULL){
      
      index = 0;
      token = strtok(ch, " ");
      
      if(strcmp(token, "MULTIPLEXER") == 0){
	
        n = log2(token[12]-'0');
	int listlen = n+(1<<n)+1;
	char list[listlen];

	int *inputVal = malloc(1<<n);
	struct node **selector = malloc(n);
	for(i=0; i<n; i++){
	  selector[i] = calloc(1, sizeof(struct node));
	}
	
	token = strtok(NULL, " ");
	token = strtok(NULL, " ");
	while(token != NULL){
	  list[index] = token[0];
	  token = strtok(NULL, " ");
	  index++;
	}
	for(i=0; i<(1<<n); i++){
	  struct node *temp = calloc(1, sizeof(struct node));
	  if(list[i] == '0' || list[i] == '1'){
	    temp->val = list[i];
	    temp->key = '\0';
	    temp->left = NULL; temp->right = NULL; temp->relation = NULL;
	  }
	  else{
	  temp->key = list[i];
	  temp->left = NULL; temp->right = NULL; temp->relation = NULL;
	  if(search(temp->key, head) == 1){
	    temp->val = getValue(temp->key, head);
	  }
	  else{
	  for(j=0; j<inum; j++){
	    if(nl[j]->key == temp->key){
	      temp->val = nl[j]->val;
	    }
	  }
	  }
	  }
	  inputVal[i] = temp->val-'0';
	  
	}
        
	
	for(i=i; i<listlen-1; i++){
	  struct node *temp = calloc(1,sizeof(struct node));
	  if(list[i] == '0' || list[i] == '1'){
	    temp->val = list[i];
	  }
	  else{
	  temp->key = list[i];
	  temp->left = NULL; temp->right = NULL; temp->relation = NULL;
	  if(search(temp->key, head) == 1){
	    temp->val = getValue(temp->key, head);
	  }
	  for(j=0; j<inum; j++){
	    if(nl[j]->key == temp->key){
	      temp->val = nl[j]->val;
	    }
	  }
	  }
	  selector[i-(1<<n)] = temp;
	}
	
	struct node *c = calloc(1, sizeof(struct node));
	c->key = list[listlen-1];
	c->left = NULL; c->right = NULL; c->relation = NULL;
	c->val = MUX(n, inputVal, selector, c);

	if(search(c->key, head) == 0){
	  head = insert(c, head);
	}
        
	
      }

       else if(strcmp(token, "DECODER") == 0){
	
	n = token[8]-'0';
        
	int listlen = n+(1<<n);
	char list[listlen];

        
	struct node **inde = malloc(n);
	for(i=0; i<n; i++){
	  inde[i] = calloc(1, sizeof(struct node));
	}

	struct node **outde = malloc(n);
	for(i=0; i<n; i++){
	  outde[i] = calloc(1, sizeof(struct node));
	}
	
	token = strtok(NULL, " ");
	token = strtok(NULL, " ");
	while(token != NULL){
	  list[index] = token[0];
	  token = strtok(NULL, " ");
	  index++;
	}
	for(i=0; i<n; i++){
	  struct node *temp = calloc(1, sizeof(struct node));
	  temp->key = list[i];
	  temp->left = NULL; temp->right = NULL; temp->relation = NULL;
	  if(search(temp->key, head) == 1){
	    temp->val = getValue(temp->key, head);
	  }
	  for(j=0; j<inum; j++){
	    if(nl[j]->key == temp->key){
	      temp->val = nl[j]->val;
	    }
	  }
	  inde[i] = temp;
	  
	}
        
	
	for(i=i; i<listlen-1; i++){
	  struct node *temp = calloc(1,sizeof(struct node));
	  temp->key = list[i];
	  outde[i-n] = temp;
	  
	}

	DECODER(n, inde, outde);
	for(i=0; i<(1<<n); i++){
	  printf("output %c = %c", outde[i]->key, outde[i]->val);
	  if(search(outde[i]->key, head) == 0){
	    head = insert(outde[i], head);
	  }
	}
	
	
      }

      else if(strcmp(token, "AND") == 0){
	char and[3];
	token = strtok(NULL, " ");
	while(token != NULL){
	  and[index] = token[0];
	  index++;
	  token = strtok(NULL, " ");
	}
      
	struct node *a = calloc(1, sizeof(struct node));
	struct node *b = calloc(1, sizeof(struct node));
	struct node *c = calloc(1, sizeof(struct node));
	a->key = and[0];
	a->left = NULL; a->right = NULL; a->relation = NULL;
	if(search(a->key, head) == 1){
	  a->val = getValue(a->key, head);
	}
	b->key = and[1];
	b->left = NULL; b->right = NULL; b->relation = NULL;
	if(search(b->key, head) == 1){
	  b->val = getValue(b->key, head);
	}
	for(i=0; i<inum; i++){
	  if(nl[i]->key == a->key){
	    a->val = nl[i]->val;
	  }
	  if(nl[i]->key == b->key){
	    b->val = nl[i]->val;
	  }
	}
	
	c->key = and[2];
	AND(a, b, c);

	if(search(c->key, head) == 0){
	  head = insert(c, head);
	}
	
      }

      else if(strcmp(token, "OR") == 0){
	char or[3];
	token = strtok(NULL, " ");
	while(token != NULL){
	  or[index] = token[0];
	  index++;
	  token = strtok(NULL, " ");
	}
	struct node *a = calloc(1, sizeof(struct node));
	struct node *b = calloc(1, sizeof(struct node));
	struct node *c = calloc(1, sizeof(struct node));
	a->key = or[0];
	a->left = NULL; a->right = NULL; a->relation = NULL;
	if(search(a->key, head) == 1){
	  a->val = getValue(a->key, head);
	}
	b->key = or[1];
	b->left = NULL; b->right = NULL; b->relation = NULL;
	if(search(b->key, head) == 1){
	  b->val = getValue(b->key, head);
	}
	for(i=0; i<inum; i++){
	  if(nl[i]->key == a->key){
	    a->val = nl[i]->val;
	  }
	  if(nl[i]->key == b->key){
	    b->val = nl[i]->val;
	  }
	}
      
	c->key = or[2];
	OR(a,b,c);

	if(search(c->key, head) == 0){
	  head = insert(c, head);
	}
      }

      else if(strcmp(token, "NOT") == 0){
	char not[2];
	token = strtok(NULL, " ");
	while(token != NULL){
	  not[index] = token[0];
	  index++;
	  token = strtok(NULL, " ");
	}
	struct node *a = calloc(1, sizeof(struct node));
	struct node *c = calloc(1, sizeof(struct node));
	a->key = not[0];
	a->left = NULL; a->right = NULL; a->relation = NULL;
	if(search(a->key, head) == 1){
	  a->val = getValue(a->key, head);
	}
	for(i=0; i<inum; i++){
	  if(nl[i]->key == a->key){
	    a->val = nl[i]->val;
	  }
	}
      
	c->key = not[1];
	NOT(a,c);

	if(search(c->key, head) == 0){
	  head = insert(c, head);
	}
      }

      

      
    }
    
    for(i=0; i<onum; i++){
      output = getValue(oarr[i], head);
      printf("%c ", output);
      
    }
    printf("\n");
    rewind(fp1);
  }
  fclose(fp1);
  return 0;

}
