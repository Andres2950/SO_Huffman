
#include <stdio.h>
#include <stdlib.h>
#include "headers/huffman.h"
#include "headers/filehandler.h"
#include <locale.h>
#include <getopt.h>

#define EXEC_MODE_SERIAL 0
#define EXEC_MODE_PARALLEL 1
#define EXEC_MODE_CONCURRENT 2

// Use comprimir [options] src dst
// 
// src must be a directory
//
// Options: 
//          -s serial
//          -p parallel using pthread
//          -c concurrent using fork
int main(int argc, char** argv){
    //Setup
    setlocale(LC_ALL, "es_ES.UTF-8");
    
    char* src;
    char* dst;
    int execution_mode;

    //Input
    int opterr = 0;
    int c;
    
    
    while((c = getopt(argc, argv, "spc")) != -1){
        printf("%d", c);
    }

    for (size_t i = 0; i < argc; i++){
        printf("%s", argv[i]);
    }
    

    



    // // Leer src todos los archivos en src (nombre y contenido)
    // TargetDir* td = read_targetdir(src);  
    // //Hacer Huffman con el contenido de todos lo archivos    
    // size_t text_size = 0;
    // for(int i = 0; i< td->n_files; i++){
    //     text_size += td->file_sizes[i];
    // }
    
    // unsigned char* text = malloc(text_size);
    // size_t offset = 0;
    // for(int i = 0; i < td->n_files; i++){
    //     memcpy(text + offset, td->content[i], td->file_sizes[i]);
    //     offset += td->file_sizes[i];
    // }

    // //Huffman    
    // int* ft = new_frequency_table();
    // frequency_table_add_text(ft, text, text_size);            
    // LinkedList list;
    // init_linked_list(&list);
    // linked_list_insert_bulk(&list, ft);        

    // Node *huffman_tree = create_huffman_binary_tree(&list);
    // char **dictionary = huffman_create_dictionary(huffman_tree);    
    // huffman_dictionary_print(dictionary);
    
    // free(text);
    // free(ft);
    
    // // Escribir el archivo comprimido  
    // targetdir_compress(td, dictionary);
    // targetdir_write("./zip.huf", td, dictionary);
    // return 0;  
}