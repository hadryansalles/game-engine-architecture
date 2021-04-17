#include <stdio.h>
#include <thread>
#include <chrono>
#include <cstring>

#include "SyncFileIO.h"
#include "AsyncFileIO.h"
#include "CrossReferences.h"

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

void crossRefereceTest() {
	printf("======================================================\n");
	printf("Cross Reference Test.\n");

    constexpr int n = 100;
    char outStr[n];
    char inStr[n];

    int ptrOffset = (int)((char*)&(((ObjectB*)0)->ptr) - (char*)(ObjectB*)0);
    int offsetA = 0;
    int offsetB = 0;
    int offsetBptr = 0;

    std::ofstream ofs("cross.txt", std::ios_base::out | std::ios_base::binary);
    if (ofs) {
        printf("======================================================\n");
        printf("Writing objects.\n");

        // setup objects
        ObjectA* outA = new ObjectA;
        outA->data = 2;
        ObjectB* outB = new ObjectB;
        outB->data = 1;
        outB->data2 = 1023;
        outB->ptr = outA;

        // serialize each object storing their offsets
        int offset = 0;
        std::memcpy(outStr+offset, (char*)outA, sizeof(ObjectA));
        offsetA = offset;
        offset += sizeof(ObjectA);

        std::memcpy(outStr+offset, (char*)outB, sizeof(ObjectB));
        offsetB = offset;
        // store each pointer offset
        offsetBptr = offset + ptrOffset;
        offset += sizeof(ObjectB);

        // replace all ptrs in the file image with object offsets
        std::memset(outStr+offsetBptr, offsetA, sizeof(ObjectA*));

        // write to file
        ofs.write(outStr, n);
        ofs.close();

        delete outA;
        delete outB;
    }

    std::ifstream ifs("cross.txt", std::ios_base::in | std::ios_base::binary);
    if (ifs) {
        printf("======================================================\n");
        printf("Reading objects.\n");

        // read file
        ifs.read(inStr, n);
        ifs.close();

        // read each object
        ObjectA* a = (ObjectA*)(inStr + offsetA);
        ::new(a) ObjectA;
        std::cout << "Read object A at " << a << " with data: " << a->data << "\n";

        ObjectB* b = (ObjectB*)(inStr + offsetB);
        ::new(b) ObjectB;
        // replace pointer with the object at the given offset from the base file image address
        b->ptr = (ObjectA*)(inStr + (int)b->ptr);
        std::cout << "Read object B with data: " << b->data << ", data2: " << b->data2 << " and ptr: " << b->ptr << '\n';
        std::cout << "Read B->ptr with data: " << b->ptr->data << '\n';
        
        a->~ObjectA();
        b->~ObjectB();
    }
}

int main() {
    crossRefereceTest();
    syncTest();
    asyncTest();
    return 0;
}