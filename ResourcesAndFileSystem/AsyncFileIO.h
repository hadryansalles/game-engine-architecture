#pragma once

#include <future>

struct AsyncRequestHandle {
    std::future<bool> future;
    size_t* bytesReadOrWritten;

    AsyncRequestHandle() {
        bytesReadOrWritten = new size_t;
    }

    ~AsyncRequestHandle() {
        delete bytesReadOrWritten;
    }

    AsyncRequestHandle(AsyncRequestHandle&& rhs) {
        future = std::move(rhs.future);
        bytesReadOrWritten = rhs.bytesReadOrWritten;
        rhs.bytesReadOrWritten = nullptr;
    }
};

struct AsyncFileHandle {

    AsyncFileHandle(const char* path) 
        : path(path)
    {}

    const char* path;
    FILE* file;
};

inline bool asyncWasSuccessful(AsyncRequestHandle& request) {
    if (request.future.valid()) {
        return request.future.get();
    }
}

inline void asyncWait(const AsyncRequestHandle& request) {
    if (request.future.valid()) {
        request.future.wait();
    }
}

inline size_t asyncGetBytesReadOrWritten(const AsyncRequestHandle& request) {
    return (*request.bytesReadOrWritten);
}

AsyncRequestHandle asyncOpen(AsyncFileHandle* handle, std::function<void()> callback);

AsyncRequestHandle asyncReadFile(const AsyncFileHandle& handle, void* buffer, size_t bufferSize, std::function<void()> callback);

