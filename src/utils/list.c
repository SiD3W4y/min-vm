#include <stdlib.h>
#include <stdint.h>

#include "utils/list.h"

node *node_new(void *data)
{
	node *n = malloc(sizeof(node));

	n->data = data;
	n->prev = NULL;
	n->next = NULL;

	return n;
}

list *list_new()
{
	list *l = malloc(sizeof(list));
	l->head = NULL;
	l->tail = NULL;

	return l;
}

void list_append(list *lst,node *elem)
{
	if(lst->head != NULL){
		node *old_tail = lst->tail;

		old_tail->next = elem;
		elem->prev = old_tail;

		lst->tail = elem;
	}

	if(lst->head == NULL && lst->tail == NULL){
		lst->head = elem;
		lst->tail = elem;
	}

}

void list_pop(list *lst)
{
	node *tail = lst->tail;

	if(lst->head != NULL && lst->tail != NULL){
		if(tail == lst->head){
			lst->head = NULL;
			lst->tail = NULL;
		}else{
			lst->tail = tail->prev;
			lst->tail->next = NULL;
		}

		free(tail->data);
		free(tail);
	}
}

void list_empty(list *lst)
{
	node *n = lst->head;
	node *c;
	
	if(lst->head != NULL){
		while(n->next != NULL){
			c = n->next;
			free(n->data);
			free(n);
			n = c;
		}
	}

	lst->head = NULL;
	lst->tail = NULL;
}

void list_free(list *lst)
{
	list_empty(lst);
	free(lst);
}
