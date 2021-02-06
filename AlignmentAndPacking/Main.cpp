// static assertion from chapter "Handling Errors"
#define _ASSERT_GLUE(a, b) a ## b
#define ASSERT_GLUE(a, b) _ASSERT_GLUE(a, b)
#define STATIC_ASSERT(expr) enum { ASSERT_GLUE(g_assert_fail_, __LINE__) = 1 / (int) (!!(expr)) }

#include "SizedTypes.h"

// data's size is usually the sum of each member's width
struct Foo {
	U32 uv; // all members are perfect aligned, 
	F32 fv; // so the data's size is just the sum of each member's width.
	I32 sv;
}; 

// but if some member has different width, the data's size will depend on the alignment
struct Bar {
	U32  uv; // the struct is 12 bytes 
	F32  fv; // even with the members suming up to 9 bytes.
	bool bv; // (3 bytes of padding)
};

// small data members interspersed with large data members
// cause waste of space due to padding requirements
struct InefficientPacking {
	U32   u1; // 4 bytes
	F32   f2; // 4 bytes
	U8    u3; // 1 byte and 3 bytes of padding
	I32   i4; // 4 bytes
	bool  b5; // 1 byte and 3 bytes of padding
	char* c6; // 4 bytes on x86
};

// InefficientPacking member's width sum to 18 bytes
// but its memory is aligned with the width of its largest member (4 bytes in this case) 
// therefore its total size is 24 bytes 

// if is a x64 system, the char* is 8 bytes so the alignment of the struct is 8 bytes 
// causing the total size to be 32 bytes
struct InefficientPackingX64 {
	U32 u1; F32 f2; // 8 bytes
	U8  u3; I32 i4; // 5 bytes and 3 bytes of padding
	bool  b5; // 1 byte and 7 bytes of padding
	char* c6; // 8 bytes on x64
}; // sum to 22 bytes but has size of 32 bytes

// only by reordering the members we can save 4 bytes on x86 and 8 bytes on x64

struct MoreEfficientPacking {
	U32 u1; // 4 bytes
	F32 f2; // 4 bytes
	I32 i4; // 4 bytes
	U8 u3; bool b5; // 2 bytes and 2 bytes of padding
	char* c6; // 4 bytes on x86
}; // sum to 18 bytes but has size of 20 bytes due to alignment of 4 bytes

struct MoreEfficientPackingX64 {
	U32 u1; F32 f2; // 8 bytes
	I32 i4; U8 u3; bool b5; // 6 bytes and 2 bytes of padding
	char* c6; // 8 bytes on x64
}; // sum to 22 bytes but has size of 24 bytes due to alignment of 8 bytes

// even better is make te padding visible and explict through a member of the same width
struct BestPacking {
	U32 u1; // 4 bytes
	F32 f2; // 4 bytes
	I32 i4; // 4 bytes
	U8 u3; bool b5; U8 pad[2]; // 4 bytes
	char* c6; // 4 bytes on x86
}; // sum to 20 bytes and has this exact size

struct BestPackingX64 {
	U32 u1; F32 f2; // 8 bytes
	I32 i4; U8 u3; bool b5; U8 pad[2]; // 8 bytes
	char* c6; // 8 bytes on x64
}; // sum to 24 bytes and has this exact size

int main() {
	STATIC_ASSERT(sizeof(Foo) == 12); 
	STATIC_ASSERT(sizeof(Bar) == 12);
#ifdef _WIN64
	STATIC_ASSERT(sizeof(InefficientPackingX64) == 32);
	STATIC_ASSERT(sizeof(MoreEfficientPackingX64) == 24);
	STATIC_ASSERT(sizeof(BestPackingX64) == 24);
#elif _WIN32
	STATIC_ASSERT(sizeof(InefficientPacking) == 24);
	STATIC_ASSERT(sizeof(MoreEfficientPacking) == 20);
	STATIC_ASSERT(sizeof(BestPacking) == 20);
#endif
	return 0;
}

