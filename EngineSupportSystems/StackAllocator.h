#pragma once 

#include <cstdint>

using U32 = std::int32_t;

class StackAllocator {
public:
    // Represents the current top of the stack.
    // The stack can only roll back to a marker, 
    // not arbitrary locations within the stack.
    typedef U32 Marker;

    // Constructs a stack allocator with the given size
    explicit StackAllocator(U32 stackSizeBytes) {
        mStack = new unsigned char[stackSizeBytes];
        mTotalSize = stackSizeBytes;
        mTopPointer = stackSizeBytes;
    }

    // Allocates a new block of the given size from the stack top
    void* alloc(U32 sizeBytes) {
        if (mTopPointer > sizeBytes) {
            mTopPointer -= sizeBytes;
            return (void*)(mStack + mTopPointer);
        }
        return NULL;
    }

    // Returns a marker to the current stack top
    Marker getMarker() {
        return mTopPointer;
    }

    // Rolls the stack back to previous marker
    void freeToMarker(Marker marker) {
        mTopPointer = marker;
    }

    // Clears the entire stack 
    void clear() {
        mTopPointer = mTotalSize;
    }

private:
    unsigned char* mStack;
    U32 mTotalSize;
    U32 mTopPointer;
};

class DoubleEndedStackAllocator {
public:
    // Represents the current top/bot of the stack.
    // The stack can only roll back to a marker, 
    // not arbitrary locations within the stack.
    typedef U32 Marker;

    // Constructs a stack allocator with the given size
    explicit DoubleEndedStackAllocator(U32 stackSizeBytes) {
        mStack = new unsigned char[stackSizeBytes];
        mTotalSize = stackSizeBytes;
        mTopPointer = stackSizeBytes;
        mBotPointer = 0;
    }

    // Allocates a new block of the given size from the stack top
    void* allocTop(U32 sizeBytes) {
        if (mTopPointer - mBotPointer > sizeBytes) {
            mTopPointer -= sizeBytes;
            return (void*)(mStack + mTopPointer);
        }
        return NULL;
    }

    // Allocates a new block of the given size from the stack top
    void* allocBot(U32 sizeBytes) {
        if (mTopPointer - mBotPointer > sizeBytes) {
            U32 tmpPointer = mBotPointer;
            mBotPointer += sizeBytes;
            return (void*)(mStack + tmpPointer);
        }
        return NULL;
    }

    // Returns a marker to the current stack top
    Marker getTopMarker() {
        return mTopPointer;
    }

    // Returns a marker to the current stack top
    Marker getBotMarker() {
        return mBotPointer;
    }
    
    // Rolls the stack back to previous marker
    void freeTopToMarker(Marker marker) {
        mTopPointer = marker;
    }

    // Rolls the stack back to previous marker
    void freeBotToMarker(Marker marker) {
        mBotPointer = marker;
    }
    
    // Clears the entire stack 
    void clearBot() {
        mBotPointer = 0;
    }

    // Clears the entire stack 
    void clearTop() {
        mTopPointer = mTotalSize;
    }

private:
    unsigned char* mStack;
    U32 mTopPointer;
    U32 mBotPointer;
    U32 mTotalSize;
};
