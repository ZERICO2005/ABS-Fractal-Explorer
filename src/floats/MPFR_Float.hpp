/*
**	Author: zerico2005 (2023 - 2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#ifndef MPFR_FLOAT_HPP
#define MPFR_FLOAT_HPP

// Disabled for now. The overhead from operator overloading made it impractical for rendering fractals.
#if defined(Enable_FloatMPFR) && false

#include <cstdint>

#include <cmath>

typedef float fp32;
typedef double fp64;
#ifdef Enable_Float128
	#include "Float80.hpp"
#endif
#ifdef Enable_Float128
	#include "Float128.hpp"
	#define MPFR_WANT_FLOAT128
#endif
#include <mpfr.h>

#define PRImpfr "R"

constexpr mpfr_prec_t MPFR_PRECISION = 256;

// template<mpfr_prec_t fpX>
class FloatMPFR {
	public:
	mpfr_t value;
	
	// private:
	public:

	inline void copy_prec(const FloatMPFR& arg) {
		mpfr_set_prec(value, mpfr_get_prec(arg.value));
	}

	inline void copy_value(const FloatMPFR& arg) {
		mpfr_set(value, arg.value, MPFR_RNDZ);
	}

	inline void copy_mpfr(const FloatMPFR& arg) {
		mpfr_set_prec(value, mpfr_get_prec(arg.value));
		mpfr_set(value, arg.value, MPFR_RNDZ);
	}

	public:

/* Query */
	inline mpfr_prec_t get_Precision() const {
		return mpfr_get_prec(value);
	}

/* Arithmetic */

	inline FloatMPFR operator+(const FloatMPFR& arg) const {
		FloatMPFR result; result.copy_prec(*this);
		mpfr_add(result.value, value, arg.value, MPFR_RNDZ);
		return result;
	}

	inline FloatMPFR operator-(const FloatMPFR& arg) const {
		FloatMPFR result; result.copy_prec(*this);;
		mpfr_sub(result.value, value, arg.value, MPFR_RNDZ);
		return result;
	}

	inline FloatMPFR operator*(const FloatMPFR& arg) const {
		FloatMPFR result; result.copy_prec(*this);
		mpfr_mul(result.value, value, arg.value, MPFR_RNDZ);
		return result;
	}

	inline FloatMPFR operator/(const FloatMPFR& arg) const {
		FloatMPFR result; result.copy_prec(*this);
		mpfr_div(result.value, value, arg.value, MPFR_RNDZ);
		return result;
	}

	inline FloatMPFR operator-() const {
		FloatMPFR result; result.copy_prec(*this);
		mpfr_neg(result.value, this->value, MPFR_RNDZ);
		return result;
	}

/* Increment/Decrement */

	inline FloatMPFR& operator++() {
		mpfr_add_ui(value, value, (unsigned long)1, MPFR_RNDZ);
		return *this;
	}

	inline FloatMPFR& operator--() {
		mpfr_sub_ui(value, value, (unsigned long)1, MPFR_RNDZ);
		return *this;
	}

	inline FloatMPFR operator++(int) {
		FloatMPFR temp; temp.copy_mpfr(*this);
		mpfr_add_ui(value, value, (unsigned long)1, MPFR_RNDZ);
		return temp;
	}

	inline FloatMPFR operator--(int) {
		FloatMPFR temp; temp.copy_mpfr(*this);
		mpfr_sub_ui(value, value, (unsigned long)1, MPFR_RNDZ);
		return temp;
	}

/* Compound Assignment */

	inline FloatMPFR& operator+=(const FloatMPFR& arg) {
		mpfr_add(value, value, arg.value, MPFR_RNDZ);
		return *this;
	}

	inline FloatMPFR& operator-=(const FloatMPFR& arg) {
		mpfr_sub(value, value, arg.value, MPFR_RNDZ);
		return *this;
	}

	inline FloatMPFR& operator*=(const FloatMPFR& arg) {
		mpfr_mul(value, value, arg.value, MPFR_RNDZ);
		return *this;
	}

	inline FloatMPFR& operator/=(const FloatMPFR& arg) {
		mpfr_div(value, value, arg.value, MPFR_RNDZ);
		return *this;
	}

