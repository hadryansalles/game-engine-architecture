#define _CRT_SECURE_NO_WARNINGS 1

#include <fstream>
#include "../AlignmentAndPacking/SizedTypes.h"


bool syncReadFile(const char* filePath, U8* buffer, size_t bufferSize, size_t& rBytesRead) {

    FILE* handle = fopen(filePath, "rb");

    if (handle) {
        // Block here until all data has been read
        size_t bytesRead = fread(buffer, 1, bufferSize, handle);

        // Get error if any
        int err = ferror(handle);

        fclose(handle);

        if (0 == err) {
            rBytesRead = bytesRead;
            return true;
        }
            
        rBytesRead = 0;
        return false;
    }
}