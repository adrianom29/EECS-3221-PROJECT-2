// test script for computing page num and offset with bit operations

#include <stdio.h>
#include <stdlib.h>

#define ARR_SIZE 7
#define ONES 255

int main(){
    int arr[] = {1, 256, 32768, 32769, 128, 65534, 33153}; 

    for (int i = 0; i < ARR_SIZE; i++){
        int pageNum = arr[i] >> 8;
        int offset = arr[i] & 255;
        printf("Page number: %d\n", pageNum);
        printf("Offset: %d\n\n", offset);
    }
    return 0;
}