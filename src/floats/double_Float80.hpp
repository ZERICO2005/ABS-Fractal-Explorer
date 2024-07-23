/*
**	Author: zerico2005 (2023 - 2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef DOUBLE_FLOAT80_HPP
#define DOUBLE_FLOAT80_HPP

#include <cstdint>
#include <cmath>

typedef float fp32;
typedef double fp64;

#include "Float80.hpp"

#if defined(Enable_Float128)
	#include "Float128.hpp"
	typedef fp128 fp80x2_Math;
#else
	typedef fp80 fp80x2_Math;
#endif


/**
 * @brief Double-Float80 Dekker Float implementation.
 * Source: Creel "Double it Like Dekker" on YouTube.
 */
class Float80x2 {
public:
	fp80 hi;
	fp80 lo;
	
private:

	inline Float80x2 Dekker_Add(
		const Float80x2& x, const Float80x2& y
	) const {
		fp80 r_hi = x.hi + y.hi;
		fp80 r_lo = 0.0;
		if (fabs(x.hi) > fabs(y.hi)) {
			r_lo = x.hi - r_hi + y.hi + y.lo + x.lo;
		} else {
			r_lo = y.hi - r_hi + x.hi + x.lo + y.lo;
		}

		Float80x2 c;
		c.hi = r_hi + r_lo;
		c.lo = r_hi - c.hi + r_lo;
		return c;
	}

	inline Float80x2 Dekker_Sub(
		const Float80x2& x, const Float80x2& y
	) const {
		fp80 r_hi = x.hi - y.hi;
		fp80 r_lo = 0.0;
		if (fabs(x.hi) > fabs(y.hi)) {
			r_lo = x.hi - r_hi - y.hi - y.lo + x.lo;
		} else {
			r_lo = -y.hi - r_hi + x.hi + x.lo - y.lo;
		}

		Float80x2 c;
		c.hi = r_hi + r_lo;
		c.lo = r_hi - c.hi + r_lo;
		return c;
	}

	static constexpr fp80 Dekker_Scale = 4294967297.0; // (2^ceil(64 / 2) + 1)
	
	inline Float80x2 Dekker_Split(const fp80& x) const {
		fp80 p = x * Dekker_Scale;
		Float80x2 r;
		r.hi = x - p + p;
		r.lo = x - r.hi;
		return r;
	}
	
	// static constexpr uint64_t Dekker_Split_Mask = ~((uint64_t)0xFFFFFFFF);

	// inline Float80x2 Dekker_Split(const fp80& x) const {
	// 	Float80x2 r;
	// 	uint64_t temp = (*(uint64_t*)((void*)&x)) & Dekker_Split_Mask;
	// 	r.hi = (*(fp80*)((void*)&temp));
	// 	r.lo = x - r.hi;
	// 	return r;
	// }

	inline Float80x2 Dekker_Mul12(
		const fp80& x, const fp80& y
	) const {
		Float80x2 a = Dekker_Split(x);
		Float80x2 b = Dekker_Split(y);
		fp80 p = a.hi * b.hi;
		fp80 q = a.hi * b.lo + a.lo * b.hi;

		Float80x2 r;
		r.hi = p + q;
		r.lo = p - r.hi + q + a.lo * b.lo;
		return r;
	}

	inline Float80x2 Dekker_Mul(
		const Float80x2& x, const Float80x2& y
	) const {
		Float80x2 t = Dekker_Mul12(x.hi, y.hi);
		fp80 c = x.hi * y.lo + x.lo * y.hi + t.lo;

		Float80x2 r;
		r.hi = t.hi + c;
		r.lo = t.hi - r.hi + c;
		return r;
	}

