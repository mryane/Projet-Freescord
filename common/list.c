#include "list.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/** Sur MacOS, la fonction error n'est pas définie et il n'y a pas
 * de fichier "error.h" */
static void error(int statut, int numerr, const char *format, ...)
{
	if (numerr) {
		fprintf(stderr, "%s\n", strerror(numerr));
	}
	va_list ap;
	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);
	exit(statut);
}

struct node *list_create_node(void *elt)
{
	struct node *r = malloc(sizeof(struct node));
	if (r == NULL)
		error(2, 0, "malloc failed in create_node\n");
	r->elt = elt;
	return r;
}

struct list *list_create(void)
{
	struct list *r = malloc(sizeof(struct list));
	if (r == NULL)
		error(2, 0, "malloc failed in list_create\n");
	r->length = 0;
	r->first = NULL;
	r->last = NULL;
	return r;
}

void list_free(struct list *l, void (*free_fct)(void *))
{
	struct node *curr = l->first, *tmp;
	while (curr != NULL) {
		if (free_fct)
			free_fct(curr->elt);
		tmp = curr;
		curr = curr->next;
		free(tmp);
	}
	free(l);
}

int list_is_empty(const struct list *l)
{
	return l->length == 0;
}

size_t list_length(const struct list *l)
{
	return l->length;
}

/** only use in empty lists! */
static struct list *list_add_in_empty(struct list *l, void *elt)
{
	struct node *n = list_create_node(elt);
	n->next = n->prev = NULL;
	l->first = l->last = n;
	l->length = 1;
	return l;
}

struct list *list_add(struct list *l, void *elt)
{
	if (list_is_empty(l))
		return list_add_in_empty(l, elt);
	struct node *n = list_create_node(elt);
	++(l->length);
	l->last->next = n;
	n->prev = l->last;
	l->last = n;
	n->next = NULL;
	return l;
}

struct list *list_add_first(struct list *l, void *elt)
{
	if (list_is_empty(l))
		return list_add_in_empty(l, elt);
	struct node *n = list_create_node(elt);
	++(l->length);
	n->prev = NULL;
	l->first->prev = n;
	n->next = l->first;
	l->first = n;
	return l;
}

void list_insert_before_node(struct list *l, void *elt, struct node *curr)
{
	struct node *n = list_create_node(elt);
	if (l->first == curr)
		l->first = n;
	n->prev = curr->prev;
	n->next = curr;
	if (curr->prev != NULL)
		curr->prev->next = n;
	curr->prev = n;
	++(l->length);
}

void list_insert_after_node(struct list *l, void *elt, struct node *curr)
{
	struct node *n = list_create_node(elt);
	if (l->last == curr)
		l->last = n;
	n->next = curr->next;
	n->prev = curr;
	if (curr->next != NULL)
		curr->next->prev = n;
	curr->next = n;
	++(l->length);
}

struct list *list_add_index(struct list *l, void *elt, size_t i)
{
	if (i > l->length)
		error(1, 0, "list_add_index: index %zu out of bound %zu\n",
			i, l->length);
	if (list_is_empty(l))
		return list_add_in_empty(l, elt);
	if (i == list_length(l))
		return list_add(l, elt);
	if (i == 0)
		return list_add_first(l, elt);
	struct node *curr = l->first;
	for (; i > 0; curr = curr->next, i--)
		;
	list_insert_before_node(l, elt, curr);
	return l;
}

void list_print(const struct list *l, void (*pri_fct)(const void *))
{
	printf("[");
	for (struct node *curr = l->first; curr != NULL; curr = curr->next) {
		pri_fct(curr->elt);
		if (curr != l->last)
			printf(", ");
	}
	printf("]");
}

void *list_get(const struct list *l, size_t i)
{
	if (i >= l->length)
		error(1, 0, "list_get: index %zu out of bound %zu\n",
			i, l->length - 1);
	struct node *curr = l->first;
	while (i > 0) {
		curr = curr->next;
		i--;
	}
	return curr->elt;
}

void *list_remove_node(struct list *l, struct node *n)
{
	if (n == NULL)
		return NULL;
	if (n == l->first)
		l->first = n->next;
	if (n == l->last)
		l->last = n->prev;
	if (n->prev != NULL)
		n->prev->next = n->next;
	if (n->next != NULL)
		n->next->prev = n->prev;
	l->length--;
	void *res = n->elt;
	free(n);
	return res;
}

void *list_remove(struct list *l)
{
	return list_remove_node(l, l->last);
}

void *list_remove_first(struct list *l)
{
	return list_remove_node(l, l->first);
}

void *list_remove_element(struct list *l, void *elt)
{
	struct node *curr = l->first;
	for (; curr != NULL && curr->elt != elt; curr = curr->next)
		;
	return list_remove_node(l, curr);
}

void *list_remove_index(struct list *l, size_t i)
{
	if (i >= l->length)
		error(1, 0, "list_add_index: index %zu out of bound %zu\n",
			i, l->length);
	struct node *curr = l->first;
	for (; curr != NULL && i > 0; curr = curr -> next, i = i - 1)
		;
	return list_remove_node(l, curr);
}
