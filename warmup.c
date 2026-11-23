// test script for computing page num and offset with bit operations

#include <stdio.h>
#include <stdlib.h>

#define ARR_SIZE 7

int main(){
    int arr[] = {1, 256, 32768, 32769, 128, 65534, 33153}; 

    for (int i = 0; i < ARR_SIZE; i++){
        arr[i] = arr[i] & 0xFFFF;   //masked to 16 bit digit

        int pageNum = arr[i] >> 8;  //leftmost 8 binary digits
        int offset = arr[i] & 0xFF; //rightmost 8 binary digits

        printf("Page number: %d\n", pageNum);
        printf("Offset: %d\n\n", offset);
    }
    return 0;
}