/* Comparison */

	inline bool operator==(const FloatMPFR& arg) const {
		return (mpfr_equal_p(value, arg.value) != 0) ? true : false;
	}

	inline bool operator!=(const FloatMPFR& arg) const {
		return (mpfr_equal_p(value, arg.value) == 0) ? true : false;
	}

	inline bool operator<(const FloatMPFR& arg) const {
		return (mpfr_less_p(value, arg.value) != 0) ? true : false;
	}

	inline bool operator<=(const FloatMPFR& arg) const {
		return (mpfr_lessequal_p(value, arg.value) != 0) ? true : false;
	}

	inline bool operator>(const FloatMPFR& arg) const {
		return (mpfr_greater_p(value, arg.value) != 0) ? true : false;
	}

	inline bool operator>=(const FloatMPFR& arg) const {
		return (mpfr_greaterequal_p(value, arg.value) != 0) ? true : false;
	}

/* Constructors */
	FloatMPFR() {
		mpfr_init2(value, MPFR_PRECISION);
	}
	// FloatMPFR() {
	// 	mpfr_init2(value, fpX);
	// }
	// FloatMPFR(mpfr_prec_t precision) {
	// 	mpfr_init2(value, precision);
	// }

	~FloatMPFR() {
		mpfr_clear(value);
	}

	FloatMPFR(const FloatMPFR& arg) {
		mpfr_init2(value, mpfr_get_prec(arg.value));
		mpfr_set(value, arg.value, MPFR_RNDZ);
	}

	inline FloatMPFR& operator=(const FloatMPFR& arg) {
		mpfr_set(this->value, arg.value, MPFR_RNDZ);
		return *this;
	}

	inline FloatMPFR(const fp32& arg) {
		mpfr_init2(value, MPFR_PRECISION);
		mpfr_set_flt(value, arg, MPFR_RNDZ);
	}

	inline FloatMPFR(const fp64& arg) {
		mpfr_init2(value, MPFR_PRECISION);
		mpfr_set_d(value, arg, MPFR_RNDZ);
	}
	#ifdef Enable_Float80
		inline FloatMPFR(const fp80& arg) {
			mpfr_init2(value, MPFR_PRECISION);
			mpfr_set_ld(value, arg, MPFR_RNDZ);
		}
	#endif
	#ifdef Enable_Float128
		inline FloatMPFR(const fp128& arg) {
			mpfr_init2(value, MPFR_PRECISION);
			mpfr_set_float128(value, arg, MPFR_RNDZ);
		}
	#endif

	inline FloatMPFR(const int& arg) {
		mpfr_init2(value, MPFR_PRECISION);
		mpfr_set_si(value, (long)arg, MPFR_RNDZ);
	}
	inline FloatMPFR(const unsigned int& arg) {
		mpfr_init2(value, MPFR_PRECISION);
		mpfr_set_ui(value, (unsigned long)arg, MPFR_RNDZ);
	}

/* Assignment */

	// inline FloatMPFR& operator=(const FloatMPFR& arg) {
	// 	if (this == &arg) {
	// 		return *this;
	// 	}
	// 	mpfr_set(this->value, arg.value, MPFR_RNDZ);
	// 	return *this;
	// }
	// inline FloatMPFR& operator=(const FloatMPFR&& arg) noexcept {
	// 	if (this == &arg) {
	// 		return *this;
	// 	}
	// 	mpfr_set(value, arg.value, MPFR_RNDZ);
	// 	return *this;
	// }

	// inline FloatMPFR& operator=(const fp32& arg) {
	// 	mpfr_set_flt(value, arg, MPFR_RNDZ);
	// 	return *this;
	// }
	// inline FloatMPFR& operator=(const fp64& arg) {
	// 	mpfr_set_d(value, arg, MPFR_RNDZ);
	// 	return *this;
	// }
	// #ifdef Enable_Float80
	// 	inline FloatMPFR& operator=(const fp80& arg) {
	// 		mpfr_set_ld(value, arg, MPFR_RNDZ);
	// 		return *this;
	// 	}
	// #endif
	// #ifdef Enable_Float128
	// 	inline FloatMPFR& operator=(const fp128& arg) {
	// 		mpfr_set_float128(value, arg, MPFR_RNDZ);
	// 		return *this;
	// 	}
	// #endif

	// inline FloatMPFR& operator=(const int& arg) {
	// 	mpfr_set_si(value, (int long)arg, MPFR_RNDZ);
	// 	return *this;
	// }
	// inline FloatMPFR& operator=(const unsigned int& arg) {
	// 	mpfr_set_ui(value, (unsigned long)arg, MPFR_RNDZ);
	// 	return *this;
	// }

