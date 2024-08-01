#ifndef FLOAT128_HPP
#define FLOAT128_HPP

#ifdef Enable_Float128

	#include <quadmath.h>
	typedef __float128 fp128;
	#define PRIfp128 "Q"

	/* Math overloads */
		/* Arithmetic */
		inline fp128 fmax(fp128 x, fp128 y) { return fmaxq(x, y); }
		inline fp128 fmin(fp128 x, fp128 y) { return fminq(x, y); }
		inline fp128 fabs(fp128 x) { return fabsq(x); }
		inline fp128 fdim(fp128 x, fp128 y) { return fdimq(x, y); }
		inline fp128 fma(fp128 x, fp128 y, fp128 z) { return fmaq(x, y, z); }
		inline fp128 copysign(fp128 x, fp128 y) { return copysignq(x, y); }
		inline fp128 sqrt(fp128 x) { return sqrtq(x); }
		inline fp128 cbrt(fp128 x) { return cbrtq(x); }
		inline fp128 hypot(fp128 x, fp128 y) { return hypotq(x, y); }
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
		inline void sincos(fp128 x, fp128* p_sin, fp128* p_cos) { sincosq(x, p_sin, p_cos); }
		/* Logarithms and Exponents */
		inline fp128 log  (fp128 x) { return logq  (x); }
		inline fp128 log1p(fp128 x) { return log1pq(x); }
		inline fp128 logb (fp128 x) { return logbq (x); }
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
		inline fp128 rint (fp128 x) { return rintq (x); }
		inline fp128 round(fp128 x) { return roundq(x); }
		inline long lrint (fp128 x) { return lrintq (x); }
		inline long lround(fp128 x) { return lroundq(x); }
		inline long long llrint (fp128 x) { return llrintq (x); }
		inline long long llround(fp128 x) { return llroundq(x); }
		/* Integer and Remainder */
		inline fp128 fmod(fp128 x, fp128  y) { return fmodq(x, y); }
		inline fp128 modf(fp128 x, fp128* y) { return modfq(x, y); }
		inline fp128 nearbyint(fp128 x) { return nearbyintq(x); }
		inline fp128 nextafter(fp128 x, fp128 y) { return nextafterq(x, y); }
		inline fp128 remainder(fp128 x, fp128 y) { return remainderq(x, y); }
		inline fp128 remquo(fp128 x, fp128 y, int* quo) { return remquoq(x, y, quo); }
		/* Float Exponents */
		inline int ilogb(fp128 x) { return ilogbq(x); }
		inline fp128 frexp  (fp128 x, int* exp) { return frexpq  (x, exp); }
		inline fp128 ldexp  (fp128 x, int  exp) { return ldexpq  (x, exp); }
		inline fp128 scalbn (fp128 x, int  exp) { return scalbnq (x, exp); }
		inline fp128 scalbln(fp128 x, long exp) { return scalblnq(x, exp); }
		/* Tests */
		inline bool signbit(fp128 x) { return (signbitq(x) != 0) ? true : false; }
		inline bool isfinite(fp128 x) { return (finiteq(x) != 0) ? true : false; }
		inline bool isinf(fp128 x) { return (isinfq(x) != 0) ? true : false; }
		inline bool isnan(fp128 x) { return (isnanq(x) != 0) ? true : false; }
		inline bool issignaling(fp128 x) { return (issignalingq(x) != 0) ? true : false; }
		/* Transcendental Functions */
		inline fp128 erf (fp128 x) { return erfq (x); }
		inline fp128 erfc(fp128 x) { return erfcq(x); }
		inline fp128 lgamma(fp128 x) { return lgammaq(x); }
		inline fp128 tgamma(fp128 x) { return tgammaq(x); }

	#ifdef _WIN32
		inline fp128 stringTo_Float128(const char* nPtr, char** endPtr = nullptr) {
			return (nPtr != nullptr) ? strtoflt128(nPtr, endPtr) : static_cast<fp128>(0.0);
		}
	#else
		inline fp128 stringTo_Float128(const char* nPtr, char** endPtr = nullptr) {
			return (nPtr != nullptr) ? strtof128  (nPtr, endPtr) : static_cast<fp128>(0.0);
		}
	#endif

#endif

#endif /* FLOAT128_HPP */