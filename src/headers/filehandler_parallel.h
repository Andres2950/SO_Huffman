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

int MEM_USE_FACTOR = 2;

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
    td->content = mmap(NULL, sizeof(unsigned char*) * td->n_files, MMAP_PROT, MMAP_FLAG, -1, 0);
    td->file_sizes = mmap(NULL, sizeof(size_t) * td->n_files, MMAP_PROT, MMAP_FLAG, -1, 0);
    

    for(int i = 0; i<td->n_files; i++){        
        char file_path[PATH_MAX];
        sprintf(file_path, "%s/%s", path, td->filenames[i]);
        size_t filesize = get_filesize(file_path);
        unsigned char *content = mmap(NULL, sizeof(unsigned char) * filesize, MMAP_PROT, MMAP_FLAG, -1, 0);
        
        int pid = fork();        
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

int targetdir_compress_parallel(TargetDir* td, char **dict){
    char cero = 0;
    char uno = 1;

    td->b_content = mmap(NULL, sizeof(char*) * td->n_files, MMAP_PROT, MMAP_FLAG, -1, 0);
    td->b_content_sizes = mmap(NULL, sizeof(size_t) * td->n_files, MMAP_PROT, MMAP_FLAG, -1, 0);    

    //No sabemos el tamaño final de b_content y no podemos estar seguros que será menor que el
    //tamaño del archivo sin comprimir.
    //El modo paralelo lanza un error si hay overflow.
    for(int i=0; i < td->n_files; i++){           
        td->b_content[i] = mmap(NULL, td->file_sizes[i]*MEM_USE_FACTOR, MMAP_PROT, MMAP_FLAG, -1, 0); 
    }

    // por cada archivo
    for(int i=0; i < td->n_files; i++){                           
        int pid = fork();
        if(pid) continue;
        // Ejecucion de hijo

        char byte = 0;
        size_t byte_counter=0;
        int bit_counter =0;
        size_t total_bit_size = 0;
        char bit;
        
        // por cada caracter
        for(size_t j=0; j<td->file_sizes[i]; j++){
            int caracter = td->content[i][j];
            //por cada bit de codigo
            for(size_t k=0; k<strlen(dict[caracter]); k++){
                char bitchar = dict[caracter][k];
                //Insertar el bit
                if(bitchar=='0'){   
                    bit = cero;            
                }else if(bitchar=='1'){
                    bit = uno;
                }else{
                    return -1; //str invalido
                }
                byte = byte << 1;
                byte = byte | bit;
                bit_counter++;

                //Escribe el byte si ya esta lleno
                if(bit_counter==8){
                    td->b_content[i][byte_counter] = byte;
                    bit_counter=0;
                    byte_counter++;
                }              
                total_bit_size++;  
                //overflow control
                if(byte_counter == td->file_sizes[i]*MEM_USE_FACTOR -2){
                    exit(-1);
                }
            }   
            //Escribe el byte si es el ultimo
            if(j==td->file_sizes[i]-1 && bit_counter!=0){
                //Rellenar con ceros
                while(bit_counter!=8){
                    byte = byte << 1;
                    bit_counter++;
                }
                td->b_content[i][byte_counter] = byte;
                byte_counter++;
            }
        }
                
        td->b_content_sizes[i] = total_bit_size;    
        exit(0);        
    }

    int r = 0;
    for(int i = 0; i<td->n_files; i++){
        int status;
        wait(&status);
        if(status){
            r=-1;
        }
    }   
    
    return r;
}





#endif
