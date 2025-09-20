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
#include <math.h>

#ifndef FILEHANDLER_C

// Struct with the path of a directory, a list of the
// files inside it and their contents
typedef struct{
    char* path;
    char** filenames;
    int n_files;
    unsigned char** content;
    size_t* file_sizes;
    char** b_content;
    size_t* b_content_sizes;
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
TargetDir* read_targetdir(char* path){    
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

    for(int i = 0; i<td->n_files; i++){
        char file_path[PATH_MAX];
        sprintf(file_path, "%s/%s", path, td->filenames[i]);
        FILE* f = fopen(file_path, "rb");
        fseek(f, 0, SEEK_END);
        size_t filesize = ftell(f);
        fseek(f, 0, SEEK_SET);

        unsigned char *content = malloc(sizeof(unsigned char) * filesize);
        fread(content, 1, filesize, f);
        td->content[i] = content;
        td->file_sizes[i] = filesize;
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

// Traduce el contenido de cada archivo de TargetDir usando el diccionario
// La representacion codificada se guarda como un string en b_content
int targetdir_compress(TargetDir* td, char **dict){
    td->b_content = malloc(sizeof(char*) * td->n_files);
    td->b_content_sizes = malloc(sizeof(size_t) * td->n_files);
    for(int i=0; i < td->n_files; i++){
        //int content_s = wcslen(td->content[i]);
        char* binary = huffman_translate(td->content[i], td->file_sizes[i], dict);
        td->b_content[i] = binary;
        td->b_content_sizes[i] = strlen(binary);
    }
    return 0;
}

// Escribe un archivo que contiene la version comprimida del contenido de TargetDir.
// El argumento td debe haber pasado por targetdir_compress.
// TODO: robustez, retornar -1 si no se ha llamado a targetdir_compress.
// 
// Formato del archivo final: diccionario \n archivo comprimido \n [archivo comprimido \n ...]
//
// Formato del diccionario: cantidad de entradas(int 4b) | [entrada ...]
// Formato de entrada: caracter(uchar 1b) | tamaño de codigo(int 4b) | codigo(char tamaño*bytes)
//
// Formato del archivo comprimido: filename(string) \t contentsize(size_t) \t codigo (contensize*bytes)
//       
int targetdir_write(const char *dst, TargetDir* td, char **dict) {
    FILE *file = fopen(dst, "wb"); //wb escribe en binario (linux y sistemas posix ignoran el b)
    if (file == NULL) return -1;


    // Escribir cantidad de entradas del diccionario    
    int size_dict = 0;
    for (int i = 0; i < MAX_BYTE; i++){
      if(dict[i] && dict[i][0] != '\0'){
        size_dict++;
      }
    }
    fwrite(&size_dict, sizeof(int), 1, file);

    // Escribir entradas del diccionario
    int n_cod;
    for (int i = 0; i < MAX_BYTE; i++){
        if(dict[i] && dict[i][0] != '\0'){                        
            unsigned char c = i;
            fwrite(&c, sizeof(unsigned char), 1, file);            
            int code_len = strlen(dict[i]);      // longitud del código en bits
            fwrite(&code_len, sizeof(int), 1, file);
            fwrite(dict[i], sizeof(char), code_len, file);
        }
    }
    fputc('\n', file);
    
    // Escribir archivos
    for (int i = 0; i < td->n_files; i++){
        // filename
        fprintf(file, "%s\t", td->filenames[i]);
        // content_size
        fprintf(file, "%zu\t", td->b_content_sizes[i]);
        // Codigo en binario
        write_binary_to_file(file, td->b_content[i]); 

        fputc('\n', file);
    }        

    fclose(file);
    return 0;
}

#endif
#endif
