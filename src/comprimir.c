#include <stdio.h>
#include <stdlib.h>
#include "headers/huffman.h"
#include "headers/filehandler.h"
#include "filehandler_concurrent.h"
#include "filehandler_parallel.h"
#include <locale.h>
#include <getopt.h>

#define EXEC_MODE_SERIAL 0
#define EXEC_MODE_PARALLEL 1
#define EXEC_MODE_CONCURRENT 2

// Usage: huff [options] src dst
// Compress src directory into dst file.
// Only handles plaintext files.
//
// Options: 
//          -s serial
//          -p parallel using fork
//          -c concurrent using pthread
int main(int argc, char** argv){
    //Setup
    setlocale(LC_ALL, "es_ES.UTF-8");
    
    char* src;
    char* dst;
    int execution_mode = EXEC_MODE_SERIAL; //Default

    //Input
    //opterr = 0;
    int c;    
    
    while((c = getopt(argc, argv, "spc")) != -1){
        switch (c){
        case 's':
            execution_mode = EXEC_MODE_SERIAL;
            break;

        case 'p':
            execution_mode = EXEC_MODE_PARALLEL;
            break;

        case 'c':
            execution_mode = EXEC_MODE_CONCURRENT;
            break;
        
        case '?':
            return 1;
            break;
        
        default:        
            abort();
        }
    }

    if(argc > optind+2){
        printf("huff: too many arguments.\n");
        return 1;
    } 

    if(optind == argc){
        printf("huff: missing source directory.\n");        
        return 1;
    }
    src = argv[optind];

    if(optind+1 == argc){
        printf("huff: missing destination file.\n");
        return 1;
    }
    dst = argv[optind+1];    
    
    
    // Program      
    
    // Leer todos los archivos en directorio src (nombre y contenido)
    
    TargetDir* td;
    
    switch (execution_mode){
    case EXEC_MODE_SERIAL:
        td = read_targetdir(src);
        break;
    case EXEC_MODE_CONCURRENT:
        td = read_targetdir_concurrent(src);
        break;
    case EXEC_MODE_PARALLEL:
        td = read_targetdir_parallel(src);        
        break;
    default:
        printf("huff: execution mode not supported.\n");
        return 0;
        break;
    }    
        

    if(td == NULL){
        printf("huff: %s is not a directory.\n", src);
        return -1;
    }
    if(td->n_files == 0){        
        printf("huff: %s is empty.\n", src);
        return -1;
    }

    //Juntar el texto de los archivos       
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
    
    free(text);
    free(ft);    
    
    // Escribir el archivo comprimido  
    
    switch (execution_mode){
    case EXEC_MODE_SERIAL:
        targetdir_compress(td, dictionary);
        break;
    case EXEC_MODE_CONCURRENT:
        targetdir_compress_concurrent(td, dictionary);        
        break;
    case EXEC_MODE_PARALLEL:
        targetdir_compress_parallel(td, dictionary);
        break;
    default:
        printf("huff: execution mode not supported.\n");
        return 0;
        break;
    }    
    
    int r = targetdir_write(dst, td, dictionary);

    if(r){
        printf("huff: error occurred while opening or creating %s.\n", dst);
    }
    
    printf("huff: compressed successfully \n");
    return 0;  
}
