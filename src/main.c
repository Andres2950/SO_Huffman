#include <stdio.h>
#include <stdlib.h>
#include "headers/huffman.h"
#include <locale.h>

int *ft;

void hola_mundo(char *c){
  printf("Hola Mundo!\n");
}

int main(int argc, char* argv[]){
  
  setlocale(LC_ALL, "es_ES.UTF-8");

  LinkedList list;
  hola_mundo("printf");
  ft = new_frequency_table();
  frequency_table_add_text(ft, "COMO COME COCORITO COME COMO COSMONAUTA");
  //frequency_table_add_text(ft, "Vamos aprender a programa");
  frequency_table_print(ft);
  init_linked_list(&list);
  linked_list_insert_bulk(&list, ft);
  linked_list_print(&list);

  Node *huffman_tree = create_huffman_binary_tree(&list);
  char **dictionary = huffman_create_dictionary(huffman_tree);

  huffman_binary_tree_print(huffman_tree, 0);
  for (int i = 0; i < NUM_SYMBOLS; ++i){
    if(dictionary[i]){
      printf("\t%c   %s\n", i, dictionary[i]);
    }
  }

  return 0;
}
