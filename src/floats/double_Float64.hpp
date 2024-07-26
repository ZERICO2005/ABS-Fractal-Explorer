/*
**	Author: zerico2005 (2023 - 2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef DOUBLE_FLOAT64_HPP
#define DOUBLE_FLOAT64_HPP

#include <cstdint>
#include <math.h>
#include <cmath>

typedef float fp32;
typedef double fp64;

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
 */
class Float64x2 {
public:
	fp64 hi;
	fp64 lo;
	
private:

	inline Float64x2 Dekker_Add(
		const Float64x2& x, const Float64x2& y
	) const {
		fp64 r_hi = x.hi + y.hi;
		fp64 r_lo = 0.0;
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

	inline Float64x2 Dekker_Sub(
		const Float64x2& x, const Float64x2& y
	) const {
		fp64 r_hi = x.hi - y.hi;
		fp64 r_lo = 0.0;
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
	
	inline Float64x2 Dekker_Split(const fp64& x) const {
		fp64 p = x * Dekker_Scale;
		Float64x2 r;
		r.hi = x - p + p;
		r.lo = x - r.hi;
		return r;
	}
	
	// static constexpr uint64_t Dekker_Split_Mask = ~((uint64_t)0x3FFFFFF);

	// inline Float64x2 Dekker_Split(const fp64& x) const {
	// 	Float64x2 r;
	// 	uint64_t temp = (*(uint64_t*)((void*)&x)) & Dekker_Split_Mask;
	// 	r.hi = (*(fp64*)((void*)&temp));
	// 	r.lo = x - r.hi;
	// 	return r;
	// }

	inline Float64x2 Dekker_Mul12(
		const fp64& x, const fp64& y
	) const {
		Float64x2 a = Dekker_Split(x);
		Float64x2 b = Dekker_Split(y);
		fp64 p = a.hi * b.hi;
		fp64 q = a.hi * b.lo + a.lo * b.hi;

		Float64x2 r;
		r.hi = p + q;
		r.lo = p - r.hi + q + a.lo * b.lo;
		return r;
	}

	inline Float64x2 Dekker_Mul(
		const Float64x2& x, const Float64x2& y
	) const {
		Float64x2 t = Dekker_Mul12(x.hi, y.hi);
		fp64 c = x.hi * y.lo + x.lo * y.hi + t.lo;

		Float64x2 r;
		r.hi = t.hi + c;
		r.lo = t.hi - r.hi + c;
		return r;
	}

	inline Float64x2 Dekker_Div(
		const Float64x2& x, const Float64x2& y
	) const {
		Float64x2 u;
		u.hi = x.hi / y.hi;
		Float64x2 t = Dekker_Mul12(u.hi, y.hi);
		fp64 l = (x.hi - t.hi - t.lo + x.lo - u.hi * y.lo) / y.hi;

		Float64x2 r;
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

	inline Float64x2 operator-() const {
		Float64x2 value = *this;
		value.hi = -value.hi;
		value.lo = -value.lo;
		return value;
	}

/* Increment/Decrement */

	inline Float64x2& operator++() {
		*this = Dekker_Add(*this, (Float64x2)1.0);
		return *this;
	}

	inline Float64x2& operator--() {
		*this = Dekker_Sub(*this, (Float64x2)1.0);
		return *this;
	}

	inline Float64x2 operator++(int) {
		Float64x2 temp = *this;
		*this = Dekker_Add(*this, (Float64x2)1.0);
		return temp;
	}

	inline Float64x2 operator--(int) {
		Float64x2 temp = *this;
		*this = Dekker_Sub(*this, (Float64x2)1.0);
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

	inline Float64x2() {
		this->hi = 0.0;
		this->lo = 0.0;
	}

	inline Float64x2(const fp32& value) {
		this->hi = (fp64)value;
		this->lo = 0.0;
	}
	inline Float64x2(const fp64& value) {
		this->hi = value;
		this->lo = 0.0;
	}

	template<typename fpX>
	inline Float64x2(const fpX& value) {
		this->hi = (fp64)value;
		this->lo = (fp64)(value - (fpX)this->hi);
	}

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

/* Math functions */
	/* Arithmetic */
	inline fp64x2 fmax(fp64x2 x, fp64x2 y) {
		return (x > y) ? x : y;
	}
	inline fp64x2 fmin(fp64x2 x, fp64x2 y) {
		return (x < y) ? x : y;
	}
	inline fp64x2 fabs(fp64x2 x) {
		return (x < (fp64x2)0.0) ? -x : x;
	}
	/* Tests */
	inline bool signbit(fp64x2 x) {
		return (x < (fp64x2)0.0) ? true : false;
	}

/* Math overloads */

		/* Arithmetic */
		// inline fp64x2 fmax(fp64x2 x, fp64x2 y) { return (fp64x2)fmax((fp64x2_Math)x, (fp64x2_Math)y); }
		// inline fp64x2 fmin(fp64x2 x, fp64x2 y) { return (fp64x2)fmin((fp64x2_Math)x, (fp64x2_Math)y); }
		// inline fp64x2 fabs(fp64x2 x) { return (fp64x2)fabs((fp64x2_Math)x); }
		inline fp64x2 fdim(fp64x2 x, fp64x2 y) { return (fp64x2)fdim((fp64x2_Math)x, (fp64x2_Math)y); }
		inline fp64x2 fma(fp64x2 x, fp64x2 y, fp64x2 z) { return (fp64x2)fma((fp64x2_Math)x, (fp64x2_Math)y, (fp64x2_Math)z); }
		inline fp64x2 copysign(fp64x2 x, fp64x2 y) { return (fp64x2)copysign((fp64x2_Math)x, (fp64x2_Math)y); }
		inline fp64x2 sqrt(fp64x2 x) { return (fp64x2)sqrt((fp64x2_Math)x); }
		inline fp64x2 cbrt(fp64x2 x) { return (fp64x2)cbrt((fp64x2_Math)x); }
		inline fp64x2 hypot(fp64x2 x, fp64x2 y) { return (fp64x2)hypot((fp64x2_Math)x, (fp64x2_Math)y); }
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
			fp64x2_Math p_sin_temp, p_cos_temp;
			sincos((fp64x2_Math)x, &p_sin_temp, &p_cos_temp);
			*p_sin = (fp64x2)p_sin_temp;
			*p_cos = (fp64x2)p_cos_temp;
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
		inline fp64x2 trunc(fp64x2 x) { return (fp64x2)trunc((fp64x2_Math)x); }
		inline fp64x2 floor(fp64x2 x) { return (fp64x2)floor((fp64x2_Math)x); }
		inline fp64x2 ceil (fp64x2 x) { return (fp64x2)ceil ((fp64x2_Math)x); }
		inline fp64x2 rint (fp64x2 x) { return (fp64x2)rint ((fp64x2_Math)x); }
		inline fp64x2 round(fp64x2 x) { return (fp64x2)round((fp64x2_Math)x); }
		inline long lrint (fp64x2 x) { return lrint ((fp64x2_Math)x); }
		inline long lround(fp64x2 x) { return lround((fp64x2_Math)x); }
		inline long long llrint (fp64x2 x) { return llrint ((fp64x2_Math)x); }
		inline long long llround(fp64x2 x) { return llround((fp64x2_Math)x); }
		/* Integer and Remainder */
		inline fp64x2 fmod(fp64x2 x, fp64x2 y) { return (fp64x2)fmod((fp64x2_Math)x, (fp64x2_Math)y); }
		inline fp64x2 modf(fp64x2 x, fp64x2* y) {
			fp64x2_Math y_temp;
			fp64x2 result = modf((fp64x2_Math)x, &y_temp);
			*y = (fp64x2)y_temp;
			return result;
		}
		inline fp64x2 nearbyint(fp64x2 x) { return (fp64x2)nearbyint((fp64x2_Math)x); }
		inline fp64x2 nextafter(fp64x2 x, fp64x2 y) { return (fp64x2)nextafter((fp64x2_Math)x, (fp64x2_Math)y); }
		inline fp64x2 remainder(fp64x2 x, fp64x2 y) { return (fp64x2)remainder((fp64x2_Math)x, (fp64x2_Math)y); }
		inline fp64x2 remquo(fp64x2 x, fp64x2 y, int* quo) { return (fp64x2)remquo((fp64x2_Math)x, (fp64x2_Math)y, quo); }
		/* Float Exponents */
		inline int ilogb(fp64x2 x) { return ilogb((fp64x2_Math)x); }
		inline fp64x2 frexp  (fp64x2 x, int* exp) { return (fp64x2)frexp  ((fp64x2_Math)x, exp); }
		inline fp64x2 ldexp  (fp64x2 x, int  exp) { return (fp64x2)ldexp  ((fp64x2_Math)x, exp); }
		inline fp64x2 scalbn (fp64x2 x, int  exp) { return (fp64x2)scalbn ((fp64x2_Math)x, exp); }
		inline fp64x2 scalbln(fp64x2 x, long exp) { return (fp64x2)scalbln((fp64x2_Math)x, exp); }
		/* Tests */
		// inline bool signbit(fp64x2 x) { return (signbit((fp64x2_Math)x) != 0) ? true : false; }
		inline bool isfinite(fp64x2 x) { return (isfinite((fp64x2_Math)x) != 0) ? true : false; }
		inline bool isinf(fp64x2 x) { return (isinf((fp64x2_Math)x) != 0) ? true : false; }
		inline bool isnan(fp64x2 x) { return (isnan((fp64x2_Math)x) != 0) ? true : false; }
		/* Transcendental Functions */
		inline fp64x2 erf (fp64x2 x) { return (fp64x2)erf ((fp64x2_Math)x); }
		inline fp64x2 erfc(fp64x2 x) { return (fp64x2)erfc((fp64x2_Math)x); }
		inline fp64x2 lgamma(fp64x2 x) { return (fp64x2)lgamma((fp64x2_Math)x); }
		inline fp64x2 tgamma(fp64x2 x) { return (fp64x2)tgamma((fp64x2_Math)x); }

#endif /* DOUBLE_FLOAT64_HPP */