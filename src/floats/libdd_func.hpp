/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef LIBDD_FUNC_HPP
#define LIBDD_FUNC_HPP

#ifdef Enable_LIBQD

/**
 * @brief libqd doesn't define all the cmath/math.h functions
 * so they are defined/implemented here.
 * 
 * @note copy the commented out "Constructors" and "Casts" code into
 * struct dd_real in <qd/dd_real.h> to allow it to work with other types.
*/

#include <qd/dd_real.h>

/* Constructors */
/*
	inline dd_real::dd_real(const float& a) {
		dd_real((double)a);
	}

	template<typename fpX>
	inline dd_real::dd_real(const fpX& a) {
		x[0] = (double)a;
		x[1] = (double)(a - (fpX)x[0]);
	}
*/

/* Casts */
/*
	constexpr inline operator dd_real::float() const {
		return (float)x[0];
	}

	constexpr inline operator dd_real::double() const {
		return x[0];
	}

	template<typename fpX>
	constexpr inline operator dd_real::fpX() const {
		return (fpX)x[0] + (fpX)x[1];
	}
*/

/* Functions */

	inline dd_real fdim(const dd_real& a, const dd_real& b) {
		return (a > b) ? (a - b) : static_cast<dd_real>(0.0);
	}
	/** (a * b) + c */
	inline dd_real fma(const dd_real& a, const dd_real& b, const dd_real& c) {
		return (a * b) + c;
	}
	inline dd_real copysign(const dd_real& a, const dd_real& b) {
		return (
			a.is_negative() != b.is_negative()
		) ? -a : a;
	}
	/** a.is_negative() */
	inline bool signbit(const dd_real& a) {
		return a.is_negative();
	}
	/** nroot(a, 3) */
	inline dd_real cbrt(const dd_real& a) {
		return nroot(a, 3);
	}
	/** sqrt(sqr(a) + sqr(b)) */
	inline dd_real hypot(const dd_real& a, const dd_real& b) {
		return sqrt(sqr(a) + sqr(b));
	}
	/** sqrt(sqr(a) + sqr(b) + sqr(c)) */
	inline dd_real hypot(const dd_real& a, const dd_real& b, const dd_real& c) {
		return sqrt(sqr(a) + sqr(b) + sqr(c));
	}

/* Rounding */

	/** aint() */
	inline dd_real trunc(const dd_real& a) {
		return aint(a);
	}
	/** nint() */
	inline dd_real nearbyint(const dd_real& a) {
		return nint(a);
	}
	inline dd_real round(const dd_real& a) {
		dd_real int_part = trunc(a);
		dd_real frac_part = a - int_part;
		if (a.is_positive()) {
			if (frac_part >= static_cast<double>(0.5)) {
				return int_part + static_cast<double>(1.0);
			}
			return int_part;
		}
		if (frac_part <= static_cast<double>(-0.5)) {
			return int_part - static_cast<double>(1.0);
		}
		return int_part;
	}
	inline long lround(const dd_real& a) {
		return (long)round(a);
	}
	inline long long llround(const dd_real& a) {
		return (long long)round(a);
	}

/* Logarithms and Exponents */

	inline dd_real exp2(const dd_real &a) {
		return exp(a * dd_real::_log2);
	}
	inline dd_real exp10(const dd_real &a) {
		return exp(a * dd_real::_log10);
	}
	inline dd_real expm1(const dd_real &a) {
		return exp(a) - static_cast<double>(1.0);
	}
	inline dd_real log2(const dd_real &a) {
		return log(a) / dd_real::_log2;
	}
	inline dd_real log1p(const dd_real &a) {
		return log(a + static_cast<double>(1.0));
	}

/* Comparison */

	inline bool isgreater(const dd_real& a, const dd_real& b) {
		return (a > b);
	}
	inline bool isgreaterequal(const dd_real& a, const dd_real& b) {
		return (a >= b);
	}
	inline bool isless(const dd_real& a, const dd_real& b) {
		return (a < b);
	}
	inline bool islessequal(const dd_real& a, const dd_real& b) {
		return (a <= b);
	}
	inline bool islessgreater(const dd_real& a, const dd_real& b) {
		return (a < b || a > b);
	}

#endif

#endif /* LIBDD_FUNC_HPP */
