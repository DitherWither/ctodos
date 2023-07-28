#include "todos.h"

struct TodoItem *todos_head = NULL;
struct TodoItem *todos_tail = NULL;

struct TodoItem* todos_get_head()
{
        return todos_head;
}

struct TodoItem* todos_get_tail()
{
        return todos_tail;
}

struct TodoItem *todos_get_by_index(int idx)
{
	struct TodoItem *cursor = todos_head;

	if (cursor == NULL) {
		return NULL;
	}

	for (int i = 0; i < idx; i++) {
		if (cursor->next != NULL)
			cursor = cursor->next;
	}

	return cursor;
}

struct TodoItem *todos_get_from_end_by_index(int idx)
{
	struct TodoItem *cursor = todos_head;

	if (cursor == NULL) {
		return NULL;
	}

	for (int i = 0; i < idx; i++) {
		if (cursor->next != NULL)
			cursor = cursor->previous;
	}

	return cursor;
}

void todos_insert_element(struct TodoItem* item)
{
	if (todos_tail == NULL) {
		todos_head = item;
		todos_tail = item;
		todos_head->next = NULL;
		todos_tail->previous = NULL;
	}

	item->next = NULL;
	item->previous = todos_tail;
	todos_tail->next = item;
	todos_tail = todos_tail->next;
}

int todos_get_length()
{
	struct TodoItem *cursor = todos_head;
        int len = 0;
        while (cursor != NULL) {
                cursor = cursor->next;
                len++;
        }
        return len;
}
