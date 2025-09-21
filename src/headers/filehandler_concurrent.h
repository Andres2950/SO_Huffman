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
} TargetDirConcurrent;


void* read_file(void* arg){
    TargetDirConcurrent* tdc = arg;

    char file_path[PATH_MAX];
    sprintf(file_path, "%s/%s", tdc->td->path, tdc->td->filenames[tdc->index]);
    FILE* f = fopen(file_path, "rb");
    fseek(f, 0, SEEK_END);
    size_t filesize = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char *content = malloc(sizeof(unsigned char) * filesize);
    fread(content, 1, filesize, f);
    tdc->td->content[tdc->index] = content;
    tdc->td->file_sizes[tdc->index] = filesize;
    fclose(f);
    printf("%d", tdc->index);
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
        TargetDirConcurrent* tdc = malloc(sizeof(TargetDir));
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


#endif
