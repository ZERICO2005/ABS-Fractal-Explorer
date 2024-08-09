/*
**	Author: zerico2005 (2024)
**	Project: LIB-Dekker-Float
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef DOUBLE_FLOAT64_HPP
#define DOUBLE_FLOAT64_HPP

#include <cstdint>
#include <math.h>
#include <cmath>
#include <cfenv>

typedef float fp32;
typedef double fp64;

// Can be changed to other types for better accuracy
#if defined(Enable_Float128)
	#include "Float128.hpp"
	typedef fp128 fp64x2_Math;
#elif defined(Enable_Float80)
	#include "Float80.hpp"
	typedef fp80 fp64x2_Math;
#else
	typedef fp64 fp64x2_Math;
#endif

/**
 * @brief Double-Float64 Dekker Float implementation.
 * Source: Creel "Double it Like Dekker" on YouTube.
 *
 * @warning -Ofast may break this library. -O3 compiles okay on gcc and clang.
 */
struct Float64x2 {
	fp64 hi;
	fp64 lo;

	/* Arithmetic */

	static inline Float64x2 Dekker_Add(
		const Float64x2& x, const Float64x2& y
	) {
		fp64 r_hi = x.hi + y.hi;
		fp64 r_lo = static_cast<fp64>(0.0);
		if (fabs(x.hi) > fabs(y.hi)) {
			r_lo = x.hi - r_hi + y.hi + y.lo + x.lo;
		} else {
			r_lo = y.hi - r_hi + x.hi + x.lo + y.lo;
		}

		Float64x2 c;
		c.hi = r_hi + r_lo;
		c.lo = r_hi - c.hi + r_lo;
		return c;
	}

	static inline Float64x2 Dekker_Sub(
		const Float64x2& x, const Float64x2& y
	) {
		fp64 r_hi = x.hi - y.hi;
		fp64 r_lo = static_cast<fp64>(0.0);
		if (fabs(x.hi) > fabs(y.hi)) {
			r_lo = x.hi - r_hi - y.hi - y.lo + x.lo;
		} else {
			r_lo = -y.hi - r_hi + x.hi + x.lo - y.lo;
		}

		Float64x2 c;
		c.hi = r_hi + r_lo;
		c.lo = r_hi - c.hi + r_lo;
		return c;
	}

	static constexpr fp64 Dekker_Scale = 134217729.0; // (2^ceil(53 / 2) + 1)
	
	static inline Float64x2 Dekker_Split(const fp64& x) {
		fp64 p = x * Dekker_Scale;
		Float64x2 r;
		r.hi = (x - p) + p;
		r.lo = x - r.hi;
		return r;
	}
	
	// static constexpr uint64_t Dekker_Split_Mask = ~((uint64_t)0x3FFFFFF);

	// static inline Float64x2 Dekker_Split(const fp64& x) {
	// 	Float64x2 r;
	// 	uint64_t temp = (*(uint64_t*)((void*)&x)) & Dekker_Split_Mask;
	// 	r.hi = (*(fp64*)((void*)&temp));
	// 	r.lo = x - r.hi;
	// 	return r;
	// }

	static inline Float64x2 Dekker_Mul12(
		const fp64& x, const fp64& y
	) {
		Float64x2 a = Dekker_Split(x);
		Float64x2 b = Dekker_Split(y);
		fp64 p = a.hi * b.hi;
		fp64 q = a.hi * b.lo + a.lo * b.hi;

		Float64x2 r;
		r.hi = p + q;
		r.lo = p - r.hi + q + a.lo * b.lo;
		return r;
	}

	static inline Float64x2 Dekker_Mul(
		const Float64x2& x, const Float64x2& y
	) {
		Float64x2 t = Dekker_Mul12(x.hi, y.hi);
		fp64 c = x.hi * y.lo + x.lo * y.hi + t.lo;

		Float64x2 r;
		r.hi = t.hi + c;
		r.lo = t.hi - r.hi + c;
		return r;
	}

	static inline Float64x2 Dekker_Div(
		const Float64x2& x, const Float64x2& y
	) {
		Float64x2 u;
		u.hi = x.hi / y.hi;
		Float64x2 t = Dekker_Mul12(u.hi, y.hi);
		fp64 l = (x.hi - t.hi - t.lo + x.lo - u.hi * y.lo) / y.hi;

		Float64x2 r;
		r.hi = u.hi + l;
		r.lo = u.hi - r.hi + l;
		return r;
	}

