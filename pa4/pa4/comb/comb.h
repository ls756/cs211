
struct node{
  char key;
  char val;
  struct node *left;
  struct node *right;
  char* relation;
};

struct link{
  char k;
  char v;
  struct link *next;
};

int log2(int n){
  int i=0;
  while(n>1){
    i++;
    n = n/2;
  }
  return i;
}

struct link *insert(struct node *new, struct link *head){
  struct link *temp = calloc(1, sizeof(struct link));
  temp->k = new->key;
  temp->v = new->val;
  temp->next = NULL;
  if(head == NULL){
    head = temp;
  }
  else{
    temp->next = head;
    head = temp;
  }
  return head;
}

int search(char key, struct link *head){
  struct link *ptr = calloc(1, sizeof(struct link));
  for(ptr = head; ptr != NULL; ptr = ptr->next){
    if(ptr->k == key){
      return 1;
    }
  }
  return 0;
}


char getValue(char key, struct link *head){
  struct link *ptr = calloc(1, sizeof(struct link));
  for(ptr = head; ptr != NULL; ptr = ptr->next){
    if(ptr->k == key){
      return ptr->v;
    }
  }
  return '\0';
}

void AND(struct node *x, struct node *y, struct node *z){
  z->left = x;
  z->right = y;
  z->relation = "AND";
  if(x->val == '1' && y->val == '1'){
    z->val = '1';
  }
  else{
    z->val = '0';
  }
  
}


void OR(struct node *x, struct node *y, struct node *z){
  z->left = x;
  z->right = y;
  z->relation = "OR";
  if(x->val == '0' && y->val == '0'){
    z->val = '0';
  }
  else{
    z->val = '1';
  }
}


void NOT(struct node *x, struct node *z){
  z->left = x;
  z->right = NULL;
  z->relation = "NOT";
  if(x->val == '1'){
    z->val = '0';
  }
  else{
    z->val = '1';
  }
}


void greycode(int n, char **code){
  int i;
  char *s1 = "0";
  char *s2 = "1";
  char *result;
  if(n==1){
    strcpy(code[0], s1);
    strcpy(code[1], s2);
  }
  else{
    greycode(n-1, code);
    
    for(i=0; i<((1<<n)/2); i++){
      strcpy(code[(1<<n)-1-i], code[i]);
    }
   
    for(i=0; i<(1<<n); i++){
        
	  
      if(i<((1<<n)/2)){
	result = malloc(strlen(s1)+strlen(code[i])+1);
	strcpy(result, s1);
	strcat(result, code[i]);
	code[i] = malloc(strlen(result+1));
	strcpy(code[i], result);
	
      }
      else{
	result = malloc(strlen(s2)+strlen(code[i])+1);
	strcpy(result, s2);
	strcat(result, code[i]);
	code[i] = malloc(strlen(result+1));
	strcpy(code[i], result);
      }
    }
  }
  
  
}

char MUX(int num, int *input, struct node **selector, struct node *output){
  int i,j,m;
  struct node **temp;
  temp = malloc(num);
  char **gray = malloc(1<<num);
  for(i=0; i<(1<<num); i++){
    gray[i] = malloc(num);
  }
  greycode(num, gray);
  int *sel = malloc(num);
  
  for(i=0; i<num; i++){
    temp[i] = calloc(1, sizeof(struct node));
  }
  for(i=0; i<num; i++){
    sel[i]=selector[i]->val-'0';
  }
  for(i=0; i<(1<<num); i++){
    
    if(input[i] == 1){
      
      for(j=0; j<num; j++){
	if(gray[i][j] == '0'){
	  if(sel[j]==0){
	    temp[j]->val = '1';
	  }
	  else{
	    temp[j]->val = '0';
	  }
	}
	else{
	  if(sel[j]==0){
	    temp[j]->val = '0';
	  }
	  else{
	    temp[j]->val = '1';
	  }
	}
      }
      if(num == 1){
	output->val = temp[0]->val;
      }
      else{
        if(temp[0]->val == '1' && temp[1]->val == '1'){
	  output->val = '1';
	}
	else{
	  output->val = '0';
	}
      }
      if(num>2){
	for(m=2; m<num; m++){
	  if(temp[m]->val == '1' && output->val == '1'){
	    output->val = '1';
	  }
	  else{
	    output->val = '0';
	  }
	}
      }
      if(output->val == '1'){
	return '1';
      }
      
    }
      
  }
  
  return '0';
  
}

void DECODER(int num, struct node **input, struct node **output){
  int i, j, m;
  struct node **temp = malloc(num);
  char ** gray = malloc(1<<num);
  for(i=0; i<num; i++){
    temp[i] = calloc(1, sizeof(struct node));
  }
  for(i=0; i<(1<<num); i++){
    gray[i] = malloc(num);
  }
  greycode(num, gray);
  for(i=0; i<(1<<num); i++){
    for(j=0; j<num; j++){
      temp[j]->val = gray[i][j];
      if(temp[j]->val == '0'){
	NOT(input[j], temp[j]);
      }
      else{
	temp[j]=input[j];
      }
    }
    if(num == 1){
      output[i]->val = temp[0]->val;
    }
    for(m=0; m<num-1; m++){
      AND(temp[m], temp[m+1], output[i]);
    }
  }


}
