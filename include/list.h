#ifndef LIST_H
#define LIST_H value
#include <stdlib.h>

/** Doubly linked lists of generic (void *) values */

/**
 * We define a struct list type to contain generic (void *) values and functions
 * to perform usual operations on lists.
 * All functions start with the prefix "list_", and (except for create) take a
 * pointer to a list as first argument.
 * 
 * There are higher-level functions to take care of the details:
 * - create (to create an empty list)
 * - add (at the end), add_first (at the beginning) and add_index (at the given
 *   index). add_index checks bounds and terminates the process in case the
 *   index is out of bounds.
 *   These functions return a pointer to the list, to make it possible to chain
 *   function calls.
 * - remove (at the end), remove_first (at the beginning) and remove_index (at
 *   the given index), the first two never fail (they leave the list unchanged
 *   and return NULL if the list is empty). remove_index checks bounds and
 *   terminates the process if the index is out of bounds.
 *   remove_element removes in the list the first element equal (as an
 *   address), if any, to the given element.
 *   If an element is removed, these functions return it (so that, for instance,
 *   it can be freed if need be).
 * - length, is_empty, get (with an index) and print (with a function argument)
 *   do not modify the list.
 *   The get function checks bounds and terminates the process in case the
 *   index is out of bounds.
 * - free, for which we can pass a free function pointer argument, if the (void
 *   *) elements need to be freed
 * - note that any failure of malloc in this library terminates the process
 *   with an error message
 *
 * Such lists can be used efficiently to implement generic
 * - stacks : push with add, pop with remove
 * - queues : enqueue with add, pop with remove_first
 * without ever needing to manipulate nodes.
 *
 * Accessing or inserting at first and last positions are O(1) operations.
 * get(i) has O(i) complexity
 *
 * The struct list and struct node types are not opaque, to allow to iterate
 * on lists with something like
 * struct list *l = list_create();
 * // then do things with l
 * struct node *curr = l->first;
 * for (; curr != NULL; curr = curr -> next) {
 *	do_something(curr);
 * }
 *
 * lower-level operations are given:
 * - insert_node_before, to insert a value before a given node in the list
 * - insert_node_after, to insert a value after a given node in the list
 * - remove_node, to remove a given node in the list
 * - create_node, to create a new node on the heap with a given (void *) element
 * The insertion after or before a given node and the removal operation of a given
 * node assume that the given node is part of the list. Behaviour is unspecified
 * if it is not.
 *
 * Any list manipulation using lower level operations, or by modifying the
 * members of the struct list (or struct nodes of this list) is on the sole
 * responsability of the user.
 *
 * The test file provides many examples.
 */

struct node {
	void *elt;
	struct node *next;
	struct node *prev;
};

struct list {
	size_t length;
	struct node *first;
	struct node *last;
};

/** create an empty struct list */
struct list *list_create(void);

/** return the length of list */
size_t list_length(const struct list *l);

/** free the struct list, with all its nodes; additionnally, if free_fct is not
 * NULL, free each value in the struct list with free_fct */
void list_free(struct list *l, void (*free_fct)(void *));

/** return 1 if the list is empty, 0 otherwise */
int list_is_empty(const struct list *l);

/** add elt at the end of the list
 * returns a pointer to the same list */
struct list *list_add(struct list *l, void *elt);

/** add elt at the beginning of the list
 * returns a pointer to the same list */
struct list *list_add_first(struct list *l, void *elt);
/** add elt at position i in the list, with 0 <= i <= length(l)
 * returns a pointer to the same list */
struct list *list_add_index(struct list *l, void *elt, size_t i);

/** print the values of the struct list in the format
 * [first, second, ..., last], where first, second, etc...
 * are printed with the function pri_fct */
void list_print(const struct list *l, void (*pri_fct)(const void *));

/** return the i-th element of list l, with 0 <= i < length(l) */
void *list_get(const struct list *l, size_t i);

/** remove the last element of the list
 * return the element which was removed, or NULL if there is no such one */
void *list_remove(struct list *l);

/* remove the first element of the list
 * return the element which was removed, or NULL if there is no such one */
void *list_remove_first(struct list *l);

/* remove the i-th element from the struct list, with 0 <= i < length(l)
 * return the element which was removed */
void *list_remove_index(struct list *l, size_t i);

/* remove the first element whose adress is elt in the list */
void *list_remove_element(struct list *l, void *elt);

/** lower-level operations: these function expose more details than the previous
 * functions, but may prove useful if the preconditions are satisfied */

/** remove the node curr in the list l, curr should be a node in the given list,
 * otherwise, the behaviour is unspecified */
void *list_remove_node(struct list *l, struct node *curr);

/** insert elt in the list l before the node curr, which should be a node in the
 * given list, otherwise, the behaviour is unspecified */
void list_insert_before_node(struct list *l, void *elt, struct node *curr);

/** insert elt in the list l after the node curr, which should be a node in the
 * given list, otherwise, the behaviour is unspecified */
void list_insert_after_node(struct list *l, void *elt, struct node *curr);

/** create a node containing elt as elt, but whose next and prev fields are
 * unspecified */
struct node *list_create_node(void *elt);
#endif /* ifndef LIST_H */
