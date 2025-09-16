#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#ifndef FILEHANDLER_C


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
