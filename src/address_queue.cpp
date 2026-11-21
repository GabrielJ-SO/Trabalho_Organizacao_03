#include "address_queue.h"

Node* _createNode(std::string address, Action action) {
    Node* newNode = (Node*)calloc(1, sizeof(Node));

    newNode->address = address;
    newNode->action  =  action;
    newNode->next    =    NULL;

    return newNode;
}

AddressQueue* createQueue() {
    AddressQueue* queue = (AddressQueue*)calloc(1, sizeof(AddressQueue));

    queue->head = NULL;
    queue->tail = NULL;
    queue->size =    0;

    return queue;
}

void enqueue(AddressQueue* queue, std::string address, Action action) {
    Node *node = _createNode(address, action);

    if (queue->head == NULL) {
        queue->head = node;
        queue->tail = node;
    }else {
        queue->tail->next = node;
        queue->tail = node;
    }

    queue->size++;
}

Node* dequeue(AddressQueue* queue) {
    if (queue->size == 0) return NULL; 
    
    Node* node     = queue->head;
    Node* nodeCopy = _createNode(node->address, node->action);

    queue->head = node->next;

    if (queue->head == NULL) {
        queue->tail = NULL;
    }

    freeNode(node);
    
    queue->size--;

    return nodeCopy;
}

void freeNode(Node* node) {
    node->next = NULL;
    free(node);
}