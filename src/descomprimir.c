
#include <stdio.h>
#include <stdlib.h>
#include "headers/huffman.h"
#include "headers/filehandler.h"
#include <locale.h>
#include <getopt.h>

#define EXEC_MODE_SERIAL 0
#define EXEC_MODE_PARALLEL 1
#define EXEC_MODE_CONCURRENT 2

// Usage: dehuff [options] src dst
// Decompress src file into dst directory.
// Only handles files created by the huff program.
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
        printf("dehuff: too many arguments.\n");
        return 1;
    } 

    if(optind == argc){
        printf("dehuff: missing source directory.\n");        
        return 1;
    }
    src = argv[optind];

    if(optind+1 == argc){
        printf("dehuff: missing destination file.\n");
        return 1;
    }
    dst = argv[optind+1];    

    
    
    // Program   

    
    FILE *f = fopen(src, "rb");
    if(!f) return -1;
    
    //Leer diccionario
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

    //Reconstruir arbol    
    Node *huffman_tree = rebuild_huffman_tree(dictionary);
    
    //Crear carpeta
    struct stat st = {0};
    if (stat(dst, &st) == -1) {
        mkdir(dst, 0700);
    }

    // Descomprimir y escribir archivos
    char filename[PATH_MAX];
    int total_bits;
    while(fscanf(f, "%1023[^\t]\t", filename) == 1){
        if(fscanf(f, "%d\t", &total_bits) != 1) break;
        
        char path[PATH_MAX];
        strcpy(path, dst);
        strcat(path, "/");
        strcat(path, filename);        

        huffman_decompress_bits(huffman_tree, f, total_bits, path);
        fgetc(f);
    }
    fclose(f);

    printf("dehuff: decompressed successfully \n");
    return 0;
}