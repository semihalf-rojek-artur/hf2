#ifndef _LIST_H_
#define _LIST_H_

#define LIST_ITEM(elem, type, member) \
	((type *)((size_t)(elem) - (size_t)&(((type *)0)->member)))

#define LIST_VALID(head) \
	((head)->prev && (head)->next)

#define LIST_EMPTY(head) \
	((head)->prev == (head) || (head)->next == (head))

#define LIST_INIT(head) do { \
	(head)->prev = (head); \
	(head)->next = (head); \
} while (0)

#define LIST_ADD(head, elem) do { \
	(elem)->prev = (head); \
	(elem)->next = (head)->next; \
	(head)->next->prev = (elem); \
	(head)->next = (elem); \
} while (0)

#define LIST_ADD_BEFORE(head, elem) do { \
	(elem)->prev = (head)->prev; \
	(elem)->next = (head); \
	(head)->prev->next = (elem); \
	(head)->prev = (elem); \
} while (0)

#define LIST_REMOVE(elem) do { \
	(elem)->prev->next = (elem)->next; \
	(elem)->next->prev = (elem)->prev; \
	(elem)->prev = (elem); \
	(elem)->next = (elem); \
} while (0)

#define LIST_FOREACH(head, elem) \
	for ((elem) = (head)->next; \
	     (elem) != (head); \
	     (elem) = (elem)->next)

#define LIST_FOREACH_SAFE(head, elem, n) \
	for ((elem) = (head)->next, \
	     (n) = (elem)->next; \
	     (elem) != (head); \
	     (elem) = (n), \
	     (n) = (elem)->next)

#define LIST_FOREACH_REVERSE(head, elem) \
	for ((elem) = (head)->prev; \
	     (elem) != (head); \
	     (elem) = (elem)->prev)

#define LIST_FOREACH_REVERSE_SAFE(head, elem, p) \
	for ((elem) = (head)->prev, \
	     (p) = (elem)->prev; \
	     (elem) != (head); \
	     (elem) = (p), \
	     (p) = (elem)->prev)

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#endif /* _LIST_H_ */
