#include <stdio.h>
#include <thread>
#include <chrono>

#include "SyncFileIO.h"
#include "AsyncFileIO.h"

void syncTest() {
	printf("======================================================\n");
	printf("Synchronous File Read");

    U8 testBuffer[512];
    size_t bytesRead = 0;

    if (syncReadFile("test.txt", testBuffer, sizeof(testBuffer), bytesRead)) {
        printf("success: read %u bytes\n", bytesRead);
        for (int i = 0; i < bytesRead; i++) {
            printf("%c", testBuffer[i]);
        }
        printf("\n");
    }
}

AsyncFileHandle g_fileHandle("test.txt");

void _asyncReadTest() {
}

void asyncTest() {
	printf("======================================================\n");
	printf("Asynchronous File Open\n");
    
    asyncOpen(&g_fileHandle, []() {
        printf("======================================================\n");
        printf("Asynchronous File Read\n");
        
        U8 testBuffer[512];

        // This function requests an I/O read, then returns immediately (non-blocking).
        AsyncRequestHandle hReadRequest = asyncReadFile(g_fileHandle, testBuffer, sizeof(testBuffer), nullptr);

        // Now do some limited amount of work...
        for (int i = 0; i < 10; i++) {
            printf("zzz...\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // We can't do anything further until we have that data, so wait for it here.
        asyncWait(hReadRequest);

        if (asyncWasSuccessful(hReadRequest)) {
            // The data is now presented in testBuffer and can be used.
            // Query for the number of bytes actually read.
            size_t bytes = asyncGetBytesReadOrWritten(hReadRequest);
            char msg[256];
            snprintf(msg, sizeof(msg), "async success, read %u bytes\n", bytes);
            printf("%s", msg);  
            for (int i = 0; i < bytes; i++) {
                printf("%c", testBuffer[i]);
            }
            printf("\n");
        }
    });
}

int main() {
    syncTest();
    asyncTest();
    return 0;
}