	inline Float80x2 Dekker_Div(
		const Float80x2& x, const Float80x2& y
	) const {
		Float80x2 u;
		u.hi = x.hi / y.hi;
		Float80x2 t = Dekker_Mul12(u.hi, y.hi);
		fp80 l = (x.hi - t.hi - t.lo + x.lo - u.hi * y.lo) / y.hi;

		Float80x2 r;
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

	inline Float80x2 operator+(const Float80x2 &value) const {
		return Dekker_Add(*this, value);
	}

	inline Float80x2 operator-(const Float80x2 &value) const {
		return Dekker_Sub(*this, value);
	}

	inline Float80x2 operator*(const Float80x2 &value) const {
		return Dekker_Mul(*this, value);
	}

	inline Float80x2 operator/(const Float80x2 &value) const {
		return Dekker_Div(*this, value);
	}

	inline Float80x2 operator-() const {
		Float80x2 value = *this;
		value.hi = -value.hi;
		value.lo = -value.lo;
		return value;
	}

/* Increment/Decrement */

	inline Float80x2& operator++() {
		*this = Dekker_Add(*this, (Float80x2)1.0);
		return *this;
	}

	inline Float80x2& operator--() {
		*this = Dekker_Sub(*this, (Float80x2)1.0);
		return *this;
	}

	inline Float80x2 operator++(int) {
		Float80x2 temp = *this;
		*this = Dekker_Add(*this, (Float80x2)1.0);
		return temp;
	}

	inline Float80x2 operator--(int) {
		Float80x2 temp = *this;
		*this = Dekker_Sub(*this, (Float80x2)1.0);
		return temp;
	}

/* Compound Assignment */

	inline Float80x2& operator+=(const Float80x2 &value) {
		*this = Dekker_Add(*this, value);
		return *this;
	}

	inline Float80x2& operator-=(const Float80x2 &value) {
		*this = Dekker_Sub(*this, value);
		return *this;
	}

	inline Float80x2& operator*=(const Float80x2 &value) {
		*this = Dekker_Mul(*this, value);
		return *this;
	}

	inline Float80x2& operator/=(const Float80x2 &value) {
		*this = Dekker_Div(*this, value);
		return *this;
	}

/* Comparison */

	inline bool operator<(const Float80x2 &value) const {
		if (this->hi == value.hi) {
			return (this->lo < value.lo);
		}
		return (this->hi < value.hi);
	}

	inline bool operator<=(const Float80x2 &value) const {
		if (this->hi == value.hi) {
			return (this->lo <= value.lo);
		}
		return (this->hi <= value.hi);
	}

	inline bool operator>(const Float80x2 &value) const {
		if (this->hi == value.hi) {
			return (this->lo > value.lo);
		}
		return (this->hi > value.hi);
	}

	inline bool operator>=(const Float80x2 &value) const {
		if (this->hi == value.hi) {
			return (this->lo >= value.lo);
		}
		return (this->hi >= value.hi);
	}

/* Constructors */

	inline Float80x2() {
		this->hi = 0.0;
		this->lo = 0.0;
	}

	inline Float80x2(const fp32& value) {
		this->hi = (fp32)value;
		this->lo = 0.0;
	}
	inline Float80x2(const fp64& value) {
		this->hi = (fp64)value;
		this->lo = 0.0;
	}
	inline Float80x2(const fp80& value) {
		this->hi = value;
		this->lo = 0.0;
	}

	template<typename fpX>
	inline Float80x2(const fpX& value) {
		this->hi = (fp80)value;
		this->lo = (fp80)(value - (fpX)this->hi);
	}

/* Casts */

	constexpr inline operator fp32() const {
		return (fp32)this->hi;
	}
	constexpr inline operator fp64() const {
		return (fp64)this->hi;
	}
	constexpr inline operator fp80() const {
		return this->hi;
	}

	template<typename fpX>
	constexpr inline operator fpX() const {
		return (fpX)this->hi + (fpX)this->lo;
	}

};

typedef Float80x2 fp80x2;

/* Math overloads */
	/* Trigonometry */
	inline fp80x2  sin (fp80x2 x) { return (fp80x2) sin ((fp80x2_Math)x); }
	inline fp80x2  cos (fp80x2 x) { return (fp80x2) cos ((fp80x2_Math)x); }
	inline fp80x2  tan (fp80x2 x) { return (fp80x2) tan ((fp80x2_Math)x); }
	inline fp80x2 asin (fp80x2 x) { return (fp80x2)asin ((fp80x2_Math)x); }
	inline fp80x2 acos (fp80x2 x) { return (fp80x2)acos ((fp80x2_Math)x); }
	inline fp80x2 atan (fp80x2 x) { return (fp80x2)atan ((fp80x2_Math)x); }
	inline fp80x2  sinh(fp80x2 x) { return (fp80x2) sinh((fp80x2_Math)x); }
	inline fp80x2  cosh(fp80x2 x) { return (fp80x2) cosh((fp80x2_Math)x); }
	inline fp80x2  tanh(fp80x2 x) { return (fp80x2) tanh((fp80x2_Math)x); }
	inline fp80x2 asinh(fp80x2 x) { return (fp80x2)asinh((fp80x2_Math)x); }
	inline fp80x2 acosh(fp80x2 x) { return (fp80x2)acosh((fp80x2_Math)x); }
	inline fp80x2 atanh(fp80x2 x) { return (fp80x2)atanh((fp80x2_Math)x); }
	inline fp80x2 atan2(fp80x2 y, fp80x2 x) { return (fp80x2)atan2((fp80x2_Math)y, (fp80x2_Math)x); }
	inline fp80x2 hypot(fp80x2 x, fp80x2 y) { return (fp80x2)hypot((fp80x2_Math)x, (fp80x2_Math)y); }
	/* Logarithms and Exponents */
	inline fp80x2 log  (fp80x2 x) { return (fp80x2)log  ((fp80x2_Math)x); }
	inline fp80x2 log1p(fp80x2 x) { return (fp80x2)log1p((fp80x2_Math)x); }
	inline fp80x2 log2 (fp80x2 x) { return (fp80x2)log2 ((fp80x2_Math)x); }
	inline fp80x2 log10(fp80x2 x) { return (fp80x2)log10((fp80x2_Math)x); }
	inline fp80x2 exp  (fp80x2 x) { return (fp80x2)exp  ((fp80x2_Math)x); }
	inline fp80x2 expm1(fp80x2 x) { return (fp80x2)expm1((fp80x2_Math)x); }
	inline fp80x2 exp2 (fp80x2 x) { return (fp80x2)exp2 ((fp80x2_Math)x); }
	inline fp80x2 pow(fp80x2 x, fp80x2 y) { return (fp80x2)pow((fp80x2_Math)x, (fp80x2_Math)y); }
	/* Rounding */
	inline fp80x2 trunc(fp80x2 x) { return (fp80x2)trunc((fp80x2_Math)x); }
	inline fp80x2 floor(fp80x2 x) { return (fp80x2)floor((fp80x2_Math)x); }
	inline fp80x2 ceil (fp80x2 x) { return (fp80x2)ceil ((fp80x2_Math)x); }
	inline fp80x2 round(fp80x2 x) { return (fp80x2)round((fp80x2_Math)x); }
	/* Others */
	inline fp80x2 fmax(fp80x2 x, fp80x2 y) { return (fp80x2)fmax((fp80x2_Math)x, (fp80x2_Math)y); }
	inline fp80x2 fmin(fp80x2 x, fp80x2 y) { return (fp80x2)fmin((fp80x2_Math)x, (fp80x2_Math)y); }
	inline fp80x2 fmod(fp80x2 x, fp80x2 y) { return (fp80x2)fmod((fp80x2_Math)x, (fp80x2_Math)y); }
	// inline fp80x2 fabs(fp80x2 x) { return (fp80x2)fabsq((fp80x2_Math)x); }
	inline fp80x2 fabs(fp80x2 x) { return (x < (fp80x2)0.0) ? -x : x; }
	inline fp80x2 copysign(fp80x2 x, fp80x2 y) { return (fp80x2)copysign((fp80x2_Math)x, (fp80x2_Math)y); }
	inline bool signbit(fp80x2 x) { return (signbit((fp80x2_Math)x) != 0) ? true : false; }
	inline fp80x2 sqrt(fp80x2 x) { return (fp80x2)sqrt((fp80x2_Math)x); }
	inline fp80x2 cbrt(fp80x2 x) { return (fp80x2)cbrt((fp80x2_Math)x); }

#endif /* DOUBLE_FLOAT64_HPP */