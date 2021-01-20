#pragma once
    
#include<cstdint>

#include <boost/crc.hpp>
#include <cstring>


#define SID(str) SID_T(str, strlen(str))
#define SID_T(str, size) boost::augmented_crc<32, 0x8005>(str, size)

typedef std::uint32_t StringId;

inline StringId operator ""_sid(const char* str, size_t size) {
    return SID_T(str, size);
}

extern StringId internString(const char* str);
