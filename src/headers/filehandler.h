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

    //Obtener archivos de directorio
    DIR* d = opendir(path);
    struct dirent* file;
    file = readdir(d);
    while(file!=NULL){        
        char file_path[PATH_MAX];
        sprintf(file_path, "%s/%s", path, file->d_name);
        if(isfile(file_path)){
            // Agregar archivo a TargetDir
            //TODO: checkear que el archivo sea texto
            td->n_files++;
            td->filenames = realloc(td->filenames, sizeof(char*) * td->n_files); 
            td->filenames[td->n_files-1] = file->d_name;
        }
        file = readdir(d);
    }
    
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
        wchar_t* content = malloc(sizeof(wchar_t) * (filesize+1));
        mbstowcs(content, chontent, filesize);
        printf("%ls\n", content);
        
        td->content[i] = content;
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

// La vara pero manejando el dict

int huffman_write_file(const wchar_t *str, char **dict, const char *filename) {
    FILE *file = fopen(filename, "wb"); //wb escribe en binario (linux y sistemas posix ignoran el b)
    if (file == NULL) return -1;

    //Contamos la cantidad de caracteres y la escribimos 
    int unique_char = 0;
    for (int i = 0; i < MAX_UNICODE; i++) {
        if (dict[i] && dict[i][0] != '\0') unique_char++;
    }
    //fwrite escribe en binario
    fwrite(&unique_char, sizeof(int), 1, file);

    //escribir el diccionario
    for (int i = 0; i < MAX_UNICODE; i++) {
        if (dict[i] && dict[i][0] == '\0') continue;

        wchar_t c = i; 
        int code_len = strlen(dict[i]);
        int code_bits = 0;
        //Convertir el codigo a bits de alguna forma no se 
        for (int j = 0; j < code_len; j++){
            //Code bits deberia ser un entero con el codigo de la letra 
            //No se como hacer esto, poner los 1 y 0 en code_bits de alguna forma 
        }

        // escribir el caracter del diccionario, y el codigo 
        fwrite(&c, sizeof(wchar_t), 1, file);
        fwrite(&code_bits, sizeof(int), 1, file);
    }

    // AQUI PONER ALGUN TIPO DE CARACTER ESPECIAL PARA SEPARAR EL CONTENIDO

    //  iterar por la string revisando el codigo del simbolo
    //  Escribir el codigo del simbolo en binario en el archivo y hacer eso para cada caracter en la string 

}


#endif
#endif
