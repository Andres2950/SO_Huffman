#include <stdio.h>
#include <stdlib.h>
#include "headers/huffman.h"
#include "headers/filehandler.h"
#include <locale.h>

int *ft;

int comprimir(char* src, char* dst){
    // Leer src todos los archivos en src (nombre y contenido)
    TargetDir* td = read_targetdir(src);  
    //Hacer Huffman con el contenido de todos lo archivos    
    size_t text_size = 0;
    for(int i = 0; i< td->n_files; i++){
        text_size += td->file_sizes[i];
    }
    
    unsigned char* text = malloc(text_size);
    size_t offset = 0;
    for(int i = 0; i < td->n_files; i++){
        memcpy(text + offset, td->content[i], td->file_sizes[i]);
        offset += td->file_sizes[i];
    }

    //Huffman    
    int* ft = new_frequency_table();
    frequency_table_add_text(ft, text, text_size);            
    LinkedList list;
    init_linked_list(&list);
    linked_list_insert_bulk(&list, ft);        

    Node *huffman_tree = create_huffman_binary_tree(&list);
    char **dictionary = huffman_create_dictionary(huffman_tree);    
    huffman_dictionary_print(dictionary);
    
    free(text);
    free(ft);
    
    // Escribir el archivo comprimido  
    targetdir_compress(td, dictionary);
    targetdir_write("./zip.huf", td, dictionary);
    return 0;  
}


int unzip(const char* src){
  FILE *f = fopen(src, "rb");
  if(!f) return -1;
  
  int size_dict = 0;
  fread(&size_dict, sizeof(int), 1, f);

  char **dictionary = malloc(sizeof(char*) * 256);
  for (int i = 0; i < 256; ++i) dictionary[i] = NULL;

  for (int i = 0; i < size_dict; ++i) {
    unsigned char byte;
    int code_len;
    fread(&byte, sizeof(unsigned char), 1, f);
    fread(&code_len, sizeof(int), 1, f);

    char *code_str = malloc(code_len + 1);
    fread(code_str, sizeof(char), code_len, f);
    code_str[code_len] = '\0';
    dictionary[byte] = code_str;
  }
  fgetc(f);
  
  Node *huffman_tree = rebuild_huffman_tree(dictionary);
  char filename[1024];
  int total_bits;
  while(fscanf(f, "%1023[^\t]\t", filename) == 1){
    if(fscanf(f, "%d\t", &total_bits) != 1) break;
  
      huffman_decompress_bits(huffman_tree, f, total_bits, filename);
      fgetc(f);
  }
  fclose(f);
  return 0;
}

int main(int argc, char* argv[]){
    setlocale(LC_ALL, "es_ES.UTF-8");
    comprimir("./ej", "");
    unzip("./zip.huf");
    return 0;
}
