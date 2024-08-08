/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef LIBQD_FUNC_HPP
#define LIBQD_FUNC_HPP

#ifdef Enable_LIBQD

/**
 * @brief libqd doesn't define all the cmath/math.h functions
 * so they are defined/implemented here.
 * 
 * @note copy the commented out "Constructors" and "Casts" code into
 * struct qd_real in <qd/qd_real.h> to allow it to work with other types.
*/

#include <qd/qd_real.h>

/* Constructors */
/*
	inline dd_real::dd_real(const float& a) {
		dd_real((double)a);
	}

	template<typename fpX>
	inline dd_real::dd_real(const fpX& a) {
		x[0] = (double)a;
		x[1] = (double)(a - (fpX)x[0]);
		x[2] = (double)((a - (fpX)x[0]) - (fpX)x[1]);
		x[3] = (double)(((a - (fpX)x[0]) - (fpX)x[1]) - (fpX)x[2]);
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
		// Add from smallest to largest
		return ((fpX)x[0] + ((fpX)x[1] + ((fpX)x[2] + (fpX)x[3])));
	}
*/

/* Functions */

	inline qd_real fdim(const qd_real& a, const qd_real& b) {
		return (a > b) ? (a - b) : static_cast<qd_real>(0.0);
	}
	/** (a * b) + c */
	inline qd_real fma(const qd_real& a, const qd_real& b, const qd_real& c) {
		return (a * b) + c;
	}
	inline qd_real copysign(const qd_real& a, const qd_real& b) {
		return (
			a.is_negative() != b.is_negative()
		) ? -a : a;
	}
	/** a.is_negative() */
	inline bool signbit(const qd_real& a) {
		return a.is_negative();
	}
	/** nroot(a, 3) */
	inline qd_real cbrt(const qd_real& a) {
		return nroot(a, 3);
	}
	/** sqrt(sqr(a) + sqr(b)) */
	inline qd_real hypot(const qd_real& a, const qd_real& b) {
		return sqrt(sqr(a) + sqr(b));
	}
	/** sqrt(sqr(a) + sqr(b) + sqr(c)) */
	inline qd_real hypot(const qd_real& a, const qd_real& b, const qd_real& c) {
		return sqrt(sqr(a) + sqr(b) + sqr(c));
	}

/* Rounding */

	/** aint() */
	inline qd_real trunc(const qd_real& a) {
		return aint(a);
	}
	/** nint() */
	inline qd_real nearbyint(const qd_real& a) {
		return nint(a);
	}
	inline qd_real round(const qd_real& a) {
		qd_real int_part = trunc(a);
		qd_real frac_part = a - int_part;
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
	inline long lround(const qd_real& a) {
		return (long)round(a);
	}
	inline long long llround(const qd_real& a) {
		return (long long)round(a);
	}

/* Logarithms and Exponents */

	inline qd_real exp2(const qd_real &a) {
		return exp(a * qd_real::_log2);
	}
	inline qd_real exp10(const qd_real &a) {
		return exp(a * qd_real::_log10);
	}
	inline qd_real expm1(const qd_real &a) {
		return exp(a) - static_cast<double>(1.0);
	}
	inline qd_real log2(const qd_real &a) {
		return log(a) / qd_real::_log2;
	}
	inline qd_real log1p(const qd_real &a) {
		return log(a + static_cast<double>(1.0));
	}

/* Comparison */

	inline bool isgreater(const qd_real& a, const qd_real& b) {
		return (a > b);
	}
	inline bool isgreaterequal(const qd_real& a, const qd_real& b) {
		return (a >= b);
	}
	inline bool isless(const qd_real& a, const qd_real& b) {
		return (a < b);
	}
	inline bool islessequal(const qd_real& a, const qd_real& b) {
		return (a <= b);
	}
	inline bool islessgreater(const qd_real& a, const qd_real& b) {
		return (a < b || a > b);
	}

#endif

#endif /* LIBQD_FUNC_HPP */
