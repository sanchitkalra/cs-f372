#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    int data;
    struct Node* next;
};

struct Node* addFront(struct Node* head, int data) {
    if (head == NULL) {
        // new linkedlist
        struct Node* v = (struct Node*)malloc(sizeof(struct Node));
        v->data = data;
        v->next = NULL;
        head = v;
    } else {
        struct Node* v = (struct Node*)malloc(sizeof(struct Node));
        v->data = data;
        v->next = head;
        head = v;
    }

    return head;
}

void addToFront(struct Node** headRef, int data) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    newNode->data = data;
    newNode->next = *headRef;
    *headRef = newNode;
}