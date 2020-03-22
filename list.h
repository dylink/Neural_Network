typedef struct List List;
struct List {
  int data;
  List * next;
};

List * list_create(int data){
  List *list = malloc(sizeof(list));
  if (list){
    list->data = data;
    list->next = NULL;
  }
  return list;
}

List * list_append(List *list, int data){
    List **plist = &list;
    while (*plist)
       plist = &(*plist)->next;
    *plist = list_create(data);
    if (*plist)
       return list;
    else
       return NULL;
}

int list_length(List *list){
    int length = 0;
    while (list)
     {
       length++;
       list = list->next;
     }
     return length;
}

int list_getDataByIndex(List * list, int index){
  List * current = list;
  for(int i = 0; i<index; i++)
    current = current->next;

  return current->data;
}