/* Casts */

	inline operator fp32() const {
		return mpfr_get_flt(value, MPFR_RNDZ);
	}
	inline operator fp64() const {
		return mpfr_get_d(value, MPFR_RNDZ);
	}
	#ifdef Enable_Float80
		inline operator fp80() const {
			return mpfr_get_ld(value, MPFR_RNDZ);
		}
	#endif
	#ifdef Enable_Float128
		inline operator fp128() const {
			return mpfr_get_float128(value, MPFR_RNDZ);
		}
	#endif

	inline operator int() const {
		return (int)mpfr_get_si(value, MPFR_RNDZ);
	}
	inline operator unsigned int() const {
		return (unsigned int)mpfr_get_ui(value, MPFR_RNDZ);
	}

};

typedef FloatMPFR fpMPFR;

inline int stringTo_FloatMPFR(mpfr_t rop, const char* nPtr, char** endPtr = nullptr) {
	return mpfr_strtofr(rop, nPtr, endPtr, 10, MPFR_RNDZ);
}

//------------------------------------------------------------------------------
// Arithmetic Functions
//------------------------------------------------------------------------------
	
	/** mpfr_sqrt */
	inline FloatMPFR sqrt(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_sqrt(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_cbrt */
	inline FloatMPFR cbrt(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_cbrt(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_abs */
	inline FloatMPFR fabs(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_abs(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** 
	 *  mpfr_dim
	 *  @note Uses precision from the first argument `x`
	 */
	inline FloatMPFR fdim(FloatMPFR x, FloatMPFR y) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_dim(result.value, x.value, y.value, MPFR_RNDZ);
		return result;
	}
	/** 
	 *  mpfr_fma
	 *  @note Uses precision from the first argument `x`
	 */
	inline FloatMPFR fma(FloatMPFR x, FloatMPFR y, FloatMPFR z) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_fma(result.value, x.value, y.value, z.value, MPFR_RNDZ);
		return result;
	}
	/** 
	 *  mpfr_hypot
	 *  @note Uses precision from the first argument `x`
	 */
	inline FloatMPFR hypot(FloatMPFR x, FloatMPFR y) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_hypot(result.value, x.value, y.value, MPFR_RNDZ);
		return result;
	}

//------------------------------------------------------------------------------
// Comparison Functions
//------------------------------------------------------------------------------

	/** mpfr_nan_p */
	inline bool isnan(FloatMPFR x) {
		return (mpfr_nan_p(x.value) != 0) ? true : false;
	}
	/** mpfr_inf_p */
	inline bool isinf(FloatMPFR x) {
		return (mpfr_inf_p(x.value) != 0) ? true : false;
	}

	/** mpfr_greater_p */
	inline bool isgreater(FloatMPFR x, FloatMPFR y) {
		return (mpfr_greater_p(x.value, y.value) != 0) ? true : false;
	}
	/** mpfr_greaterequal_p */
	inline bool isgreaterequal(FloatMPFR x, FloatMPFR y) {
		return (mpfr_greaterequal_p(x.value, y.value) != 0) ? true : false;
	}
	/** mpfr_less_p */
	inline bool isless(FloatMPFR x, FloatMPFR y) {
		return (mpfr_less_p(x.value, y.value) != 0) ? true : false;
	}
	/** mpfr_lessequal_p */
	inline bool islessequal(FloatMPFR x, FloatMPFR y) {
		return (mpfr_lessequal_p(x.value, y.value) != 0) ? true : false;
	}
	/** mpfr_lessgreater_p */
	inline bool islessgreater(FloatMPFR x, FloatMPFR y) {
		return (mpfr_lessgreater_p(x.value, y.value) != 0) ? true : false;
	}
	/** mpfr_unordered_p */
	inline bool isunordered(FloatMPFR x, FloatMPFR y) {
		return (mpfr_unordered_p(x.value, y.value) != 0) ? true : false;
	}

//------------------------------------------------------------------------------
// Transcendental Functions
//------------------------------------------------------------------------------

/* Logarithms */
	/** mpfr_log */
	inline FloatMPFR log(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_log(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_log2 */
	inline FloatMPFR log2(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_log2(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_log10 */
	inline FloatMPFR log10(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_log10(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_log1p */
	inline FloatMPFR log1p(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_log1p(result.value, x.value, MPFR_RNDZ);
		return result;
	}

/* Exponents */
	/** mpfr_exp */
	inline FloatMPFR exp(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_exp(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_exp2 */
	inline FloatMPFR exp2(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_exp2(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_expm1 */
	inline FloatMPFR expm1(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_expm1(result.value, x.value, MPFR_RNDZ);
		return result;
	}

	/** 
	*  mpfr_pow
	*  @note Uses precision from the first argument `x`
	*/
	inline FloatMPFR pow(FloatMPFR x, FloatMPFR y) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_pow(result.value, x.value, y.value, MPFR_RNDZ);
		return result;
	}

/* Trigonometry */

	/** mpfr_sin */
	inline FloatMPFR sin(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_sin(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_cos */
	inline FloatMPFR cos(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_cos(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_tan */
	inline FloatMPFR tan(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_tan(result.value, x.value, MPFR_RNDZ);
		return result;
	}

	/** mpfr_asin */
	inline FloatMPFR asin(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_asin(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_acos */
	inline FloatMPFR acos(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_acos(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_atan */
	inline FloatMPFR atan(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_atan(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	
	/** mpfr_sinh */
	inline FloatMPFR sinh(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_sinh(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_cosh */
	inline FloatMPFR cosh(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_cosh(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_tanh */
	inline FloatMPFR tanh(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_tanh(result.value, x.value, MPFR_RNDZ);
		return result;
	}

	/** mpfr_asinh */
	inline FloatMPFR asinh(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_asinh(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_acosh */
	inline FloatMPFR acosh(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_acosh(result.value, x.value, MPFR_RNDZ);
		return result;
	}
	/** mpfr_atanh */
	inline FloatMPFR atanh(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_atanh(result.value, x.value, MPFR_RNDZ);
		return result;
	}

	/** 
	*  mpfr_atan2
	*  @note Uses precision from the first argument `y`
	*/
	inline FloatMPFR atan2(FloatMPFR y, FloatMPFR x) {
		FloatMPFR result; result.copy_prec(y);
		mpfr_atan2(result.value, y.value, x.value, MPFR_RNDZ);
		return result;
	}

//------------------------------------------------------------------------------
// Integer and Remainder Related Functions
//------------------------------------------------------------------------------

	/** mpfr_ceil */
	inline FloatMPFR ceil(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_ceil(result.value, x.value);
		return result;
	}
	/** mpfr_floor */
	inline FloatMPFR floor(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_floor(result.value, x.value);
		return result;
	}
	/** mpfr_round */
	inline FloatMPFR round(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_round(result.value, x.value);
		return result;
	}
	/** mpfr_trunc */
	inline FloatMPFR trunc(FloatMPFR x) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_trunc(result.value, x.value);
		return result;
	}

	/** mpfr_modf */
	inline FloatMPFR modf(FloatMPFR x, FloatMPFR* y) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_modf(result.value, y->value, x.value, MPFR_RNDZ);
		return result;
	}

	/** 
	*  mpfr_fmod
	*  @note Uses precision from the first argument `x`
	*/
	inline FloatMPFR fmod(FloatMPFR x, FloatMPFR y) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_fmod(result.value, x.value, y.value, MPFR_RNDZ);
		return result;
	}

	/** 
	*  mpfr_remainder
	*  @note Uses precision from the first argument `x`
	*/
	inline FloatMPFR remainder(FloatMPFR x, FloatMPFR y) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_remainder(result.value, x.value, y.value, MPFR_RNDZ);
		return result;
	}

	/** 
	*  mpfr_remquo
	*  @note Uses precision from the first argument `x`
	*/
	inline FloatMPFR remquo(FloatMPFR x, FloatMPFR y, int* quo) {
		FloatMPFR result; result.copy_prec(x);
		long temp_quo;
		mpfr_remquo(result.value, &temp_quo, x.value, y.value, MPFR_RNDZ);
		*quo = (int)temp_quo;
		return result;
	}

//------------------------------------------------------------------------------
// Miscellaneous Functions
//------------------------------------------------------------------------------

	/** mpfr_signbit */
	inline bool signbit(FloatMPFR x) {
		return (mpfr_signbit(x.value) != 0) ? true : false;
	}
	/** 
	*  mpfr_copysign
	*  @note Uses precision from the first argument `x`
	*/
	inline FloatMPFR copysign(FloatMPFR x, FloatMPFR y) {
		FloatMPFR result; result.copy_prec(x);
		mpfr_copysign(result.value, x.value, y.value, MPFR_RNDZ);
		return result;
		
	}

#endif

#endif /* MPFR_FLOAT_HPP */