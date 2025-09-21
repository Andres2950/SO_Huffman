#ifndef FILEHANDLER_PARALLEL_H
#define FILEHANDLER_PARALLEL_H

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
#include <sys/mman.h>
#include <unistd.h>
#include <sys/wait.h>

#endif

#ifndef FILEHANDLER_PARALLEL_C
#define FILEHANDLER_PARALLEL_C

#define MMAP_PROT PROT_WRITE | PROT_READ
#define MMAP_FLAG MAP_ANONYMOUS | MAP_SHARED


TargetDir* read_targetdir_parallel(char* path){    
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
    //td->content = malloc(sizeof(unsigned char*) * td->n_files);
    //td->file_sizes = malloc(sizeof(size_t) * td->n_files);
    td->content = mmap(NULL, sizeof(unsigned char*) * td->n_files, MMAP_PROT, MMAP_FLAG, -1, 0);
    td->file_sizes = mmap(NULL, sizeof(size_t) * td->n_files, MMAP_PROT, MMAP_FLAG, -1, 0);
    

    for(int i = 0; i<td->n_files; i++){        
        char file_path[PATH_MAX];
        sprintf(file_path, "%s/%s", path, td->filenames[i]);
        size_t filesize = get_filesize(file_path);
        unsigned char *content = mmap(NULL, sizeof(unsigned char) * filesize, MMAP_PROT, MMAP_FLAG, -1, 0);
        
        int pid = fork();
        printf("%d\n", pid);
        if(pid) continue;                
        //Ejecucion de hijo        
        
        FILE* f = fopen(file_path, "rb");                    
        fread(content, 1, filesize, f);
        td->content[i] = content;
        td->file_sizes[i] = filesize;
        fclose(f);        
        exit(0);
    }    
    
    for(int i = 0; i<td->n_files; i++){
        wait(NULL);
    }    
    return td;
}






#endif
