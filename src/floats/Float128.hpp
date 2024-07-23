#ifndef FLOAT128_HPP
#define FLOAT128_HPP

#ifdef Enable_Float128

	#include <quadmath.h>
	typedef __float128 fp128;
	#define PRIfp128 "Q"

	/* Math overloads */
		/* Trigonometry */
		inline fp128  sin (fp128 x) { return  sinq (x); }
		inline fp128  cos (fp128 x) { return  cosq (x); }
		inline fp128  tan (fp128 x) { return  tanq (x); }
		inline fp128 asin (fp128 x) { return asinq (x); }
		inline fp128 acos (fp128 x) { return acosq (x); }
		inline fp128 atan (fp128 x) { return atanq (x); }
		inline fp128  sinh(fp128 x) { return  sinhq(x); }
		inline fp128  cosh(fp128 x) { return  coshq(x); }
		inline fp128  tanh(fp128 x) { return  tanhq(x); }
		inline fp128 asinh(fp128 x) { return asinhq(x); }
		inline fp128 acosh(fp128 x) { return acoshq(x); }
		inline fp128 atanh(fp128 x) { return atanhq(x); }
		inline fp128 atan2(fp128 y, fp128 x) { return atan2q(y, x); }
		inline fp128 hypot(fp128 x, fp128 y) { return hypotq(x, y); }
		/* Logarithms and Exponents */
		inline fp128 log  (fp128 x) { return logq  (x); }
		inline fp128 log1p(fp128 x) { return log1pq(x); }
		inline fp128 log2 (fp128 x) { return log2q (x); }
		inline fp128 log10(fp128 x) { return log10q(x); }
		inline fp128 exp  (fp128 x) { return expq  (x); }
		inline fp128 expm1(fp128 x) { return expm1q(x); }
		inline fp128 exp2 (fp128 x) { return exp2q (x); }
		inline fp128 pow(fp128 x, fp128 y) { return powq(x, y); }
		/* Rounding */
		inline fp128 trunc(fp128 x) { return truncq(x); }
		inline fp128 floor(fp128 x) { return floorq(x); }
		inline fp128 ceil (fp128 x) { return ceilq (x); }
		inline fp128 round(fp128 x) { return roundq(x); }
		/* Others */
		inline fp128 fmax(fp128 x, fp128 y) { return fmaxq(x, y); }
		inline fp128 fmin(fp128 x, fp128 y) { return fminq(x, y); }
		inline fp128 fmod(fp128 x, fp128 y) { return fmodq(x, y); }
		inline fp128 fabs(fp128 x) { return fabsq(x); }
		inline fp128 copysign(fp128 x, fp128 y) { return copysignq(x, y); }
		inline bool signbit(fp128 x) { return (signbitq(x) != 0) ? true : false; }
		inline fp128 sqrt(fp128 x) { return sqrtq(x); }
		inline fp128 cbrt(fp128 x) { return cbrtq(x); }

	#ifdef _WIN32
		inline fp128 stringTo_Float128(const char* nPtr, char** endPtr = nullptr) {
			return (nPtr != nullptr) ? strtoflt128(nPtr, endPtr) : (fp128)0.0;
		}
	#else
		inline fp128 stringTo_Float128(const char* nPtr, char** endPtr = nullptr) {
			return (nPtr != nullptr) ? strtof128  (nPtr, endPtr) : (fp128)0.0;
		}
	#endif

#endif

#endif /* FLOAT128_HPP */