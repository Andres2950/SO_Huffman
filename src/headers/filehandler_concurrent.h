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
    char* binary = huffman_translate(cfa->td->content[i], cfa->td->file_sizes[i], cfa->dict);
    cfa->td->b_content[i] = binary;
    cfa->td->b_content_sizes[i] = strlen(binary);
    free(cfa);
}


// Traduce el contenido de cada archivo de TargetDir usando el diccionario
// La representacion codificada se guarda como un string en b_content
int targetdir_compress_concurrent(TargetDir* td, char **dict){
    td->b_content = malloc(sizeof(char*) * td->n_files);
    td->b_content_sizes = malloc(sizeof(size_t) * td->n_files);

    pthread_t* pid = malloc(sizeof(pthread_t) * td->n_files);

    for(int i = 0; i<td->n_files; i++){
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
}

#endif
