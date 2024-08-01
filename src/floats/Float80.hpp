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
		return (nPtr != nullptr) ? strtold(nPtr, endPtr) : static_cast<fp80>(0.0);
	}

#endif

#endif /* FLOAT80_HPP */