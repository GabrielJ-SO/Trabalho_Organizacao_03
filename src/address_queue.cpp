#include "address_queue.h"

Node* _createNode(std::string address, Action action) {
    Node* newNode = new Node();

    newNode->address = address;
    newNode->action  =  action;
    newNode->next    = nullptr;

    return newNode;
}

AddressQueue* createQueue() {
    AddressQueue* queue = new AddressQueue();

    queue->head = nullptr;
    queue->tail = nullptr;
    queue->size =    0;

    return queue;
}

void enqueue(AddressQueue* queue, std::string address, Action action) {
    Node *node = _createNode(address, action);

    if (queue->head == nullptr) {
        queue->head = node;
        queue->tail = node;
    }else {
        queue->tail->next = node;
        queue->tail = node;
    }

    queue->size++;
}

Node* dequeue(AddressQueue* queue) {
    if (queue->size == 0) return nullptr; 
    
    Node* node     = queue->head;
    queue->head = node->next;

    if (queue->head == nullptr) {
        queue->tail = nullptr;
    }
    Node* nodeCopy = _createNode(node->address, node->action);

    delete(node);
    queue->size--;

    return nodeCopy;
}

void freeQueue(AddressQueue*& queue) {
    if (queue != nullptr) {
        delete(queue);
        queue = nullptr;
    }
}

void freeNode(Node*& node) {
    if (node != nullptr) {
        delete(node);
        node = nullptr;
    }
}