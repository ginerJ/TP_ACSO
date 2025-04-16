#include "ej1.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

string_proc_list* string_proc_list_create(void){
    string_proc_list* list = (string_proc_list*)malloc(sizeof(string_proc_list));
    if(!list){
        fprintf(stderr, "Error: Fallo alocacion de memoria para la lista\n");
        return NULL;
    }
    list->first = NULL;
    list->last  = NULL;
    return list;
}

string_proc_node* string_proc_node_create(uint8_t type, char* hash){
    string_proc_node* node = (string_proc_node*)malloc(sizeof(string_proc_node));
    if(!node){
        fprintf(stderr, "Error: Fallo alocacion de memoria para el nodo\n");
        return NULL;
    }
    node->next = NULL;
    node->previous = NULL;
    node->type = type;
    node->hash = hash;
    return node;
}

void string_proc_list_add_node(string_proc_list* list, uint8_t type, char* hash){
    string_proc_node* node = string_proc_node_create(type, hash);
    if(!node)
        return;

    if(!list->first)
        list->first = node;
    else {
        list->last->next = node;
        node->previous = list->last;
    }

    list->last = node;
}

char* string_proc_list_concat(string_proc_list* list, uint8_t type, char* hash) {
    if (!list || !hash)
        return NULL;

    // Reemplazo de strdup
    char* dup = (char*)malloc(strlen(hash) + 1);
    if (!dup) {
        fprintf(stderr, "Error: No se logro duplicar el hash\n");
        return NULL;
    }
    strcpy(dup, hash);

    for (string_proc_node* current = list->first; current; current = current->next) {
        if (current->type == type) {
            char* temp = str_concat(dup, current->hash);
            free(dup);
            dup = temp;
        }
    }
    return dup;
}



/** AUX FUNCTIONS **/

void string_proc_list_destroy(string_proc_list* list){

    /* borro los nodos: */
    string_proc_node* current_node	= list->first;
    string_proc_node* next_node		= NULL;
    while(current_node != NULL){
        next_node = current_node->next;
        string_proc_node_destroy(current_node);
        current_node	= next_node;
    }
    /*borro la lista:*/
    list->first = NULL;
    list->last  = NULL;
    free(list);
}
void string_proc_node_destroy(string_proc_node* node){
    node->next      = NULL;
    node->previous	= NULL;
    node->hash		= NULL;
    node->type      = 0;
    free(node);
}

char* str_concat(char* a, char* b) {
    int totalLength = strlen(a) + strlen(b);
    char *result = (char *)malloc(totalLength + 1);
    strcpy(result, a);
    strcat(result, b);
    return result;
}

void string_proc_list_print(string_proc_list* list, FILE* file){
    uint32_t length = 0;
    string_proc_node* current_node  = list->first;
    while(current_node != NULL){
        length++;
        current_node = current_node->next;
    }
    fprintf( file, "List length: %d\n", length );
    current_node    = list->first;
    while(current_node != NULL){
        fprintf(file, "\tnode hash: %s | type: %d\n", current_node->hash, current_node->type);
        current_node = current_node->next;
    }
}