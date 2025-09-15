#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUM_SYMBOLS 256


typedef struct Node{
  char ch;
  int freq;
  struct Node *left, *right, *next;
} Node;

typedef struct {
  Node *first;
  int size;
} LinkedList;


int* new_frequency_table();
void frequency_table_add_text(int *ft, char *text);
void frequency_table_print(int *ft);
void init_linked_list(LinkedList *list);
void linked_list_insert(LinkedList *list, Node *node);
void linked_list_insert_bulk(LinkedList *list, int *ft);
void linked_list_print(LinkedList *list);
Node* create_huffman_binary_tree(LinkedList *list);
char** huffman_create_dictionary(Node *tree);
void huffman_binary_tree_print(Node *root, int depth);

#ifndef HUFFMAN_C

void init_linked_list(LinkedList *list){
  list->first = 0;
  list->size = 0;
}

void linked_list_insert(LinkedList *list, Node *node){
  Node *tmp;

  if(list->first == 0){
    list->first = node;
    list->size++;
  } else if(node->freq < list->first->freq){
    node->next = list->first;
    list->first = node;
    list->size++;
  } else {
    tmp = list->first;
    while(tmp->next && tmp->next->freq <= node->freq)
      tmp = tmp->next;
    node->next = tmp->next;
    tmp->next = node;
    list->size++;
  }
}

void linked_list_insert_bulk(LinkedList *list, int *ft){
  Node *new_node;
  for (int i = 0; i < NUM_SYMBOLS; ++i){
    if(ft[i] > 0){
      new_node = malloc(sizeof(Node));
      new_node->ch = i;
      new_node->freq = ft[i];
      new_node->left = 0;
      new_node->right = 0;
      new_node->next = 0;

      linked_list_insert(list, new_node);
    }
  }
}

void linked_list_print(LinkedList *list){
  Node *tmp = list->first;
  while(tmp){
    printf("\tCharacter: %c Frequency: %d\n", tmp->ch, tmp->freq);
    tmp = tmp->next;
  }
}

Node* linked_list_remove_first(LinkedList *list){
  Node *tmp = 0;
  if (list->first){
    tmp = list->first;
    list->first = tmp->next;
    tmp->next = 0;
    list->size--;
  }
  return tmp;
}

Node* create_huffman_binary_tree(LinkedList *list){
  Node *new_node, *first, *second;
  while (list->size > 1){
    first = linked_list_remove_first(list);
    second = linked_list_remove_first(list);
    new_node = malloc(sizeof(Node));
    new_node->ch = '+';
    new_node->freq = first->freq + second->freq;
    new_node->next = 0;
    new_node->right = first;
    new_node->left = second;
    linked_list_insert(list, new_node);
  }
  return list->first;
}


int huffman_binary_tree_get_depth(Node *root){
  if (!root){
    return -1;
  }
  int left, right;

  left = huffman_binary_tree_get_depth(root->left) + 1;
  right = huffman_binary_tree_get_depth(root->right) + 1;
  
  if (left > right)
    return left;
  else
    return right;
}

void huffman_binary_tree_print(Node *root, int depth){
  if (root->left == 0 && root->right == 0){
    printf("\tLetter: %c\tDepth: %d\n", root->ch, depth);
  } else {
    huffman_binary_tree_print(root->left, depth + 1);
    huffman_binary_tree_print(root->right, depth + 1);
  }
}


void huffman_fill_dictionary(Node *root, char **dictionary, char *code, int depth){
  if(root->left == 0 && root->right == 0)
    strcpy(dictionary[root->ch], code);
  else {
    char left[depth];
    char right[depth];

    strcpy(left, code);
    strcpy(right, code);

    strcat(left, "0");
    strcat(right, "1");

    huffman_fill_dictionary(root->left, dictionary, left, depth);
    huffman_fill_dictionary(root->right, dictionary, right, depth);

  }
}


char** huffman_create_dictionary(Node *tree){
  char **dictionary = malloc(sizeof(char *) * NUM_SYMBOLS);
  int depth = huffman_binary_tree_get_depth(tree) + 1;
  for (int i = 0; i < NUM_SYMBOLS; ++i){
    dictionary[i] = calloc(depth, sizeof(char));
  }
  huffman_fill_dictionary(tree, dictionary, "", depth);
  return dictionary;
}





int* new_frequency_table(){
  int i;
  int *ft = malloc(4 * NUM_SYMBOLS);
  for (i = 0; i < NUM_SYMBOLS; ++i)
    ft[i] = 0;
  return ft;
}

void frequency_table_add_text(int *ft, char *text){
  for(int i = 0; text[i] != '\0'; ++i){
    ft[text[i]]++;
  }
}

void frequency_table_print(int *ft){
  for(int i = 0; i < NUM_SYMBOLS; ++i){
    if (ft[i] > 0)
      printf("\t%d = %d = %c\n", i, ft[i], i);
  }
}


#endif
#endif
