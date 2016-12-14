#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mergeArray(void *output, const void *a, size_t sizeOfA, const void *b, size_t sizeOfB, size_t sizeOfType){
    memcpy((char *)output, a, sizeOfA * sizeOfType);
    memcpy(((char **)output)[sizeOfA * sizeOfType], b, sizeOfB * sizeOfType);

    /*
    char *oa = (char *)a,
         *ob = (char *)b;

    sizeOfA = (sizeOfA - 1) * sizeOfType;
    sizeOfB = (sizeOfB - 1) * sizeOfType;

    while(sizeOfA != -1 && sizeOfB != -1){
        
        if(sizeOfA >= 0){
            ((char*)output)[i++] = oa[sizeOfA--];
        }

        if(sizeOfB >= 0){
            ((char*)output)[i++] = ob[sizeOfB--];
        }
    }
    
    return i;*/
}

int main(){
    int i;
    int a[5] = {1,2,3,4,5};
    int b[5] = {1,2,3,4,5};
    int c[20];

    mergeArray(&c,&a,5,&b,5,sizeof(int));

    for (i = 0; i < 10; i++){
        printf("%d\n", c[i]);   
    }
    return 0;
}
