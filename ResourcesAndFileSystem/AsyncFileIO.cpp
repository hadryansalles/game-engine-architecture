#include "AsyncFileIO.h"

#include <fstream>

namespace {

bool _asyncOpen(AsyncFileHandle* handle, std::function<void()> callback) { 
    handle->file = fopen(handle->path, "rb");

    if (handle->file) {
        if (callback) {
            callback();
        }
        return true;
    }

    return false;
}

bool _asyncReadFile(const AsyncFileHandle& handle, void* buffer, size_t bufferSize, size_t* bytesReadOrWritten, std::function<void()> callback) {
    if (handle.file) {
        size_t rBytesRead = fread(buffer, 1, bufferSize, handle.file);
       
        int err = ferror(handle.file);
        
        fclose(handle.file);

        if (0 == err) {
            (*bytesReadOrWritten) = rBytesRead;
            if (callback) {
                callback();
            };
            return true;
        }

        (*bytesReadOrWritten) = 0;
        return false;
    }
}

}

AsyncRequestHandle asyncOpen(AsyncFileHandle* handle, std::function<void()> callback) {
    AsyncRequestHandle request;
    request.future = std::async(std::launch::async, _asyncOpen, handle, callback);
    return request;
}

AsyncRequestHandle asyncReadFile(const AsyncFileHandle& handle, void* buffer, size_t bufferSize, std::function<void()> callback) {
    AsyncRequestHandle request;
    request.future = std::async(std::launch::async, _asyncReadFile, handle, buffer, bufferSize, request.bytesReadOrWritten, callback);
    return request;
}

