#pragma once

typedef struct list* List;   // Points to the first node of the list
typedef struct list_node* ListNode;   // Points to a node of the list

typedef void* Pointer;

typedef char* String;

#ifndef COMPAREFUNC
    #define COMPAREFUNC
    typedef int (*CompareFunc)(const void* a, const void* b);
#endif

typedef void (*DestrFunc)(Pointer value);


#define LIST_FIRST (ListNode)-2
#define LIST_LAST (ListNode)-1



// Given a compare and a destroy function, creates a new list
// and returns a pointer to it
List list_create(CompareFunc compare, DestrFunc destroy);


// Returns the size of the list
int list_size(List list);


// Returns the actual first node of the list, not the dummy node 
ListNode list_first(List list);


// Returns the dummy last node
ListNode list_last(List list);


// Returns the node after "node"
ListNode list_next(ListNode node);


// Inserts "value" after node "node"
void list_insert_next(List list, ListNode node, Pointer value);

// Removes the first actual node of the list
void list_remove(List list);

void list_remove_node(List list, ListNode node);


// Finds and return the node matching to "value", or if it doesn't exist NULL
ListNode list_find_node(List list, Pointer value);

// Returns the value of a node
Pointer list_node_value(ListNode node);

// Destroys a list 
void list_destroy(Pointer l);


