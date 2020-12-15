#pragma once

#include <iostream>
#include <Windows.h>

// instruction to reduce power consumption in busy-wait loops
#define PAUSE() _mm_pause() 

#define _ASSERT_GLUE(a, b) a ## b
#define ASSERT_GLUE(a, b) _ASSERT_GLUE(a, b)
#define STATIC_ASSERT(expr) enum { ASSERT_GLUE(g_assert_fail_, __LINE__) = 1 / (int) (!!(expr)) }

template<bool> class T_STATIC_ASSERT;
template<> class T_STATIC_ASSERT<true> {};
#define T_STATIC_ASSERT(expr) enum { ASSERT_GLUE(g_assert_fail_, __LINE__) = sizeof(T_STATIC_ASSERT<!!(expr)>) }

#ifdef _WIN64
	#define DEBUG_BREAK() __debugbreak()
#elif _WIN32
	// assembly instruction to break
	#define DEBUG_BREAK() __asm { int 3 }
#else 
	// not implemented
	#define DEBUG_BREAK() T_STATIC_ASSERT(false)
#endif

#define DEBUG_REPORT(expr, file, line) std::cout << expr << " ON FILE: " << file << " LINE: " << line << '\n'
#define ASSERT(expr) if(expr) { } else { DEBUG_REPORT(#expr, __FILE__, __LINE__); DEBUG_BREAK(); }	

#ifdef _DEBUG
	#define SLOW_ASSERT(expr) ASSERT(expr)
#else
	#define SLOW_ASSERT(expr)
#endif
