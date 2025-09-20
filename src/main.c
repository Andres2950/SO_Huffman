#include <stdio.h>
#include <stdlib.h>
#include "headers/huffman.h"
#include "headers/filehandler.h"
#include <locale.h>

int *ft;

void hola_mundo(char *c){
  printf("Hola Mundo!\n");
}

int comprimir(char* src, char* dst){
    // Leer src todos los archivos en src (nombre y contenido)
    TargetDir* td = read_targetdir(src);  
    //Hacer Huffman con el contenido de todos lo archivos    
    int text_size = 1;
    for(int i = 0; i<td->n_files; i++){
        text_size += wcslen(td->content[i]);
    }
    
    wchar_t* text = calloc(text_size, sizeof(wchar_t));
    for(int i = 0; i<td->n_files; i++){
        wcscat(text, td->content[i]);
    }    
    text[text_size-1] = L'0';    

    //Huffman    
    int* ft = new_frequency_table();
    frequency_table_add_text(ft, text);            
    LinkedList list;
    init_linked_list(&list);
    linked_list_insert_bulk(&list, ft);        

    Node *huffman_tree = create_huffman_binary_tree(&list);
    char **dictionary = huffman_create_dictionary(huffman_tree);    
    
    // Comprimir archivos
    targetdir_compress(td, dictionary);

    //TODO: escribir el archivo comprimido  
    return 0;  
}


int main(int argc, char* argv[]){
    setlocale(LC_ALL, "es_ES.UTF-8");
    // hola_mundo("printf");

    // LinkedList list;
    // ft = new_frequency_table();
    // wchar_t *texto_perron = L"COMO COME COCORITO COME COMO COSMONAUTA áéíóúñ";
    // printf("---------------------- cadena -----------------\n");
    // for (int i = 0; texto_perron[i] != L'\0'; i++){
    //     printf("char: '%lc', unicode: %u\n", texto_perron[i],texto_perron[i]);
    // }

    // frequency_table_add_text(ft, texto_perron);
    // printf("---------------------- FRACUENCY TABLE -----------------\n");
    // frequency_table_print(ft);
    // init_linked_list(&list);
    // linked_list_insert_bulk(&list, ft);
    // printf("-------------------------- LINKED LIST --------------------\n");
    // linked_list_print(&list);

    // Node *huffman_tree = create_huffman_binary_tree(&list);
    // char **dictionary = huffman_create_dictionary(huffman_tree);

    // printf("--------------------------- TREE ---------------------------\n");
    // huffman_binary_tree_print(huffman_tree, 0);
    // printf("---------------------------------- DICCIONARIO --------------------------\n");
    // huffman_dictionary_print(dictionary);

    //Ejemplo escribir binario
    // FILE* f = fopen("binario.b", "w");
    // write_binary_to_file(f, "0100011101001001010101000010000001000111010101010100010");
    // fclose(f);

    //ejemplo leer directorio
    //TargetDir* td = readTargetDir("./ej");
    // for(int i = 0; i<td->n_files; i++){
    //     printf("%s\n%ls\n\n", td->filenames[i], td->content[i]);
    // }
    
    comprimir("./ej", "");

    return 0;
}
