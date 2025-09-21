#ifndef FILEHANDLER_CONCURRENT_H
#define FILEHANDLER_CONCURRENT_H

#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <sys/stat.h>
#include <dirent.h>
#include <linux/limits.h>
#include <math.h>
#include <filehandler.h>
#include <pthread.h>

#endif

#ifndef FILEHANDLER_CONCURRENT_C
#define FILEHANDLER_CONCURRENT_C


typedef struct {
    TargetDir* td;
    int index;
} read_file_arg;


void* read_file(void* arg){
    read_file_arg* tdc = arg;

    char file_path[PATH_MAX];
    sprintf(file_path, "%s/%s", tdc->td->path, tdc->td->filenames[tdc->index]);
    size_t filesize = get_filesize(file_path);
    
    FILE* f = fopen(file_path, "rb");
    unsigned char *content = malloc(sizeof(unsigned char) * filesize);
    fread(content, 1, filesize, f);
    tdc->td->content[tdc->index] = content;
    tdc->td->file_sizes[tdc->index] = filesize;
    fclose(f);    
    free(tdc);

    return NULL;
}



TargetDir* read_targetdir_concurrent(char* path){    
    // Confirmar que path es un directorio    
    if(isdir(path)){
        return NULL;
    }
        
    TargetDir* td = malloc(sizeof(TargetDir));
    td->path = path;
    td->n_files = 0; 
    td->filenames = NULL;

    //Obtener archivos de directorio
    DIR* d = opendir(path);
    struct dirent* file;
    file = readdir(d);
    while(file!=NULL){        
        char file_path[PATH_MAX];
        sprintf(file_path, "%s/%s", path, file->d_name);
        if(isfile(file_path)){
            // Agregar archivo a TargetDir            
            td->n_files++;
            td->filenames = realloc(td->filenames, sizeof(char*) * td->n_files);
            td->filenames[td->n_files-1] = malloc(sizeof(char)*(PATH_MAX+1));
            strcpy(td->filenames[td->n_files-1], file->d_name);
        }
        
        file = readdir(d);
    }    
    closedir(d);

    //Leer el contenido de los archivos
    td->content = malloc(sizeof(unsigned char*) * td->n_files);
    td->file_sizes = malloc(sizeof(size_t) * td->n_files);

    pthread_t* pid = malloc(sizeof(pthread_t) * td->n_files);

    for(int i = 0; i<td->n_files; i++){
        read_file_arg* tdc = malloc(sizeof(read_file_arg));
        tdc->index = i;
        tdc->td = td;

        pthread_create(&pid[i], NULL, read_file, (void*) tdc);
    }
    
    for(int i = 0; i<td->n_files; i++){
       pthread_join(pid[i], NULL);
    }    
    free(pid);

    return td;
}


typedef struct{
    TargetDir* td;
    char** dict;
    int index;
} compress_file_arg;


void* compress_file(void* arg){        
    compress_file_arg* cfa = arg;    
    int i = cfa->index;
    char cero = 0;
    char uno = 1;
    //The binary size will not be more than the uncompressed size     
    char* binary = malloc(cfa->td->file_sizes[i]); 

    char byte = 0;
    int byte_counter=0;
    int bit_counter =0;
    size_t total_bit_size = 0;
    char bit;
    
    // por cada caracter
    for(size_t j=0; j<cfa->td->file_sizes[i]; j++){
        int caracter = cfa->td->content[i][j];
        //por cada bit de codigo
        for(size_t k=0; k<strlen(cfa->dict[caracter]); k++){
            char bitchar = cfa->dict[caracter][k];
            //Insertar el bit
            if(bitchar=='0'){   
                bit = cero;            
            }else if(bitchar=='1'){
                bit = uno;
            }else{
                return (void*)-1; //str invalido
            }
            byte = byte << 1;
            byte = byte | bit;
            bit_counter++;

            //Escribe el byte si ya esta lleno
            if(bit_counter==8){
                binary[byte_counter] = byte;
                bit_counter=0;
                byte_counter++;
            }              
            total_bit_size++;  
        }   
        //Escribe el byte si es el ultimo
        if(j==cfa->td->file_sizes[i]-1 && bit_counter!=0){
            //Rellenar con ceros
            while(bit_counter!=8){
                byte = byte << 1;
                bit_counter++;
            }
            binary[byte_counter] = byte;
            byte_counter++;
        }
    }
    
    cfa->td->b_content[i] = binary;
    cfa->td->b_content_sizes[i] = total_bit_size; 
    free(cfa);

    return NULL;
}

// Traduce el contenido de cada archivo de TargetDir usando el diccionario
// La representacion codificada se guarda como binario en b_content
// La cantidad de bits de codigo se guarda en b_content_size
int targetdir_compress_concurrent(TargetDir* td, char **dict){
    td->b_content = malloc(sizeof(char*) * td->n_files);
    td->b_content_sizes = malloc(sizeof(size_t) * td->n_files);

    pthread_t* pid = malloc(sizeof(pthread_t) * td->n_files);

    // por cada archivo
    for(int i=0; i < td->n_files; i++){           
        compress_file_arg* cfa = malloc(sizeof(compress_file_arg));
        cfa->index = i;
        cfa->td = td;
        cfa->dict = dict;

        pthread_create(&pid[i], NULL, compress_file, (void*) cfa);
    }

    for(int i = 0; i<td->n_files; i++){
       pthread_join(pid[i], NULL);
    }    
    free(pid); 
    return 0;
}

#endif
