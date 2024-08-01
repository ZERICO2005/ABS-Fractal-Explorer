/*
**	Author: zerico2005 (2023 - 2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef DOUBLE_FLOAT32_HPP
#define DOUBLE_FLOAT32_HPP

#include <cstdint>
#include <math.h>
#include <cmath>
#include <cfenv>

typedef float fp32;
typedef double fp64;

typedef fp64 fp32x2_Math;

/**
 * @brief Double-Float32 Dekker Float implementation.
 * Source: Creel "Double it Like Dekker" on YouTube.
 */
class Float32x2 {
public:
	fp32 hi;
	fp32 lo;
	
private:

	inline Float32x2 Dekker_Add(
		const Float32x2& x, const Float32x2& y
	) const {
		fp32 r_hi = x.hi + y.hi;
		fp32 r_lo = 0.0f;
		if (fabsf(x.hi) > fabsf(y.hi)) {
			r_lo = x.hi - r_hi + y.hi + y.lo + x.lo;
		} else {
			r_lo = y.hi - r_hi + x.hi + x.lo + y.lo;
		}

		Float32x2 c;
		c.hi = r_hi + r_lo;
		c.lo = r_hi - c.hi + r_lo;
		return c;
	}

	inline Float32x2 Dekker_Sub(
		const Float32x2& x, const Float32x2& y
	) const {
		fp32 r_hi = x.hi - y.hi;
		fp32 r_lo = 0.0f;
		if (fabsf(x.hi) > fabsf(y.hi)) {
			r_lo = x.hi - r_hi - y.hi - y.lo + x.lo;
		} else {
			r_lo = -y.hi - r_hi + x.hi + x.lo - y.lo;
		}

		Float32x2 c;
		c.hi = r_hi + r_lo;
		c.lo = r_hi - c.hi + r_lo;
		return c;
	}

	static constexpr fp32 Dekker_Scale = 4097.0f; // (2^ceil(24 / 2) + 1)
	
	inline Float32x2 Dekker_Split(const fp32& x) const {
		fp32 p = x * Dekker_Scale;
		Float32x2 r;
		r.hi = x - p + p;
		r.lo = x - r.hi;
		return r;
	}
	
	// static constexpr uint64_t Dekker_Split_Mask = ~((uint64_t)0xFFF);

	// inline Float32x2 Dekker_Split(const fp32& x) const {
	// 	Float32x2 r;
	// 	uint64_t temp = (*(uint64_t*)((void*)&x)) & Dekker_Split_Mask;
	// 	r.hi = (*(fp32*)((void*)&temp));
	// 	r.lo = x - r.hi;
	// 	return r;
	// }

	inline Float32x2 Dekker_Mul12(
		const fp32& x, const fp32& y
	) const {
		Float32x2 a = Dekker_Split(x);
		Float32x2 b = Dekker_Split(y);
		fp32 p = a.hi * b.hi;
		fp32 q = a.hi * b.lo + a.lo * b.hi;

		Float32x2 r;
		r.hi = p + q;
		r.lo = p - r.hi + q + a.lo * b.lo;
		return r;
	}

	inline Float32x2 Dekker_Mul(
		const Float32x2& x, const Float32x2& y
	) const {
		Float32x2 t = Dekker_Mul12(x.hi, y.hi);
		fp32 c = x.hi * y.lo + x.lo * y.hi + t.lo;

		Float32x2 r;
		r.hi = t.hi + c;
		r.lo = t.hi - r.hi + c;
		return r;
	}

	inline Float32x2 Dekker_Div(
		const Float32x2& x, const Float32x2& y
	) const {
		Float32x2 u;
		u.hi = x.hi / y.hi;
		Float32x2 t = Dekker_Mul12(u.hi, y.hi);
		fp32 l = (x.hi - t.hi - t.lo + x.lo - u.hi * y.lo) / y.hi;

		Float32x2 r;
		r.hi = u.hi + l;
		r.lo = u.hi - r.hi + l;
		return r;
	}

public:

// #ifdef Enable_Float128
// 	inline void print() const {
// 		char buf[128];
// 		memset(buf, '\0', sizeof(buf));
// 		quadmath_snprintf(buf, sizeof(buf), "%.25Qg", (fp128)this->hi + (fp128)this->lo);
// 		printf("{%.25lg, %.25lg} | %s\n", this->hi, this->lo, buf);
// 	}
// #endif

/* Arithmetic */

