
struct node{
  int key;
  unsigned long long value;
  struct node *next;
};

struct myhash{
  int size;
  int count;
  struct node **arr;
};

int search(struct myhash *table, struct node *target){
  int hashkey;
  struct node *head, *ptr;
  target->key = target->value % table->size;
  hashkey = target->key;
  head = table->arr[hashkey];
  ptr = head;
  if(head == NULL){
    return 0;
  }
  while(ptr!=NULL){
    if(ptr->value == target->value){
      return 1;
    }
    ptr= ptr->next;
  }
  return 0;
}
  

int insert(struct myhash *table, struct node *new){
  
  int hashkey;
  new->key = new->value % table->size;
  new->next = NULL;
  hashkey = new->key;
  if(table->arr[hashkey]==NULL){
    table->arr[hashkey] = new;
    table->arr[hashkey]->next = NULL;
    table->count++;
    return 0;
  }
  new->next = table->arr[hashkey];
  table->arr[hashkey] = new;
  table->count++;
  return 0;
}
