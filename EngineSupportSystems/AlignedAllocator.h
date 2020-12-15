#pragma once

#include <cstdint>
#include "../HandlingErrors/Assert.h"

// Shift the given address upwards if/as necessary to ensure it's aligned to the given number of bytes
inline std::uintptr_t AlignAddress(std::uintptr_t addr, size_t align) {
    const size_t mask = align - 1;
    ASSERT((align & mask) == 0); // power of 2
    return (addr + mask) & ~mask;
}

// Shift the given pointer upwards if/as necessary to ensure it's aligned to the given number of bytes
template<typename T>
inline T* AlignPointer(T* ptr, size_t align) {
    std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(ptr);
    std::uintptr_t addrAligned = AlignAddress(addr, align);
    return reinterpret_cast<T*>(addrAligned);
}

// Aligned allocation function. align must be a power of 2
void* AllocAligned(size_t bytes, size_t align) {

    // Allocate 'align' more bytes than we need
    size_t actualBytes = bytes + align;

    // Allocate unaligned block
    std::uint8_t* pRawMemory = new std::uint8_t[actualBytes];

    // Align the block
    // If no alignment occurred, shift it up the full 'align' bytes
    // so we always have room to store the shift
    std::uint8_t* pAlignedMem = AlignPointer(pRawMemory, align);
    if (pAlignedMem == pRawMemory) {
        pAlignedMem += align;
    }
    
    // Determine the shift, and store it.
    // (This works for up to 256-byte alignment)
    ptrdiff_t shift = pAlignedMem - pRawMemory;
    ASSERT(shift > 0 && shift <= 256);

    pAlignedMem[-1] = static_cast<std::uint8_t>(shift & 0xFF);

    return pAlignedMem;
}


void FreeAligned(void* pMem) {
    if (pMem) {

        // Convert to U8 pointer
        std::uint8_t* pAlignedMem = reinterpret_cast<std::uint8_t*>(pMem);

        // Extract the shift
        ptrdiff_t shift = pAlignedMem[-1];
        if (shift == 0) {
            shift = 256;
        }

        // Back up to the actual allocated address add delete it
        std::uint8_t* pRawMem = pAlignedMem - shift;
        delete[] pRawMem;
    }
}
