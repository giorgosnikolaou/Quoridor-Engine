#include <stdlib.h>
#include "list.h"
#include <stdio.h>

struct list_node
{
    Pointer variable;
    ListNode next;
    
};

struct list
{
    ListNode* head;
    ListNode* last;  // To get the end
    CompareFunc compare;
    DestrFunc destroy;
    int size;
};


List list_create(CompareFunc compare, DestrFunc destroy)
{
    List list = malloc(sizeof(struct list));
    ListNode* first = malloc(sizeof(struct listnode**));
    ListNode* last = malloc(sizeof(struct listnode**));

    list->compare = compare;
    list->destroy = destroy;
    list->size = 0;

    list->head = first;    // When the list is empty there isn't a next node for the head node to point to 
	list->last = last;    // When the list is empty the head and last nodes match
    *list->head = NULL;
    *list->last = NULL;
    
    

    if (list == NULL)
    {
        printf("Not enough available memory to create the list!\n");
        exit(0);
    }


    return list;
}


int list_size(List list)
{
    return list->size;
}


ListNode list_first(List list) 
{
    return (list->size == 0) ? NULL : *list->head;
}


ListNode list_last(List list)
{
    return (list->size == 0) ? NULL : *list->last;
}


ListNode list_next(ListNode node) 
{
    return (node != NULL) ? node->next : NULL;
}


// Inserts node
void list_insert_next(List list, ListNode node, Pointer value) 
{
	
	ListNode new_node = malloc(sizeof(struct list_node));  // It doesnt let me put *ListNode inside sizeof() for some reason, but this works
    if (new_node == NULL)
    {
        printf("Not enough available memory to create the ListNode!\n");
        exit(0);
    }

	new_node->variable = value;

    if (list->size == 0) // If list is empty
    {
        *list->head = new_node;
        *list->last = new_node;
        new_node->next = NULL;
    }
    else
    {
        if (node == LIST_FIRST)
        {
            new_node->next = *list->head;
            *list->head = new_node;
        }
        else if (node == LIST_LAST || node == *list->last) // Insert at the end 
        {   
            new_node->next = NULL;
            (*list->last)->next = new_node;
            *list->last = new_node;
        }
        else
        {
            new_node->next = node->next;
            node->next = new_node;
        }
    }


    (list->size)++;


}


// Removes the first node
void list_remove(List list) 
{	
	list_remove_node(list, LIST_FIRST);
}

void list_remove_node(List list, ListNode node)
{
    if (list->size == 0)
    {
        int* p = NULL;
        *p = 0;
        printf("List is empty, cannot remove node!\n");
        exit(0);
    }
    else if (list->size == 1)
    {
        node = *list->head;
        *list->head = NULL;
        *list->last = NULL;
    }
    else
    {
        if (node == LIST_FIRST|| node == *list->head)
        {
            node = *list->head;
            *list->head = (*list->head)->next;
        }
        else if (node == LIST_LAST || node == *list->last)
        {
            ListNode tnode = *list->head;
            ListNode prev_node = *list->last;

            while (tnode != *list->last)
            {
                prev_node = tnode;
                tnode = tnode->next;
            }

            node = tnode;
            *list->last = prev_node;
            prev_node->next = NULL;
        }
        else
        {
            ListNode tnode = *list->head;
            ListNode prev_node = NULL;

            while (tnode != node)
            {
                prev_node = tnode;
                tnode = tnode->next;
            }

            prev_node->next = node->next;
            
        }
    }


    
    
    if (list->destroy != NULL)
        list->destroy(node->variable);
    
    free(node);

    (list->size)--;
}


ListNode list_find_node(List list, Pointer value) 
{
    if (list->compare == NULL)  // If there is a compare given function try to find the value
    {
        printf("Cannot search for an item when there isn't a compare function defined!\n");
        exit(0);
    }

    for (ListNode node = *list->head;
        node != NULL;
        node = node->next)
    {
        if (list->compare(value, node->variable) == 0)    // If it is found we return the node
            return node;		
    }
    
	
	return NULL;	// Else we return NULL
}


Pointer list_node_value(ListNode node)   // List list isn't needed but I cant bother changing it now
{
	return (node != NULL) ? node->variable : NULL;
}


void list_destroy(Pointer l)
{
    List list = (List)l;
    ListNode node = *list->head;
    while (node != NULL) // we dont call list_remove beacuse we dont care about the next node of the one we free
    {				
        ListNode next = node->next;	
        

        if (list->destroy != NULL)
            list->destroy(node->variable);
        
        free(node);
        node = next;
    }

    free(list->last);           
    free(list->head);
    free(list);
    
}