	inline Float32x2 operator+(const Float32x2 &value) const {
		return Dekker_Add(*this, value);
	}

	inline Float32x2 operator-(const Float32x2 &value) const {
		return Dekker_Sub(*this, value);
	}

	inline Float32x2 operator*(const Float32x2 &value) const {
		return Dekker_Mul(*this, value);
	}

	inline Float32x2 operator/(const Float32x2 &value) const {
		return Dekker_Div(*this, value);
	}

	inline Float32x2 operator-() const {
		Float32x2 value = *this;
		value.hi = -value.hi;
		value.lo = -value.lo;
		return value;
	}

/* Increment/Decrement */

	inline Float32x2& operator++() {
		*this = Dekker_Add(*this, static_cast<Float32x2>(1.0));
		return *this;
	}

	inline Float32x2& operator--() {
		*this = Dekker_Sub(*this, static_cast<Float32x2>(1.0));
		return *this;
	}

	inline Float32x2 operator++(int) {
		Float32x2 temp = *this;
		*this = Dekker_Add(*this, static_cast<Float32x2>(1.0));
		return temp;
	}

	inline Float32x2 operator--(int) {
		Float32x2 temp = *this;
		*this = Dekker_Sub(*this, static_cast<Float32x2>(1.0));
		return temp;
	}

/* Compound Assignment */

	inline Float32x2& operator+=(const Float32x2 &value) {
		*this = Dekker_Add(*this, value);
		return *this;
	}

	inline Float32x2& operator-=(const Float32x2 &value) {
		*this = Dekker_Sub(*this, value);
		return *this;
	}

	inline Float32x2& operator*=(const Float32x2 &value) {
		*this = Dekker_Mul(*this, value);
		return *this;
	}

	inline Float32x2& operator/=(const Float32x2 &value) {
		*this = Dekker_Div(*this, value);
		return *this;
	}

/* Comparison */

	inline bool operator==(const Float32x2 &value) const {
		return (
			this->hi == value.hi &&
			this->lo == value.lo
		) ? true : false;
	}
	inline bool operator!=(const Float32x2 &value) const {
		return (
			this->hi != value.hi ||
			this->lo != value.lo
		) ? true : false;
	}

	inline bool operator<(const Float32x2 &value) const {
		if (this->hi == value.hi) {
			return (this->lo < value.lo);
		}
		return (this->hi < value.hi);
	}

	inline bool operator<=(const Float32x2 &value) const {
		if (this->hi == value.hi) {
			return (this->lo <= value.lo);
		}
		return (this->hi <= value.hi);
	}

	inline bool operator>(const Float32x2 &value) const {
		if (this->hi == value.hi) {
			return (this->lo > value.lo);
		}
		return (this->hi > value.hi);
	}

	inline bool operator>=(const Float32x2 &value) const {
		if (this->hi == value.hi) {
			return (this->lo >= value.lo);
		}
		return (this->hi >= value.hi);
	}

/* Constructors */

	inline Float32x2() {
		// this->hi = 0.0;
		// this->lo = 0.0;
	}

	inline Float32x2(const fp32& value) {
		this->hi = value;
		this->lo = 0.0;
	}

	inline Float32x2(const fp64& value) {
		this->hi = (fp32)value;
		this->lo = (fp32)(value - (fp64)this->hi);
	}

	template<typename fpX>
	inline Float32x2(const fpX& value) {
		this->hi = (fp32)value;
		this->lo = (fp32)(value - (fpX)this->hi);
	}

/* Casts */

	constexpr inline operator fp32() const {
		return this->hi;
	}
	constexpr inline operator fp64() const {
		return (fp64)this->hi + (fp64)this->lo;
	}

	template<typename fpX>
	constexpr inline operator fpX() const {
		return (fpX)this->hi + (fpX)this->lo;
	}

};

typedef Float32x2 fp32x2;

