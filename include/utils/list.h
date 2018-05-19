#ifndef LIST_H
#define LIST_H

typedef struct node_t node;

struct node_t {
	void *data;
	node *next;
	node *prev;
};

typedef struct list_t {
	node *head;
	node *tail;
}list;

node *node_new(void *data);

list *list_new();
void list_append(list *lst,node *elem);
void list_pop(list *lst);
void list_empty(list *lst);
void list_free(list *lst);

#endif
