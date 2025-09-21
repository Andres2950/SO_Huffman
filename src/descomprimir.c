#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "headers/huffman.h"
#include "headers/filehandler.h"
#include <locale.h>
#include <getopt.h>
#include <pthread.h>
#include <string.h>

#define EXEC_MODE_SERIAL 0
#define EXEC_MODE_PARALLEL 1
#define EXEC_MODE_CONCURRENT 2

// Usage: dehuff [options] src dst
// Decompress src file into dst directory.
// Only handles files created by the huff program.
//
// Options: 
//          -s serial
//          -p parallel using fork
//          -c concurrent using pthread
void serial_handler(FILE *f, char *dst, Node *huffman_tree) {
    char filename[PATH_MAX];
    int total_bits;
    while(fscanf(f, "%1023[^\t]\t", filename) == 1){
        if(fscanf(f, "%d\t", &total_bits) != 1) break;
        
        //Obtener nombres del archivo
        char path[PATH_MAX];
        strcpy(path, dst);
        strcat(path, "/");
        strcat(path, filename);        

        //Obtener el contenido del file
        huffman_decompress_bits(huffman_tree, f, total_bits, path);
        fgetc(f); //Consule \n
    }
}

void concurrent_handler(FILE *f, char *dst, Node *huffman_tree, char *src_path){
    char filename[PATH_MAX];
    int total_bits;
    int pt_index = 0;
    int size = 128;
    pthread_t *threads = malloc(sizeof(pthread_t)*size);
    huffman_args* args;
    
    while(fscanf(f, "%1023[^\t]\t", filename) == 1){
        if(fscanf(f, "%d\t", &total_bits) != 1) break;
        
        //Obtener nombres del archivo
        char path[PATH_MAX];
        strcpy(path, dst);
        strcat(path, "/");
        strcat(path, filename);        

        //Obtener el contenido del file
        args = malloc(sizeof(huffman_args));
        args->tree = huffman_tree;
        args->filename = strdup(path);
        args->src = src_path;
        args->offset = ftell(f);
        args->total_bits = total_bits;
        // Le pasamos la kk a un hilo
        pthread_create(&(threads[pt_index]), NULL, (void *)huffman_decompress_bits_void, (void *)args);

        pt_index++;
        if (pt_index >= size){
            size *= 2;
            threads = realloc(threads, sizeof(pthread_t)*size);
        }
        // para saltarnos el bloque y el \n
        int total_bytes = (total_bits + 7)/8;
        fseek(f, total_bytes+1, SEEK_CUR);
    }
    // cerramos todos los pthreads
    for (int i = 0; i < pt_index; ++i) {
        pthread_join(threads[i], NULL);
    }
}

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

    
    
    // Esto creo que se puede dejar serial 
    // No tiene sentido si se obtiene el diccionario y arbol con multyiples hilos
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

    // Una vez se tiene el arbol, empezamos los  y concurrent
    // Descomprimir y escribir archivos
    switch (execution_mode) {
        case EXEC_MODE_SERIAL:
            serial_handler(f, dst, huffman_tree);
            break;
        case EXEC_MODE_CONCURRENT:
            concurrent_handler(f, dst, huffman_tree, src);
            break;
        case EXEC_MODE_PARALLEL:
            printf("PArealel execvution mode not implemented");
            fclose(f);
            return -1;
            break;
        default:
            printf("Decompresion mode not found");
            fclose(f);
            return -1;
    }
   
    fclose(f);

    printf("dehuff: decompressed successfully \n");
    return 0;
}