	static inline Float64x2 Dekker_Sqr12(
		const fp64& x
	) {
		Float64x2 a = Dekker_Split(x);
		fp64 p = a.hi * a.hi;
		fp64 q = static_cast<fp64>(2.0) * a.hi * a.lo;

		Float64x2 r;
		r.hi = p + q;
		r.lo = p - r.hi + q + a.lo * a.lo;
		return r;
	}

	static inline Float64x2 Dekker_Sqr(
		const Float64x2& x
	) {
		Float64x2 t = Dekker_Sqr12(x.hi);
		fp64 c = static_cast<fp64>(2.0) * x.hi * x.lo + t.lo;

		Float64x2 r;
		r.hi = t.hi + c;
		r.lo = t.hi - r.hi + c;
		return r;
	}

	/* Double-Single Arithmetic */

	static inline Float64x2 Dekker_Add_Float64(
		const Float64x2& x, const fp64& y
	) {
		fp64 r_hi = x.hi + y;
		fp64 r_lo = static_cast<fp64>(0.0);
		if (fabs(x.hi) > fabs(y)) {
			r_lo = x.hi - r_hi + y + x.lo;
		} else {
			r_lo = y - r_hi + x.hi + x.lo;
		}

		Float64x2 c;
		c.hi = r_hi + r_lo;
		c.lo = r_hi - c.hi + r_lo;
		return c;
	}

	static inline Float64x2 Dekker_Sub_Float64(
		const Float64x2& x, const fp64& y
	) {
		fp64 r_hi = x.hi - y;
		fp64 r_lo = static_cast<fp64>(0.0);
		if (fabs(x.hi) > fabs(y)) {
			r_lo = x.hi - r_hi - y + x.lo;
		} else {
			r_lo = -y - r_hi + x.hi + x.lo;
		}

		Float64x2 c;
		c.hi = r_hi + r_lo;
		c.lo = r_hi - c.hi + r_lo;
		return c;
	}

	static inline Float64x2 Dekker_Mul_Float64(
		const Float64x2& x, const fp64& y
	) {
		Float64x2 t = Dekker_Mul12(x.hi, y);
		fp64 c = x.lo * y + t.lo;

		Float64x2 r;
		r.hi = t.hi + c;
		r.lo = t.hi - r.hi + c;
		return r;
	}

	static inline Float64x2 Dekker_Div_Float64(
		const Float64x2& x, const fp64& y
	) {
		Float64x2 u;
		u.hi = x.hi / y;
		Float64x2 t = Dekker_Mul12(u.hi, y);
		fp64 l = (x.hi - t.hi - t.lo + x.lo) / y;

		Float64x2 r;
		r.hi = u.hi + l;
		r.lo = u.hi - r.hi + l;
		return r;
	}

	static inline Float64x2 Float64_Div_Dekker(
		const fp64& x, const Float64x2& y
	) {
		Float64x2 u;
		u.hi = x / y.hi;
		Float64x2 t = Dekker_Mul12(u.hi, y.hi);
		fp64 l = (x - t.hi - t.lo - u.hi * y.lo) / y.hi;

		Float64x2 r;
		r.hi = u.hi + l;
		r.lo = u.hi - r.hi + l;
		return r;
	}

/* Arithmetic */

	inline Float64x2 operator+(const Float64x2 &value) const {
		return Dekker_Add(*this, value);
	}

	inline Float64x2 operator-(const Float64x2 &value) const {
		return Dekker_Sub(*this, value);
	}

	inline Float64x2 operator*(const Float64x2 &value) const {
		return Dekker_Mul(*this, value);
	}

	inline Float64x2 operator/(const Float64x2 &value) const {
		return Dekker_Div(*this, value);
	}

	inline Float64x2 operator+(const fp64 &value) const {
		return Dekker_Add_Float64(*this, value);
	}

	inline Float64x2 operator-(const fp64 &value) const {
		return Dekker_Sub_Float64(*this, value);
	}

	inline Float64x2 operator*(const fp64 &value) const {
		return Dekker_Mul_Float64(*this, value);
	}

	inline Float64x2 operator/(const fp64 &value) const {
		return Dekker_Div_Float64(*this, value);
	}

