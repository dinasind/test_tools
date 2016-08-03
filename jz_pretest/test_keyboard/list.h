#ifndef __PRETEST_LIST_H__
#define __PRETEST_LIST_H__
#include <stdbool.h>

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>


union widget {
	struct button *button;
	struct keyboard *keyboard;
};

typedef struct Node {
	union widget widget;
	struct Node *pNext;
} NODE, *PNODE;

PNODE create_list(void);
void initialize_list(PNODE*);
int is_empty(PNODE);
int length_list(PNODE);
int insert_button(PNODE, int, struct button *);
int insert_keyboard(PNODE, int, struct keyboard *);
int add_button(PNODE*,struct button*);
int add_keyboard(PNODE*,struct keyboard*);
int delete_list(PNODE, int);
#endif
