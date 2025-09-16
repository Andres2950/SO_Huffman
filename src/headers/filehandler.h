#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <stdio.h>
#include <stdlib.h>


#ifndef FILEHANDLER_C


int write_binary_to_file(char *path, char* str){
    FILE* f = fopen(path, "w");    
    if(f==NULL){
        return -1;
    }

    
}


#endif
#endif