	inline Float64x2 operator-() const {
		Float64x2 value = *this;
		value.hi = -value.hi;
		value.lo = -value.lo;
		return value;
	}

/* Increment/Decrement */

	inline Float64x2& operator++() {
		*this = Dekker_Add(*this, static_cast<Float64x2>(1.0));
		return *this;
	}

	inline Float64x2& operator--() {
		*this = Dekker_Sub(*this, static_cast<Float64x2>(1.0));
		return *this;
	}

	inline Float64x2 operator++(int) {
		Float64x2 temp = *this;
		*this = Dekker_Add(*this, static_cast<Float64x2>(1.0));
		return temp;
	}

	inline Float64x2 operator--(int) {
		Float64x2 temp = *this;
		*this = Dekker_Sub(*this, static_cast<Float64x2>(1.0));
		return temp;
	}

/* Compound Assignment */

	inline Float64x2& operator+=(const Float64x2 &value) {
		*this = Dekker_Add(*this, value);
		return *this;
	}

	inline Float64x2& operator-=(const Float64x2 &value) {
		*this = Dekker_Sub(*this, value);
		return *this;
	}

	inline Float64x2& operator*=(const Float64x2 &value) {
		*this = Dekker_Mul(*this, value);
		return *this;
	}

	inline Float64x2& operator/=(const Float64x2 &value) {
		*this = Dekker_Div(*this, value);
		return *this;
	}

/* Double-Single Compound Assignment */

	inline Float64x2& operator+=(const fp64 &value) {
		*this = Dekker_Add_Float64(*this, value);
		return *this;
	}

	inline Float64x2& operator-=(const fp64 &value) {
		*this = Dekker_Sub_Float64(*this, value);
		return *this;
	}

	inline Float64x2& operator*=(const fp64 &value) {
		*this = Dekker_Mul_Float64(*this, value);
		return *this;
	}

	inline Float64x2& operator/=(const fp64 &value) {
		*this = Dekker_Div_Float64(*this, value);
		return *this;
	}

/* Comparison */

	inline bool operator==(const Float64x2 &value) const {
		return (
			this->hi == value.hi &&
			this->lo == value.lo
		) ? true : false;
	}
	inline bool operator!=(const Float64x2 &value) const {
		return (
			this->hi != value.hi ||
			this->lo != value.lo
		) ? true : false;
	}

	inline bool operator<(const Float64x2 &value) const {
		if (this->hi == value.hi) {
			return (this->lo < value.lo);
		}
		return (this->hi < value.hi);
	}

	inline bool operator<=(const Float64x2 &value) const {
		if (this->hi == value.hi) {
			return (this->lo <= value.lo);
		}
		return (this->hi <= value.hi);
	}

	inline bool operator>(const Float64x2 &value) const {
		if (this->hi == value.hi) {
			return (this->lo > value.lo);
		}
		return (this->hi > value.hi);
	}

	inline bool operator>=(const Float64x2 &value) const {
		if (this->hi == value.hi) {
			return (this->lo >= value.lo);
		}
		return (this->hi >= value.hi);
	}

/* Constructors */

	constexpr inline Float64x2() : hi(), lo() {}

	constexpr inline Float64x2(const fp32& value) :
		hi((fp64)value), lo(0.0) {}

	constexpr inline Float64x2(const fp64& value) :
		hi(value), lo(0.0) {}

	template<typename fpX>
	constexpr inline Float64x2(const fpX& value) :
		hi((fp64)value), lo((fp64)(value - (fpX)this->hi)) {}

/* Casts */

	constexpr inline operator fp32() const {
		return (fp32)this->hi;
	}
	constexpr inline operator fp64() const {
		return this->hi;
	}

	template<typename fpX>
	constexpr inline operator fpX() const {
		return (fpX)this->hi + (fpX)this->lo;
	}

};

typedef Float64x2 fp64x2;

