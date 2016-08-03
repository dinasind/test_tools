#include <stdio.h>
#include <stdlib.h>
#include "list.h"

void initialize_list(PNODE *pHead) {
	*pHead = NULL;
}

int is_empty(PNODE pHead) {
	if (NULL == pHead->pNext) {
		printf("链表为空!\n");
		return 1;
	} else {
		printf("链表不为空!\n");
		return 0;
	}
}

int length_list(PNODE pHead) {
	int len = 0;
	PNODE p = pHead->pNext;

	while (p != NULL) {
		++len;
		p = p->pNext;
	}
	return len;
}

int insert_button(PNODE pHead, int pos, struct button *button) {
	int i = 0;
	PNODE p = pHead;

	while (p != NULL && i < pos - 1) {
		p = p->pNext;
		++i;
	}
	if (i > pos - 1 || p == NULL)
		return 0;

	PNODE pNew = (PNODE) malloc(sizeof(NODE));
	if (pNew == NULL) {
		printf("动态申请内存失败\n");
		return -1;
	}
	pNew->widget.button = button;
	pNew->pNext = p->pNext;
	p->pNext = pNew;
	return 0;
}
int insert_keyboard(PNODE pHead, int pos, struct keyboard *keyboard) {
	int i = 0;
	PNODE p = pHead;

	while (p != NULL && i < pos - 1) {
		p = p->pNext;
		++i;
	}
	if (i > pos - 1 || p == NULL)
		return -1;

	PNODE pNew = (PNODE) malloc(sizeof(NODE));
	if (pNew == NULL) {
		printf("动态申请内存失败\n");
		return -1;
	}
	pNew->widget.keyboard = keyboard;
	pNew->pNext = p->pNext;
	p->pNext = pNew;
	return 0;
}

int add_button(PNODE *pHead, struct button *button) {
	PNODE scan = *pHead;
	PNODE pnew = (PNODE)malloc(sizeof(NODE));

	if (pnew == NULL) {
		printf("动态申请内存失败\n");
		return (-1);
	}
	pnew->widget.button = button;
	pnew->pNext = NULL;
	if (scan == NULL){
		*pHead = pnew;
	} else {
		while(scan->pNext != NULL)
			scan = scan->pNext;
		scan->pNext = pnew;
	}
	return 0;
}

int add_keyboard(PNODE *pHead, struct keyboard *keyboard) {
	PNODE scan = *pHead;
	PNODE pnew = (PNODE)malloc(sizeof(NODE));

	if (pnew == NULL) {
		printf("动态申请内存失败\n");
		return (-1);
	}
	pnew->widget.keyboard = keyboard;
	pnew->pNext = NULL;
	if (scan == NULL){
		*pHead = pnew;
	} else {
		while(scan->pNext != NULL)
			scan = scan->pNext;
		scan->pNext = pnew;
	}
	return 0;
}

int delete_list(PNODE pHead, int pos) {
	int i = 0;
	PNODE p = pHead;

	while (p->pNext != NULL && i < pos - 1) {
		p = p->pNext;
		++i;
	}
	if (i > pos - 1 || p->pNext == NULL)
		return -1;
	PNODE q = p->pNext;

	p->pNext = p->pNext->pNext;
	free(q);
	q = NULL;

	return 0;
}
