#ifndef FLOAT80_HPP
#define FLOAT80_HPP

#ifdef Enable_Float80

	#include <climits>
	#include <cfloat>
	#include <cstdlib>
	#include <math.h>
	#include <cmath>

	#if defined(__GNUC__) && !defined(__clang__)
		typedef __float80 fp80;
		#define PRIfp80 "L"
	#else
		typedef long double fp80;
		#define PRIfp80 "L"

		static_assert(
			LDBL_MANT_DIG > DBL_MANT_DIG && LDBL_EPSILON < DBL_EPSILON ,
			"Enable_Float80 failed. long double is equivilent to double."
		);
	#endif

	static_assert(
		sizeof(fp80) * CHAR_BIT >= 80,
		"Enable_Float80 failed. Float80 is less than 80 bits wide."
	);

	inline fp80 stringTo_Float80(const char* nPtr, char** endPtr = nullptr) {
		return (nPtr != nullptr) ? strtold(nPtr, endPtr) : (fp80)0.0;
	}

	inline void sincos(fp80 x, fp80* p_sin, fp80* p_cos) {
		// Unsure why sin(long double) is not defined here.
		*p_sin = sinl(x);
		*p_cos = cosl(x);
	}
#endif

#endif /* FLOAT80_HPP */