/* Math functions (Natively implemented) */

	/* Arithmetic */

	inline fp64x2 fmax(fp64x2 x, fp64x2 y) {
		return (x > y) ? x : y;
	}
	// inline fp64x2 fmax(fp64x2 x, fp64x2 y, fp64x2 z) {
	// 	return (x > y) ?
	// 	((x > z) ? x : z) :
	// 	((y > z) ? y : z);
	// }
	inline fp64x2 fmin(fp64x2 x, fp64x2 y) {
		return (x < y) ? x : y;
	}
	// inline fp64x2 fmin(fp64x2 x, fp64x2 y, fp64x2 z) {
	// 	return (x < y) ?
	// 	((x < z) ? x : z) :
	// 	((y < z) ? y : z);
	// }
	inline fp64x2 fabs(fp64x2 x) {
		return (x < static_cast<fp64x2>(0.0)) ? -x : x;
	}
	inline fp64x2 fdim(fp64x2 x, fp64x2 y) {
		return (x > y) ? (x - y) : static_cast<fp64x2>(0.0);
	}
	inline fp64x2 fma(fp64x2 x, fp64x2 y, fp64x2 z) {
		return (x * y) + z;
	}
	inline fp64x2 copysign(fp64x2 x, fp64x2 y) {
		return (
			(x.hi < static_cast<fp64>(0.0)) != (y.hi < (static_cast<fp64>(0.0)))
		) ? -x : x;
	}
	/** @note This function name may change to square() or etc */
	inline fp64x2 sqr(fp64x2 x) {
		return Float64x2::Dekker_Sqr(x);
	}
	inline fp64x2 sqrt(fp64x2 x) {
		if (x == static_cast<fp64x2>(0.0)) {
			return x;
		}
		fp64x2 guess = (fp64x2)sqrt(x.hi);
		return (guess + x / guess) * static_cast<fp64>(0.5);
	}
	inline fp64x2 cbrt(fp64x2 x) {
		if (x == static_cast<fp64x2>(0.0)) {
			return x;
		}
		fp64x2 guess = (fp64x2)cbrt(x.hi);
		return (
			guess * static_cast<fp64>(2.0) + (x) / Float64x2::Dekker_Sqr(guess)
		) / static_cast<fp64>(3.0);
	}
	inline fp64x2 hypot(fp64x2 x, fp64x2 y) {
		return sqrt(
			Float64x2::Dekker_Sqr(x) + Float64x2::Dekker_Sqr(y)
		);
	}
	// inline fp64x2 hypot(fp64x2 x, fp64x2 y, fp64x2 z) {
	// 	return sqrt(
	// 		Float64x2::Dekker_Sqr(x) + Float64x2::Dekker_Sqr(y) + Float64x2::Dekker_Sqr(z)
	// 	);
	// }

	/* Tests */

	inline bool signbit(fp64x2 x) {
		return (x.hi < 0.0) ? true : false;
	}
	/** Returns true if both x.hi and x.lo are finite */
	inline bool isfinite(fp64x2 x) {
		return (isfinite(x.hi) && isfinite(x.lo));
	}
	/** Returns true if either x.hi or x.lo are infinite */
	inline bool isinf(fp64x2 x) {
		return (isinf(x.hi) || isinf(x.lo));
	}
	/** Returns true if either x.hi or x.lo are nan */
	inline bool isnan(fp64x2 x) {
		return (isnan(x.hi) || isnan(x.lo));
	}
	/** Returns true if both x.hi and x.lo are normal */
	inline bool isnormal(fp64x2 x) {
		return (isnormal(x.hi) && isnormal(x.lo));
	}
	/** Returns true if either {x.hi, y.hi} or {x.lo, y.lo} are unordered */
	inline bool isunordered(fp64x2 x, fp64x2 y) {
		return (isunordered(x.hi, y.hi) || isunordered(x.lo, y.lo));
	}
	inline int fpclassify(fp64x2 x) {
		if (isfinite(x)) { return FP_INFINITE; }
		if (isnan(x)) { return FP_NAN; }
		if (x == static_cast<fp64x2>(0.0)) { return FP_ZERO; }
		if (isnormal(x)) { return FP_NORMAL; }
		return FP_SUBNORMAL;
	}

	/* Comparison */

	inline bool isgreater(fp64x2 x, fp64x2 y) {
		return (x > y);
	}
	inline bool isgreaterequal(fp64x2 x, fp64x2 y) {
		return (x >= y);
	}
	inline bool isless(fp64x2 x, fp64x2 y) {
		return (x < y);
	}
	inline bool islessequal(fp64x2 x, fp64x2 y) {
		return (x <= y);
	}
	inline bool islessgreater(fp64x2 x, fp64x2 y) {
		return (x < y) || (x > y);
	}
	
	/* Rounding */

	inline fp64x2 trunc(fp64x2 x) {
		fp64x2 int_hi = trunc(x.hi);
		fp64x2 int_lo = trunc(x.lo);
		fp64 frac_hi = x.hi - int_hi.hi;
		fp64 frac_lo = x.lo - int_lo.hi;
		// Sum in increasing order
		fp64x2 trunc_all = static_cast<fp64x2>(0.0);
		trunc_all += (
			(fp64x2)frac_hi + (fp64x2)frac_lo >= static_cast<fp64x2>(1.0)
		) ? static_cast<fp64x2>(1.0) : static_cast<fp64x2>(0.0);
		trunc_all += int_lo;
		trunc_all += int_hi;
		return trunc_all;
	}
	inline fp64x2 floor(fp64x2 x) {
		fp64x2 int_part = trunc(x);
		return (
			x < static_cast<fp64x2>(0.0) && int_part != x
		) ? int_part - static_cast<fp64x2>(1.0) : int_part;
	}
	inline fp64x2 ceil(fp64x2 x) {
		fp64x2 int_part = trunc(x);
		return (
			x > static_cast<fp64x2>(0.0) && int_part != x
		) ? int_part + static_cast<fp64x2>(1.0) : int_part;
	}
	inline fp64x2 round(fp64x2 x) {
		fp64x2 int_part = trunc(x);
		fp64x2 frac_part = x - int_part;
		if (x >= static_cast<fp64x2>(0.0)) {
			if (frac_part >= static_cast<fp64x2>(0.5)) {
				return int_part + static_cast<fp64x2>(1.0);
			}
			return int_part;
		}
		if (frac_part <= static_cast<fp64x2>(-0.5)) {
			return int_part - static_cast<fp64x2>(1.0);
		}
		return int_part;
	}
	inline fp64x2 rint(fp64x2 x) {
		switch (fegetround()) {
			default:
			case FE_TOWARDZERO:
				return trunc(x);
			case FE_DOWNWARD:
				return floor(x);
			case FE_UPWARD:
				return ceil(x);
			case FE_TONEAREST:
				return round(x);
		}
	}
	inline long lround(fp64x2 x) {
		return (long)round(x);
	}
	inline long lrint(fp64x2 x) {
		return (long)rint(x);
	}
	inline long long llround(fp64x2 x) {
		return (long long)round(x);
	}
	inline long long llrint(fp64x2 x) {
		return (long long)rint(x);
	}

	/* Integer and Remainder */

	inline fp64x2 modf(fp64x2 x, fp64x2* int_part) {
		fp64x2 trunc_part = trunc(x);
		if (int_part != nullptr) {
			*int_part = trunc_part;
		}
		return x - trunc_part;
	}
	inline fp64x2 nearbyint(fp64x2 x) {
		return rint(x);
	}
	
	/* Float Exponents */

	inline fp64x2 ldexp(fp64x2 x, int exp) {
		x.hi = ldexp(x.hi, exp);
		x.lo = isfinite(x.hi) ? ldexp(x.lo, exp) : x.hi;
		return x;
	}
	inline fp64x2 scalbn(fp64x2 x, int exp) {
		x.hi = scalbn(x.hi, exp);
		x.lo = isfinite(x.hi) ? scalbn(x.lo, exp) : x.hi;
		return x;
	}
	inline fp64x2 scalbln(fp64x2 x, long exp) {
		x.hi = scalbln(x.hi, exp);
		x.lo = isfinite(x.hi) ? scalbln(x.lo, exp) : x.hi;
		return x;
	}

