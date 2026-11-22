#pragma once

#include "pch.h"


typedef enum Action {
    UNDEFINED,
    READ,
    WRITE
} Action;

typedef struct Node {
    Node* next;
    Action action;
    std::string address;
} Node;


typedef struct AddressQueue {

    Node* head;
    Node* tail;
    int   size;

} AddressQueue;


AddressQueue* createQueue();

void enqueue(AddressQueue* queue, std::string address, Action action);
Node* dequeue(AddressQueue* queue);

void freeNode(Node*& node);