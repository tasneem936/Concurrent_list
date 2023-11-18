#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "concurrent_list.h"

struct node {
    int value;
    pthread_mutex_t node_mutex;
    node *next;
};

struct list {
    node *head;
    pthread_mutex_t list_mutex;
};

//////////////////////////////////////////////////////////////
void print_node(node* node)
{
    if(node)
        printf("%d ", node->value);
}
//******************************//
list* create_list()
{
    list* List=(list*)malloc(sizeof(list));
    List->head=NULL;
    pthread_mutex_init(&(List->list_mutex),NULL);
    return List;

}
//*****************************//
void delete_list(list* list)
{
    if(!list){
        return;
    }
    pthread_mutex_lock(&(list->list_mutex));
    if(list->head){
        pthread_mutex_lock(&(list->head->node_mutex));
        node* curr=list->head;
        pthread_mutex_unlock(&(list->list_mutex));
        pthread_mutex_destroy(&(list->list_mutex));
        free(list);

        node* temp;
        while(curr) {
            if(!(curr->next)){
                pthread_mutex_unlock(&(curr->node_mutex));
                pthread_mutex_destroy(&(curr->node_mutex));
                free(curr);
                break;
            }
            temp=curr;
            pthread_mutex_lock(&(curr->next->node_mutex));
            curr=curr->next;
            pthread_mutex_unlock(&(temp->node_mutex));
            pthread_mutex_destroy(&(temp->node_mutex));
            free(temp);
        }
        return;
    }
    pthread_mutex_unlock(&(list->list_mutex));
    pthread_mutex_destroy(&(list->list_mutex));
    free(list);
}

//*****************************//

void insert_value(list* list, int value)
{
    if(!list)
        return;
    pthread_mutex_lock(&(list->list_mutex));

    node* new_node = (node*)malloc(sizeof(node));;
    pthread_mutex_init(&new_node->node_mutex, NULL);
    new_node->value=value;

    if(list->head == NULL){ // empty list
        list->head=new_node;
        new_node->next=NULL;
        pthread_mutex_unlock(&(list->list_mutex));
        return;
    }

    pthread_mutex_lock(&(list->head->node_mutex));
    node* curr = list->head;
    if(curr->value >= value){ // add to the beginning
        new_node->next =curr;
        list->head = new_node;
        pthread_mutex_unlock(&(list->list_mutex));
        pthread_mutex_unlock(&(curr->node_mutex));
        return;
    }

    pthread_mutex_unlock(&(list->list_mutex));
    node* prev;
    while(curr->next){
        pthread_mutex_lock(&(curr->next->node_mutex));
//        for(long long i = 0; i < 100000000; i++){}
//        for(long long i = 0; i < 100000000; i++){}

        prev = curr;
        curr = curr->next;
        if(curr->value >= value){
            prev->next = new_node;
            new_node->next = curr;
            pthread_mutex_unlock(&(prev->node_mutex));
            pthread_mutex_unlock(&(curr->node_mutex));
            return;
        }
        pthread_mutex_unlock(&(prev->node_mutex));
    }

    new_node->next=curr->next;
    curr->next=new_node;
    pthread_mutex_unlock(&(curr->node_mutex));
    return;

}
//*****************************//

void remove_value(list* list, int value) {
    if(!list)
        return;
    pthread_mutex_lock(&(list->list_mutex));
    int found = 0;
    if (list->head == NULL) { // empty list
        pthread_mutex_unlock(&(list->list_mutex));
        return;
    }
    pthread_mutex_lock(&(list->head->node_mutex));
    node *curr = list->head;
    if (curr->value == value) {
        list->head = curr->next;
        pthread_mutex_unlock(&(list->list_mutex));
        pthread_mutex_unlock(&(curr->node_mutex));
        pthread_mutex_destroy(&(curr->node_mutex));
        free(curr);
        return;
    }

    //the node that will be removed in the rest of the list.
    pthread_mutex_unlock(&(list->list_mutex));
    node *prev;
    while (curr->next) {
        pthread_mutex_lock(&(curr->next->node_mutex));
//        for(long long i = 0; i < _time; i++){}
        prev = curr;
        curr = curr->next;
        if (curr->value == value) {
            found = 1;
            prev->next = curr->next;
            pthread_mutex_unlock(&(prev->node_mutex));
            pthread_mutex_unlock(&(curr->node_mutex));
            pthread_mutex_destroy(&(curr->node_mutex));
            free(curr);
            break;
        }
        pthread_mutex_unlock(&(prev->node_mutex));
    }
    if(!found)
        pthread_mutex_unlock(&(curr->node_mutex));
    return;
}
//*****************************//

void print_list(list* list)
{
    if(!list){
        printf("\n"); // DO NOT DELETE
        return;
    }
    pthread_mutex_lock(&(list->list_mutex));

    if (list->head == NULL) // empty list
    {
        printf("\n"); // DO NOT DELETE
        pthread_mutex_unlock(&(list->list_mutex));
        return;
    }
    pthread_mutex_lock(&(list->head->node_mutex));
    pthread_mutex_unlock(&(list->list_mutex));
    node* curr = list->head;

    print_node(curr);

    while (curr->next)
    {
        pthread_mutex_lock(&(curr->next->node_mutex));
        pthread_mutex_unlock(&(curr->node_mutex));
        curr = curr->next;
        print_node(curr);
    }
    pthread_mutex_unlock(&(curr->node_mutex));

    printf("\n"); // DO NOT DELETE
    return;
}
//*****************************//

void count_list(list* list, int (*predicate)(int))
{
    int count = 0; // DO NOT DELETE
    //TODO: add your implementation
    if(!list){
        return;
    }
    pthread_mutex_lock(&(list->list_mutex));
    if(list->head==NULL){
        printf("%d items were counted\n", count); // DO NOT DELETE
        pthread_mutex_unlock(&(list->list_mutex));
        return;
    }
    node* curr=list->head;
    if(!(curr->next)){ //if single node
        if(predicate(curr->value)){
            count=1;
        }
        printf("%d items were counted\n", count); // DO NOT DELETE
        pthread_mutex_unlock(&(list->list_mutex));
        return;
    } else
    {
        pthread_mutex_lock(&(curr->node_mutex));
        pthread_mutex_unlock(&(list->list_mutex));
        while (curr){
            if(predicate(curr->value)){
                count++;
            }

            if(!(curr->next)){ //if this is the last element
                break;
            }
            pthread_mutex_lock(&(curr->next->node_mutex));
            pthread_mutex_unlock(&(curr->node_mutex));
            curr=curr->next;
        }
    }
    printf("%d items were counted\n", count); // DO NOT DELETE
    pthread_mutex_unlock(&(curr->node_mutex));
}
