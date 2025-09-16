#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifndef FILEHANDLER_C


//int write_binary_to_file(char *path, char* str){
//    FILE* f = fopen(path, "w");    
//    if(f==NULL){
//        return -1;
//    }
//}

// La vara pero manejando el dict

int huffman_write_file(const wchar_t *str, char **dict, const char *filename) {
    FILE *file = fopen(filename, "wb"); //wb escribe en binario 
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
