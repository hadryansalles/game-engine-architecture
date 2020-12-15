#include <cstdint>
#include <fstream>
#include <iostream>
#include <float.h>
#include <bitset>

using U16 = std::uint16_t;
using U32 = std::uint32_t;
using U64 = std::uint64_t;
using F32 = float;
using F64 = double;

U16 SwapU16(U16 v) {
	return ((v & 0x00FF) << 8) 
		|  ((v & 0xFF00) >> 8);
}

U32 SwapU32(U32 v) {
	return ((v & 0x000000FF) << 24)
		 | ((v & 0x0000FF00) << 8)
		 | ((v & 0x00FF0000) >> 8)
		 | ((v & 0xFF000000) >> 24);
}

U64 SwapU64(U64 v) {
	return ((v & 0x00000000000000FF) << 56)
		|  ((v & 0x000000000000FF00) << 40)
		|  ((v & 0x0000000000FF0000) << 24)
		|  ((v & 0x00000000FF000000) << 8)
		|  ((v & 0x000000FF00000000) >> 8)
		|  ((v & 0x0000FF0000000000) >> 24)
		|  ((v & 0x00FF000000000000) >> 40)
		|  ((v & 0xFF00000000000000) >> 56);
}

union U32F32 {
	F32 asF32;
	U32 asU32;
};

union U64F64 {
	F64 asF64;
	U64 asU64;
};

F32 SwapF32(F32 v) {
	U32F32 u;
	u.asF32 = v;
	u.asU32 = SwapU32(u.asU32);
	return u.asF32;
}

F64 SwapF64(F64 v) {
	U64F64 u;
	u.asF64 = v;
	u.asU64 = SwapU64(u.asU64);
	return u.asF64;
}

int main() {

	U16 outU16 = 1000;
	U32 outU32 = 1000;
	U64 outU64 = 1000;
	F32 outF32 = 1000;
	F64 outF64 = 1000;
	
	outU16 = SwapU16(outU16);
	outU32 = SwapU32(outU32);
	outU64 = SwapU64(outU64);
	outF32 = SwapF32(outF32);
	outF64 = SwapF64(outF64);

	std::ofstream fileOut("out.bin", std::ios::out | std::ios::binary);
	fileOut.write((char*)&outU16, sizeof(U16));
	fileOut.write((char*)&outU32, sizeof(U32));
	fileOut.write((char*)&outU64, sizeof(U64));
	fileOut.write((char*)&outF32, sizeof(F32));
	fileOut.write((char*)&outF64, sizeof(F64));
	fileOut.close();

	U16 inU16;
	U32 inU32;
	U64 inU64;
	F32 inF32;
	F64 inF64;

	std::ifstream fileIn("out.bin", std::ios::in | std::ios::binary);
	fileIn.read((char*)&inU16, sizeof(U16));
	fileIn.read((char*)&inU32, sizeof(U32));
	fileIn.read((char*)&inU64, sizeof(U64));
	fileIn.read((char*)&inF32, sizeof(F32));
	fileIn.read((char*)&inF64, sizeof(F64));
	fileIn.close();

	inU16 = SwapU16(inU16);
	inU32 = SwapU32(inU32);
	inU64 = SwapU64(inU64);
	inF32 = SwapF32(inF32);
	inF64 = SwapF64(inF64);

	std::cout << inU16 << " | " << inU32 << " | " << inU64 << " | " << inF32 << " | " << inF64 << "\n";

	return 0;
}