/* Math functions (Natively implemented) */

	/* Arithmetic */
	inline fp32x2 fmax(fp32x2 x, fp32x2 y) {
		return (x > y) ? x : y;
	}
	inline fp32x2 fmin(fp32x2 x, fp32x2 y) {
		return (x < y) ? x : y;
	}
	inline fp32x2 fabs(fp32x2 x) {
		return (x < static_cast<fp32x2>(0.0)) ? -x : x;
	}
	inline fp32x2 fdim(fp32x2 x, fp32x2 y) {
		return (x > y) ? (x - y) : static_cast<fp32x2>(0.0);
	}
	inline fp32x2 fma(fp32x2 x, fp32x2 y, fp32x2 z) {
		return (x * y) + z;
	}
	inline fp32x2 copysign(fp32x2 x, fp32x2 y) {
		return (
			(x < static_cast<fp32x2>(0.0)) != (y < (static_cast<fp32x2>(0.0)))
		) ? -x : x;
	}

	/* Tests */
	inline bool signbit(fp32x2 x) {
		return (x < static_cast<fp32x2>(0.0)) ? true : false;
	}
	/** Returns true if both x.hi and x.lo are finite */
	inline bool isfinite(fp32x2 x) {
		return (isfinite(x.hi) && isfinite(x.lo));
	}
	/** Returns true if either x.hi or x.lo are infinite */
	inline bool isinf(fp32x2 x) {
		return (isinf(x.hi) || isinf(x.lo));
	}
	/** Returns true if either x.hi or x.lo are nan */
	inline bool isnan(fp32x2 x) {
		return (isnan(x.hi) || isnan(x.lo));
	}
	/** Returns true if both x.hi and x.lo are normal */
	inline bool isnormal(fp32x2 x) {
		return (isnormal(x.hi) && isnormal(x.lo));
	}
	/** Returns true if either {x.hi, y.hi} or {x.lo, y.lo} are unordered */
	inline bool isunordered(fp32x2 x, fp32x2 y) {
		return (isunordered(x.hi, y.hi) || isunordered(x.lo, y.lo));
	}
	inline int fpclassify(fp32x2 x) {
		if (isfinite(x)) { return FP_INFINITE; }
		if (isnan(x)) { return FP_NAN; }
		if (x == static_cast<fp32x2>(0.0)) { return FP_ZERO; }
		if (isnormal(x)) { return FP_NORMAL; }
		return FP_SUBNORMAL;
	}

	/* Comparison */
	inline bool isgreater(fp32x2 x, fp32x2 y) {
		return (x > y);
	}
	inline bool isgreaterequal(fp32x2 x, fp32x2 y) {
		return (x >= y);
	}
	inline bool isless(fp32x2 x, fp32x2 y) {
		return (x < y);
	}
	inline bool islessequal(fp32x2 x, fp32x2 y) {
		return (x <= y);
	}
	inline bool islessgreater(fp32x2 x, fp32x2 y) {
		return (x < y) || (x > y);
	}

    /* Rounding */
    inline fp32x2 trunc(fp32x2 x) {
        fp32 frac_hi = x.hi - trunc(x.hi);
        fp32 frac_lo = x.lo - trunc(x.lo);
        fp32x2 int_hi = trunc(x.hi);
        fp32x2 int_lo = trunc(x.lo);
        // Sum in increasing order
        fp32x2 trunc_all = static_cast<fp32x2>(0.0);
        trunc_all += (
			(fp32x2)frac_hi + (fp32x2)frac_lo >= static_cast<fp32x2>(1.0)
		) ? static_cast<fp32x2>(1.0) : static_cast<fp32x2>(0.0);
        trunc_all += int_lo;
        trunc_all += int_hi;
        return trunc_all;
    }
	inline fp32x2 floor(fp32x2 x) {
		fp32x2 int_part = trunc(x);
		return (
			x < static_cast<fp32x2>(0.0) && int_part != x
		) ? int_part : int_part - static_cast<fp32x2>(1.0);
	}
	inline fp32x2 ceil(fp32x2 x) {
		fp32x2 int_part = trunc(x);
		return (
			x > static_cast<fp32x2>(0.0) && int_part != x
		) ? int_part + static_cast<fp32x2>(1.0) : int_part;
	}
	inline fp32x2 round(fp32x2 x) {
		fp32x2 int_part = trunc(x);
		fp32x2 frac_part = x - int_part;
		if (x >= static_cast<fp32x2>(0.0)) {
			if (frac_part >= static_cast<fp32x2>(0.5)) {
				return int_part + static_cast<fp32x2>(1.0);
			}
			return int_part;
		}
		if (frac_part <= static_cast<fp32x2>(-0.5)) {
			return int_part - static_cast<fp32x2>(1.0);
		}
		return int_part;
	}
	inline fp32x2 rint(fp32x2 x) {
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
	inline long lround(fp32x2 x) {
		return (long)round(x);
	}
	inline long lrint(fp32x2 x) {
		return (long)rint(x);
	}
	inline long long llround(fp32x2 x) {
		return (long long)round(x);
	}
	inline long long llrint(fp32x2 x) {
		return (long long)rint(x);
	}

	/* Integer and Remainder */
	inline fp32x2 nearbyint(fp32x2 x) {
		return rint(x);
	}

/* Math overloads (Casts to other types) */

		/* Arithmetic */
		// inline fp32x2 fmax(fp32x2 x, fp32x2 y) { return (fp32x2)fmax((fp32x2_Math)x, (fp32x2_Math)y); }
		// inline fp32x2 fmin(fp32x2 x, fp32x2 y) { return (fp32x2)fmin((fp32x2_Math)x, (fp32x2_Math)y); }
		// inline fp32x2 fabs(fp32x2 x) { return (fp32x2)fabs((fp32x2_Math)x); }
		// inline fp32x2 fdim(fp32x2 x, fp32x2 y) { return (fp32x2)fdim((fp32x2_Math)x, (fp32x2_Math)y); }
		// inline fp32x2 fma(fp32x2 x, fp32x2 y, fp32x2 z) { return (fp32x2)fma((fp32x2_Math)x, (fp32x2_Math)y, (fp32x2_Math)z); }
		// inline fp32x2 copysign(fp32x2 x, fp32x2 y) { return (fp32x2)copysign((fp32x2_Math)x, (fp32x2_Math)y); }
		inline fp32x2 sqrt(fp32x2 x) { return (fp32x2)sqrt((fp32x2_Math)x); }
		inline fp32x2 cbrt(fp32x2 x) { return (fp32x2)cbrt((fp32x2_Math)x); }
		inline fp32x2 hypot(fp32x2 x, fp32x2 y) { return (fp32x2)hypot((fp32x2_Math)x, (fp32x2_Math)y); }
		/* Trigonometry */
		inline fp32x2  sin (fp32x2 x) { return (fp32x2) sin ((fp32x2_Math)x); }
		inline fp32x2  cos (fp32x2 x) { return (fp32x2) cos ((fp32x2_Math)x); }
		inline fp32x2  tan (fp32x2 x) { return (fp32x2) tan ((fp32x2_Math)x); }
		inline fp32x2 asin (fp32x2 x) { return (fp32x2)asin ((fp32x2_Math)x); }
		inline fp32x2 acos (fp32x2 x) { return (fp32x2)acos ((fp32x2_Math)x); }
		inline fp32x2 atan (fp32x2 x) { return (fp32x2)atan ((fp32x2_Math)x); }
		inline fp32x2  sinh(fp32x2 x) { return (fp32x2) sinh((fp32x2_Math)x); }
		inline fp32x2  cosh(fp32x2 x) { return (fp32x2) cosh((fp32x2_Math)x); }
		inline fp32x2  tanh(fp32x2 x) { return (fp32x2) tanh((fp32x2_Math)x); }
		inline fp32x2 asinh(fp32x2 x) { return (fp32x2)asinh((fp32x2_Math)x); }
		inline fp32x2 acosh(fp32x2 x) { return (fp32x2)acosh((fp32x2_Math)x); }
		inline fp32x2 atanh(fp32x2 x) { return (fp32x2)atanh((fp32x2_Math)x); }
		inline fp32x2 atan2(fp32x2 y, fp32x2 x) { return (fp32x2)atan2((fp32x2_Math)y, (fp32x2_Math)x); }
		inline void sincos(fp32x2 x, fp32x2* p_sin, fp32x2* p_cos) {
			*p_sin = sin(x);
			*p_cos = cos(x);
		}
		/* Logarithms and Exponents */
		inline fp32x2 log  (fp32x2 x) { return (fp32x2)log  ((fp32x2_Math)x); }
		inline fp32x2 log1p(fp32x2 x) { return (fp32x2)log1p((fp32x2_Math)x); }
		inline fp32x2 logb (fp32x2 x) { return (fp32x2)logb ((fp32x2_Math)x); }
		inline fp32x2 log2 (fp32x2 x) { return (fp32x2)log2 ((fp32x2_Math)x); }
		inline fp32x2 log10(fp32x2 x) { return (fp32x2)log10((fp32x2_Math)x); }
		inline fp32x2 exp  (fp32x2 x) { return (fp32x2)exp  ((fp32x2_Math)x); }
		inline fp32x2 expm1(fp32x2 x) { return (fp32x2)expm1((fp32x2_Math)x); }
		inline fp32x2 exp2 (fp32x2 x) { return (fp32x2)exp2 ((fp32x2_Math)x); }
		inline fp32x2 pow(fp32x2 x, fp32x2 y) { return (fp32x2)pow((fp32x2_Math)x, (fp32x2_Math)y); }
		/* Rounding */
		// inline fp32x2 trunc(fp32x2 x) { return (fp32x2)trunc((fp32x2_Math)x); }
		// inline fp32x2 floor(fp32x2 x) { return (fp32x2)floor((fp32x2_Math)x); }
		// inline fp32x2 ceil (fp32x2 x) { return (fp32x2)ceil ((fp32x2_Math)x); }
		// inline fp32x2 rint (fp32x2 x) { return (fp32x2)rint ((fp32x2_Math)x); }
		// inline fp32x2 round(fp32x2 x) { return (fp32x2)round((fp32x2_Math)x); }
		// inline long lrint (fp32x2 x) { return lrint ((fp32x2_Math)x); }
		// inline long lround(fp32x2 x) { return lround((fp32x2_Math)x); }
		// inline long long llrint (fp32x2 x) { return llrint ((fp32x2_Math)x); }
		// inline long long llround(fp32x2 x) { return llround((fp32x2_Math)x); }
		/* Integer and Remainder */
		inline fp32x2 fmod(fp32x2 x, fp32x2 y) { return (fp32x2)fmod((fp32x2_Math)x, (fp32x2_Math)y); }
		inline fp32x2 modf(fp32x2 x, fp32x2* y) {
			fp32x2_Math y_temp;
			fp32x2 result = modf((fp32x2_Math)x, &y_temp);
			*y = (fp32x2)y_temp;
			return result;
		}
		// inline fp32x2 nearbyint(fp32x2 x) { return (fp32x2)nearbyint((fp32x2_Math)x); }
		// Incorrect Function // inline fp32x2 nextafter(fp32x2 x) { return (fp32x2)nextafter((fp32x2_Math)x); }
		inline fp32x2 remainder(fp32x2 x, fp32x2 y) { return (fp32x2)remainder((fp32x2_Math)x, (fp32x2_Math)y); }
		inline fp32x2 remquo(fp32x2 x, fp32x2 y, int* quo) { return (fp32x2)remquo((fp32x2_Math)x, (fp32x2_Math)y, quo); }
		/* Float Exponents */
		inline int ilogb(fp32x2 x) { return ilogb((fp32x2_Math)x); }
		inline fp32x2 frexp  (fp32x2 x, int* exp) { return (fp32x2)frexp  ((fp32x2_Math)x, exp); }
		inline fp32x2 ldexp  (fp32x2 x, int  exp) { return (fp32x2)ldexp  ((fp32x2_Math)x, exp); }
		inline fp32x2 scalbn (fp32x2 x, int  exp) { return (fp32x2)scalbn ((fp32x2_Math)x, exp); }
		inline fp32x2 scalbln(fp32x2 x, long exp) { return (fp32x2)scalbln((fp32x2_Math)x, exp); }
		/* Tests */
		// inline bool signbit(fp32x2 x) { return (signbit((fp32x2_Math)x) != 0) ? true : false; }
		// inline bool isfinite(fp32x2 x) { return (isfinite((fp32x2_Math)x) != 0) ? true : false; }
		// inline bool isinf(fp32x2 x) { return (isinf((fp32x2_Math)x) != 0) ? true : false; }
		// inline bool isnan(fp32x2 x) { return (isnan((fp32x2_Math)x) != 0) ? true : false; }
		/* Transcendental Functions */
		inline fp32x2 erf (fp32x2 x) { return (fp32x2)erf ((fp32x2_Math)x); }
		inline fp32x2 erfc(fp32x2 x) { return (fp32x2)erfc((fp32x2_Math)x); }
		inline fp32x2 lgamma(fp32x2 x) { return (fp32x2)lgamma((fp32x2_Math)x); }
		inline fp32x2 tgamma(fp32x2 x) { return (fp32x2)tgamma((fp32x2_Math)x); }

#endif /* DOUBLE_FLOAT32_HPP */