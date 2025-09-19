#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <sys/stat.h>
#include <dirent.h>
#include <linux/limits.h>


#ifndef FILEHANDLER_C

// Struct with the path of a directory, a list of the
// files inside it and their contents
typedef struct{
    char* path;
    char** filenames;
    int n_files;
    wchar_t** content;    
} TargetDir;


int isdir(const char* path){
    struct stat stat_info;
    stat(path, &stat_info);
    return !S_ISDIR(stat_info.st_mode);    
}

int isfile(const char* path){
    struct stat stat_info;
    stat(path, &stat_info);
    return S_ISREG(stat_info.st_mode);    
}


// Lee la información de los archivos de texto en una carpeta
TargetDir* readTargetDir(char* path){    
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
            //TODO: checkear que el archivo sea texto. HAY QUE HACER ESTO        
            td->n_files++;
            td->filenames = realloc(td->filenames, sizeof(char*) * td->n_files); //memory leak            
            td->filenames[td->n_files-1] = malloc(sizeof(char)*(PATH_MAX+1));
            strcpy(td->filenames[td->n_files-1], file->d_name);
        }
        
        file = readdir(d);
    }
    
    closedir(d);
    //Leer el contenido de los archivos
    td->content = malloc(sizeof(wchar_t*)*td->n_files);
    for(int i = 0; i<td->n_files; i++){
        char file_path[PATH_MAX];
        sprintf(file_path, "%s/%s", path, td->filenames[i]);
        FILE* f = fopen(file_path, "r");
        fseek(f, 0, SEEK_END);
        long filesize = ftell(f);
        fseek(f, 0, SEEK_SET);
        char* chontent = malloc(sizeof(char) * (filesize+1));
        fread(chontent, filesize, 1,  f);
        // Hay que pasar todo explicitamente ha wchar_t para que huffman no llore
        int dsize =mbstowcs(NULL,chontent,0)+1;
        wchar_t* content = malloc(sizeof(wchar_t) * dsize);        
        mbstowcs(content, chontent, dsize);        
        
        td->content[i] = content;
        fclose(f);
    }    

    
    return td;
}

// Escribe en un archivo el codigo binario que contiene la str.
// str solo puede contener '0' o '1'.
int write_binary_to_file(FILE* f, const char* str){
    char byte = 0;
    int byte_counter=0;
    char bit;
    char cero = 0;
    char uno = 1;
    for(int i=0; i<strlen(str); i++){            
        //Insertar el bit
        if(str[i]=='0'){   
            bit = cero;            
        }else if(str[i]=='1'){
            bit = uno;
        }else{
            return -1; //str invalido
        }
        byte = byte << 1;
        byte = byte | bit;
        byte_counter++;

        //Escribe el byte si ya esta lleno
        if(byte_counter==8){
            fputc(byte, f);
            byte_counter=0;
        }
        //Escribe el byte si el ultimo
        if(i==strlen(str)-1 && byte_counter!=0){
            //Rellenar con ceros
            while(byte_counter!=8){
                byte = byte << 1;
                byte_counter++;
            }
            fputc(byte, f);
        }
   }   
   return 0;
}


// Escribe el archivo comprimido
// Formato: arbol \n filename \t bytes del contenido \t contenido \n
int huffman_write_file(const char *dst, TargetDir* td, char **dict, Node* tree) {
    FILE *file = fopen(dst, "wb"); //wb escribe en binario (linux y sistemas posix ignoran el b)
    if (file == NULL) return -1;


    //Escribir arbol; no se como ._.XD

    fputc('\n', file);
    
    // Escribir archivos

    for(int i=0; i<td->n_files; i++){
        fprintf(file, "%s\t", td->filenames[i]);

        // int filesize = wcslen(td->content[i]);
        // fprintf(file, "%d\t", filesize);        

        // char* binary = huffman_translate(td->content[i], filesize, dict); //prueba con todo el texto        
                
        // write_binary_to_file(file, binary);        
        // fprintf(file,"\n");
    }

    fclose(file);

    return 0;
}


#endif
#endif
