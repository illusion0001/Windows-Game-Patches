#pragma once

// Generated Wed, 12 Jun 2024 02:11:46 GMT
// https://alloc8or.re/rdr3/nativedb/

#ifndef NATIVE_DECL
#if defined(_MSC_VER)
	#define NATIVE_DECL __forceinline
#elif defined(__clang__) || defined(__GNUC__)
	#define NATIVE_DECL __attribute__((always_inline)) inline
#else
	#define NATIVE_DECL inline
#endif
#endif

namespace MISC
{
	// Old name: _GET_BENCHMARK_TIME
	NATIVE_DECL float GET_SYSTEM_TIME_STEP() { return invoke<float>(0x3F3172FEAE3AFE1C); } // 0x3F3172FEAE3AFE1C b1207
}