/* Math overloads (Casts to other types) */

		/* Arithmetic */
		// inline fp64x2 fmax(fp64x2 x, fp64x2 y) { return (fp64x2)fmax((fp64x2_Math)x, (fp64x2_Math)y); }
		// inline fp64x2 fmin(fp64x2 x, fp64x2 y) { return (fp64x2)fmin((fp64x2_Math)x, (fp64x2_Math)y); }
		// inline fp64x2 fabs(fp64x2 x) { return (fp64x2)fabs((fp64x2_Math)x); }
		// inline fp64x2 fdim(fp64x2 x, fp64x2 y) { return (fp64x2)fdim((fp64x2_Math)x, (fp64x2_Math)y); }
		// inline fp64x2 fma(fp64x2 x, fp64x2 y, fp64x2 z) { return (fp64x2)fma((fp64x2_Math)x, (fp64x2_Math)y, (fp64x2_Math)z); }
		// inline fp64x2 copysign(fp64x2 x, fp64x2 y) { return (fp64x2)copysign((fp64x2_Math)x, (fp64x2_Math)y); }
		// inline fp64x2 sqrt(fp64x2 x) { return (fp64x2)sqrt((fp64x2_Math)x); }
		// inline fp64x2 cbrt(fp64x2 x) { return (fp64x2)cbrt((fp64x2_Math)x); }
		// inline fp64x2 hypot(fp64x2 x, fp64x2 y) { return (fp64x2)hypot((fp64x2_Math)x, (fp64x2_Math)y); }
		/* Trigonometry */
		inline fp64x2  sin (fp64x2 x) { return (fp64x2) sin ((fp64x2_Math)x); }
		inline fp64x2  cos (fp64x2 x) { return (fp64x2) cos ((fp64x2_Math)x); }
		inline fp64x2  tan (fp64x2 x) { return (fp64x2) tan ((fp64x2_Math)x); }
		inline fp64x2 asin (fp64x2 x) { return (fp64x2)asin ((fp64x2_Math)x); }
		inline fp64x2 acos (fp64x2 x) { return (fp64x2)acos ((fp64x2_Math)x); }
		inline fp64x2 atan (fp64x2 x) { return (fp64x2)atan ((fp64x2_Math)x); }
		inline fp64x2  sinh(fp64x2 x) { return (fp64x2) sinh((fp64x2_Math)x); }
		inline fp64x2  cosh(fp64x2 x) { return (fp64x2) cosh((fp64x2_Math)x); }
		inline fp64x2  tanh(fp64x2 x) { return (fp64x2) tanh((fp64x2_Math)x); }
		inline fp64x2 asinh(fp64x2 x) { return (fp64x2)asinh((fp64x2_Math)x); }
		inline fp64x2 acosh(fp64x2 x) { return (fp64x2)acosh((fp64x2_Math)x); }
		inline fp64x2 atanh(fp64x2 x) { return (fp64x2)atanh((fp64x2_Math)x); }
		inline fp64x2 atan2(fp64x2 y, fp64x2 x) { return (fp64x2)atan2((fp64x2_Math)y, (fp64x2_Math)x); }
		inline void sincos(fp64x2 x, fp64x2* p_sin, fp64x2* p_cos) {
			*p_sin = sin(x);
			*p_cos = cos(x);
		}
		/* Logarithms and Exponents */
		inline fp64x2 log  (fp64x2 x) { return (fp64x2)log  ((fp64x2_Math)x); }
		inline fp64x2 log1p(fp64x2 x) { return (fp64x2)log1p((fp64x2_Math)x); }
		inline fp64x2 logb (fp64x2 x) { return (fp64x2)logb ((fp64x2_Math)x); }
		inline fp64x2 log2 (fp64x2 x) { return (fp64x2)log2 ((fp64x2_Math)x); }
		inline fp64x2 log10(fp64x2 x) { return (fp64x2)log10((fp64x2_Math)x); }
		inline fp64x2 exp  (fp64x2 x) { return (fp64x2)exp  ((fp64x2_Math)x); }
		inline fp64x2 expm1(fp64x2 x) { return (fp64x2)expm1((fp64x2_Math)x); }
		inline fp64x2 exp2 (fp64x2 x) { return (fp64x2)exp2 ((fp64x2_Math)x); }
		inline fp64x2 pow(fp64x2 x, fp64x2 y) { return (fp64x2)pow((fp64x2_Math)x, (fp64x2_Math)y); }
		/* Rounding */
		// inline fp64x2 trunc(fp64x2 x) { return (fp64x2)trunc((fp64x2_Math)x); }
		// inline fp64x2 floor(fp64x2 x) { return (fp64x2)floor((fp64x2_Math)x); }
		// inline fp64x2 ceil (fp64x2 x) { return (fp64x2)ceil ((fp64x2_Math)x); }
		// inline fp64x2 rint (fp64x2 x) { return (fp64x2)rint ((fp64x2_Math)x); }
		// inline fp64x2 round(fp64x2 x) { return (fp64x2)round((fp64x2_Math)x); }
		// inline long lrint (fp64x2 x) { return lrint ((fp64x2_Math)x); }
		// inline long lround(fp64x2 x) { return lround((fp64x2_Math)x); }
		// inline long long llrint (fp64x2 x) { return llrint ((fp64x2_Math)x); }
		// inline long long llround(fp64x2 x) { return llround((fp64x2_Math)x); }
		/* Integer and Remainder */
		inline fp64x2 fmod(fp64x2 x, fp64x2 y) { return (fp64x2)fmod((fp64x2_Math)x, (fp64x2_Math)y); }
		// inline fp64x2 modf(fp64x2 x, fp64x2* y) {
		// 	fp64x2_Math y_temp;
		// 	fp64x2 result = modf((fp64x2_Math)x, &y_temp);
		// 	*y = (fp64x2)y_temp;
		// 	return result;
		// }
		// inline fp64x2 nearbyint(fp64x2 x) { return (fp64x2)nearbyint((fp64x2_Math)x); }
		// Incorrect Function // inline fp64x2 nextafter(fp64x2 x, fp64x2 y) { return (fp64x2)nextafter((fp64x2_Math)x, (fp64x2_Math)y); }
		inline fp64x2 remainder(fp64x2 x, fp64x2 y) { return (fp64x2)remainder((fp64x2_Math)x, (fp64x2_Math)y); }
		inline fp64x2 remquo(fp64x2 x, fp64x2 y, int* quo) { return (fp64x2)remquo((fp64x2_Math)x, (fp64x2_Math)y, quo); }
		/* Float Exponents */
		inline int ilogb(fp64x2 x) { return ilogb((fp64x2_Math)x); }
		inline fp64x2 frexp  (fp64x2 x, int* exp) { return (fp64x2)frexp  ((fp64x2_Math)x, exp); }
		// inline fp64x2 ldexp  (fp64x2 x, int  exp) { return (fp64x2)ldexp  ((fp64x2_Math)x, exp); }
		// inline fp64x2 scalbn (fp64x2 x, int  exp) { return (fp64x2)scalbn ((fp64x2_Math)x, exp); }
		// inline fp64x2 scalbln(fp64x2 x, long exp) { return (fp64x2)scalbln((fp64x2_Math)x, exp); }
		/* Tests */
		// inline bool signbit(fp64x2 x) { return (signbit((fp64x2_Math)x) != 0) ? true : false; }
		// inline bool isfinite(fp64x2 x) { return (isfinite((fp64x2_Math)x) != 0) ? true : false; }
		// inline bool isinf(fp64x2 x) { return (isinf((fp64x2_Math)x) != 0) ? true : false; }
		// inline bool isnan(fp64x2 x) { return (isnan((fp64x2_Math)x) != 0) ? true : false; }
		/* Transcendental Functions */
		inline fp64x2 erf (fp64x2 x) { return (fp64x2)erf ((fp64x2_Math)x); }
		inline fp64x2 erfc(fp64x2 x) { return (fp64x2)erfc((fp64x2_Math)x); }
		inline fp64x2 lgamma(fp64x2 x) { return (fp64x2)lgamma((fp64x2_Math)x); }
		inline fp64x2 tgamma(fp64x2 x) { return (fp64x2)tgamma((fp64x2_Math)x); }

	/* Strings */

		#include "double_FloatN_stringTo.hpp"

		inline Float64x2 stringTo_Float64x2(const char* nPtr, char** endPtr = nullptr) {
			internal_double_FloatN_stringTo<Float64x2, fp64> stringTo_func;
			return stringTo_func.stringTo_FloatNx2(nPtr, endPtr);
		}

		#include "double_FloatN_snprintf.hpp"

		#define PRIFloat64x2 "D"
		#define PRIfp64x2 "D"

		/**
		 * @brief snprintf a singular Float64x2/fp64x2.
		 * Similar in functionallity to quadmath_snprintf.
		 * @note $ not supported. This function ignore additional
		 * format specifiers.
		 * @return -1 on encoding failure. Otherwise the total length of the
		 * string excluding the \0 terminator and ignoring the buffer size.
		 */
		inline int Float64x2_snprintf(
			char* buf, size_t len,
			const char* format, ...
		) {
			va_list args;
			va_start(args, format);
			internal_double_FloatN_snprintf<Float64x2> func_snprintf;
			int ret_val = func_snprintf.FloatNx2_snprintf(
				PRIFloat64x2, buf, len,
				format, args
			);
			va_end(args);
			return ret_val;
		}

#endif /* DOUBLE_FLOAT64_HPP */