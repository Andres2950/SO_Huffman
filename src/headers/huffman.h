#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
//#define NUM_SYMBOLS 256 
#define MAX_UNICODE 0x110000 // El unicode maximo es 10FFFF, etonces necesitmaos un arreglo de 10FFFF+1
#define MAX_BYTE 256

typedef struct Node{
  unsigned char ch;
  int freq;
  struct Node *left, *right, *next;
} Node;

typedef struct {
  Node *first;
  int size;
} LinkedList;

typedef struct {
    Node *tree;
    char *filename; // archivo de salida 
    char *src;      // archivo de contenido
    long int offset; // donde va a empezar a leer el archivo
    size_t total_bits;
} huffman_args;

////////////////////////////////// -------------------------------
int* new_frequency_table();
void frequency_table_add_text(int *ft, unsigned char *text, size_t size);
void frequency_table_print(int *ft);
void init_linked_list(LinkedList *list);
void linked_list_insert(LinkedList *list, Node *node);
void linked_list_insert_bulk(LinkedList *list, int *ft);
void linked_list_print(LinkedList *list);
Node* create_huffman_binary_tree(LinkedList *list);
char** huffman_create_dictionary(Node *tree);
void huffman_dictionary_print(char **dict);
void huffman_binary_tree_print(Node *root, int depth);
Node* rebuild_huffman_tree(char **dict);
void huffman_decompress_bits(Node *tree, FILE *input, size_t total_bits, char *filename);
void *huffman_decompress_bits_void(void *args);
// ----------------------------------------------------
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

Node* create_node(){
  Node *node = malloc(sizeof(Node));
  node->ch = '+';
  node->freq = 0;
  node->left = 0;
  node->right = 0;
  node->next = 0;
  return node;
}

Node* rebuild_huffman_tree(char **dict){
  Node *root = create_node();

  for(int i = 0; i < MAX_BYTE; ++i){
    if(dict[i] && dict[i][0] != '\0'){
      Node *current = root;
      char *code = dict[i];
      int n_code = strlen(code);

      for(int j = 0; j < n_code; ++j){
        if(code[j] == '0'){
          if(!current->left)
            current->left = create_node();
          current = current->left;
        } else if (code[j] == '1'){ 
          if(!current->right)
            current->right = create_node();
          current = current->right;
        }
      }
      current->ch = i;
    }
  }
  return root;
}

// No nos sirve pasar por parametro el mismo puntero a FILE 
// Seia desatstroso pq cualdo se hace fread mueve el puintero al sigueinte char a leer 
void *huffman_decompress_bits_void(void* arg){
    huffman_args *xd = (huffman_args *) arg;
    FILE *input = fopen(xd->src, "rb");
    fseek(input, xd->offset, SEEK_SET); // saltamos al lugar que queremos leer
    FILE *out = fopen(xd->filename, "wb");
    Node *tree = xd->tree;
    size_t total_bits = xd->total_bits;

    Node *current = tree;
    size_t bits_read = 0;
    unsigned char byte;

    while(bits_read < total_bits && fread(&byte, 1, 1, input) == 1){
        for(int b = 7; b >= 0 && bits_read < total_bits; --b){
        int bit = (byte >> b) & 1;
        if(bit == 0) 
            current = current->left;
        else
            current = current->right;
        if(current->left == 0 && current->right == 0){
            fputc(current->ch, out);
            current = tree;
        }
        bits_read++;
        }
    }
    fclose(out);
    fclose(input);
    
    free(xd->filename);
    free(xd);
    return NULL; 
}

void huffman_decompress_bits_by_pos(Node *tree, char *file_input, size_t total_bits, char *file_out, long offset){
  FILE *input = fopen(file_input, "rb");
  fseek(input, offset, SEEK_SET);

  FILE *out = fopen(file_out, "wb");
  Node *current = tree;
  size_t bits_read = 0;
  unsigned char byte;

  while(bits_read < total_bits && fread(&byte, 1, 1, input) == 1){
    for(int b = 7; b >= 0 && bits_read < total_bits; --b){
      int bit = (byte >> b) & 1;
      if(bit == 0) 
        current = current->left;
      else
        current = current->right;
      if(current->left == 0 && current->right == 0){
        fputc(current->ch, out);
        current = tree;
      }
      bits_read++;
    }    
  }  
  fclose(input);
  fclose(out);
  free(file_out);
}

void huffman_decompress_bits(Node *tree, FILE *input, size_t total_bits, char *filename){

  FILE *out = fopen(filename, "wb");
  Node *current = tree;
  size_t bits_read = 0;
  unsigned char byte;

  while(bits_read < total_bits && fread(&byte, 1, 1, input) == 1){
    for(int b = 7; b >= 0 && bits_read < total_bits; --b){
      int bit = (byte >> b) & 1;
      if(bit == 0) 
        current = current->left;
      else
        current = current->right;
      if(current->left == 0 && current->right == 0){
        fputc(current->ch, out);
        current = tree;
      }
      bits_read++;
    }    
  }  
  fclose(out);
}


void linked_list_insert_bulk(LinkedList *list, int *ft){
  Node *new_node;
  for (int i = 0; i < MAX_BYTE; ++i){
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
  if (!root) return -1;
  
  int left, right;

  left = huffman_binary_tree_get_depth(root->left) + 1;
  right = huffman_binary_tree_get_depth(root->right) + 1;
  
  return (left > right) ? left : right;
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
  char **dictionary = malloc(sizeof(char *) * MAX_BYTE);
  int depth = huffman_binary_tree_get_depth(tree) + 1;
  for (int i = 0; i < MAX_BYTE; ++i){
    dictionary[i] = calloc(depth, sizeof(char));
  }
  huffman_fill_dictionary(tree, dictionary, "", depth);
  return dictionary;
}



int* new_frequency_table(){
  int *ft = malloc(sizeof(int) * MAX_BYTE);
  for (int i = 0; i < MAX_BYTE; ++i)
    ft[i] = 0;
  return ft;
}

void frequency_table_add_text(int *ft, unsigned char *text, size_t size){
  for(size_t i = 0; i < size; ++i){
    ft[text[i]]++;
  }
}

void frequency_table_print(int *ft){
  for(int i = 0; i < MAX_BYTE; ++i){
    if (ft[i] > 0)
      printf("\ti: %d = ft[i]: %d = char: %c\n", i, ft[i], i);
  }
}

void huffman_dictionary_print(char **dict){
    for (int i = 0; i < MAX_BYTE; ++i){
        if(dict[i] && dict[i][0] != '\0'){
      }
    }
  }

char* huffman_translate(unsigned char* text, int size, char** dictionary){
  char* binary_code = calloc(2, sizeof(char));
  int len = 0;

  for(int i=0; i<size; i++){
    char* code = dictionary[text[i]];
    int code_len = strlen(code);
    binary_code = realloc(binary_code, len + code_len + 1);
    strcpy(binary_code + len, code);
    len += code_len;
  }

  return binary_code;
}

#endif
#endif
