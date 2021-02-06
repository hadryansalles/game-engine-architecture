#include "SyncFileIO.h"
#include <stdio.h>

int main() {

    U8 testBuffer[512];
    size_t bytesRead = 0;

    if (syncReadFile("test.txt", testBuffer, sizeof(testBuffer), bytesRead)) {
        printf("success: read %u bytes\n", bytesRead);
        for (int i = 0; i < bytesRead; i++) {
            printf("%c", testBuffer[i]);
        }
        printf("\n");
    }

    return 0;
}