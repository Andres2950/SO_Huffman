#include <stdio.h>
#include <stdlib.h>
#include "headers/huffman.h"
#include "headers/filehandler.h"
#include <locale.h>

int *ft;

void hola_mundo(char *c){
  printf("Hola Mundo!\n");
}

int main(int argc, char* argv[]){
    setlocale(LC_ALL, "es_ES.UTF-8");
    hola_mundo("printf");

    LinkedList list;
    ft = new_frequency_table();
    wchar_t *texto_perron = L"COMO COME COCORITO COME COMO COSMONAUTA áéíóúñ";
    printf("---------------------- cadena -----------------\n");
    for (int i = 0; texto_perron[i] != L'\0'; i++){
        printf("char: '%lc', unicode: %u\n", texto_perron[i],texto_perron[i]);
    }

    frequency_table_add_text(ft, texto_perron);
    printf("---------------------- FRACUENCY TABLE -----------------\n");
    frequency_table_print(ft);
    init_linked_list(&list);
    linked_list_insert_bulk(&list, ft);
    printf("-------------------------- LINKED LIST --------------------\n");
    linked_list_print(&list);

    Node *huffman_tree = create_huffman_binary_tree(&list);
    char **dictionary = huffman_create_dictionary(huffman_tree);

    printf("--------------------------- TREE ---------------------------\n");
    huffman_binary_tree_print(huffman_tree, 0);
    printf("---------------------------------- DICCIONARIO --------------------------\n");
    huffman_dictionary_print(dictionary);

    return 0;
}
