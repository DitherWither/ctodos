#ifndef _TODOS_H_
#define _TODOS_H_

#include "common.h"

#define TODOS_TYPE_INCOMPLETE 0
#define TODOS_TYPE_IN_PROGRESS 1
#define TODOS_TYPE_COMPLETE 2

struct TodoItem {
	char title[32];

	int8_t type;

	struct TodoItem *next;
	struct TodoItem *previous;
};

struct TodoItem *todos_get_by_index(int idx);
void todos_insert_element(struct TodoItem *item);
int todos_get_length();

char *todos_type_to_string(struct TodoItem *item);
int todos_type_from_string(char *str);

struct TodoItem *todos_get_head();
struct TodoItem *todos_get_tail();

#endif