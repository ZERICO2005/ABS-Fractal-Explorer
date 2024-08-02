/*
**	Author: zerico2005 (2023 - 2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#ifndef DOUBLE_FLOATN_SNPRINTF_HPP
#define DOUBLE_FLOATN_SNPRINTF_HPP

#include <cmath>
#include <cstdarg>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <iostream>

template<typename FloatNx2>
class internal_double_FloatN_snprintf {
	private:
	
	/** Searches for the first %, skips %% */
	static const char* find_format_start(const char* format) {
		if (format == nullptr) { return nullptr; }
		size_t pos = 0;
		while (format[pos] != '\0') {
			if (format[pos] == '%') {
				pos++;
				if (format[pos] != '%') {
					return &format[pos];
				}
			}
			pos++;
		}
		return nullptr;
	}

	/** Parses - + <space> # 0 */
	static const char* parse_flags(
		const char* format,
		bool& left_justify,
		bool& explicit_sign,
		bool& space_sign,
		bool& explicit_decimal_point,
		bool& pad_zeros
	) {
		left_justify = false;
		explicit_sign = false;
		space_sign = false;
		explicit_decimal_point = false;
		pad_zeros = false;
		if (format == nullptr) { return nullptr; }

		size_t pos = 0;
		while (format[pos] != '\0') {
			switch (format[pos]) {
				case '-':
					left_justify = true;
				break;
				case '+':
					explicit_sign = true;
				break;
				case ' ':
					space_sign = true;
				break;
				case '#':
					explicit_decimal_point = true;
				break;
				case '0':
					pad_zeros = true;
				break;
				default:
					return &format[pos];
			}
			pos++;
		}
		return &format[pos];
	}

	/** Sets format width and detects if the width is in a va_arg */
	static const char* parse_width(
		const char* format,
		int& width,
		bool& custom_width
	) {
		width = 0;
		custom_width = false;
		if (format == nullptr) { return nullptr; }

		if (*format == '*') {
			custom_width = true;
			format++; // Return the next character
			return format;
		}
		if (*format >= '1' && *format <= '9') {
			char* end_ptr = nullptr;
			width = (int)strtol(format, &end_ptr, 10);
			if (end_ptr == nullptr) {
				return format;
			}
			return end_ptr; // Return character after integer
		}
		return format;
	}

	/** Sets format precision and detects if the precision is in a va_arg */
	static const char* parse_precision(
		const char* format,
		int& precision,
		bool& custom_precision
	) {
		precision = 6;
		custom_precision = false;
		if (format == nullptr) { return nullptr; }
		
		if (*format != '.') {
			return format;
		}
		format++; // Grab integer after '.'

		if (*format == '*') {
			custom_precision = true;
			format++; // Return the next character
			return format;
		}
		if (*format >= '1' && *format <= '9') {
			char* end_ptr = nullptr;
			precision = (int)strtol(format, &end_ptr, 10);
			if (end_ptr == nullptr) {
				return format;
			}
			return end_ptr; // Return character after integer
		}
		return format;
	}

	/** Searches for the PRIFloatNx2 specifier and aAeEfFgG */
	static const char* parse_specifier(
		const char* format,
		char& specifier,
		const char* PRIFloatNx2
	) {
		if (format == nullptr) { return nullptr; }
		specifier = '\0';
		if (strcmp(format, PRIFloatNx2) == 0) {
			return format;
		}
		format++;
		switch(*format) {
			case 'a':
			case 'A':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
			case 'g':
			case 'G':
				specifier = *format;
				format++;
				return format;
			default:
				return format;
		}
		return format;
	}

	struct FloatNx2_format_param {
		int width; // %*Df
		int precision; // %.*Df
		bool left_justify; // %-Df
		bool explicit_sign; // %+Df
		bool space_sign; // % Df
		bool explicit_decimal_point; // %#Df
		bool pad_zeros; // %0Df
	};

	/**
	* @brief Prints decimial/hexadecimal digit
	* @note Assumes base is either 10 or 16 for %f, %F, %a, and %A
	*/
	static char get_digit_from_base(
		int value, int base, bool upperCase
	) {
		assert(base == 10 || base == 16);
		if (value >= 0 && value <= 9) {
			return (char)(value + '0');
		}
		if (base == 16) {
			if (value >= 10 && value <= 16) {
				return (char)(value + (upperCase ? 'A' : 'a') - 10);
			}
		}
		return '?';
	}

	static std::string FloatNx2_write(
		const FloatNx2_format_param& param,
		FloatNx2 value, const int base, const bool upperCase
	) {

		std::string str = "";
		if (value < static_cast<FloatNx2>(0.0)) {
			str += '-';
		} else if (param.explicit_sign) {
			str += '+';
		} else if (param.space_sign) {
			str += ' ';
		}
		if (isinf(value)) {
			str += upperCase ? "INFINITY" : "infinity";
			return str;
		}
		if (isnan(value)) {
			str += upperCase ? "NAN" : "nan";
			return str;
		}
		value = fabs(value);
		FloatNx2 value_int = trunc(value);
		FloatNx2 value_frac = value - value_int;

		if (value_int == static_cast<FloatNx2>(0.0)) {
			str += '0';
		} else {
			/* Integer Part */
			std::string int_digits = "";
			while (value_int > static_cast<FloatNx2>(0.0)) {
				FloatNx2 digit_temp = value_int - (trunc(value_int / (FloatNx2)base) * (FloatNx2)base);
				char digit = get_digit_from_base((int)digit_temp, base, upperCase);
				int_digits += digit;
				value_int = trunc(value_int / (FloatNx2)base);
			}
			reverse(int_digits.begin(), int_digits.end());
			str += int_digits;
		}
		
		if (param.precision != 0) {
			
			/* Fractional/Decimal Part */
			str += '.';
			std::string frac_digits = "";
			for (int d = 0; d < param.precision - 1; d++) {
				if (value_frac == static_cast<FloatNx2>(0.0)) {
					// Rest of the digits will be 0
					frac_digits += '0';
					continue;
				}
				value_frac *= (FloatNx2)base;
				FloatNx2 digit_temp = trunc(value_frac);
				char digit = get_digit_from_base((int)digit_temp, base, upperCase);
				frac_digits += digit;
				value_frac = value_frac - trunc(value_frac);
			}
			{ /* Rounds the last digit */
				value_frac *= (FloatNx2)base;
				FloatNx2 digit_temp = round(value_frac);
				char digit = get_digit_from_base((int)digit_temp, base, upperCase);
				frac_digits += digit;
			}
			
			str += frac_digits;
		} else if (param.explicit_decimal_point) {
			str += '.';
		}

		size_t len_str = str.length();
		size_t pad_width = (param.width < 0) ? 0 : (size_t)param.width;
		if (len_str >= pad_width) {
			return str;
		}
		
		std::string padded_str = "";
		size_t pad_length = pad_width - len_str;
		const char pad_char = (param.pad_zeros && param.left_justify == false) ? '0' : ' ';
		for (size_t i = 0; i < pad_length; i++) {
			padded_str += pad_char;
		}
		
		if (param.left_justify) {  
			str += padded_str;
			return str;
		}
		padded_str += str;
		
		return padded_str;
	}

	public:

	static int FloatNx2_snprintf(
		const char* PRIFloatNx2,
		char* buf, size_t len,
		const char* format, va_list args
	) {
		const char* fm_ptr = find_format_start(format);
		if (fm_ptr == nullptr || *fm_ptr == '\0') {
			return -1; // Invalid format
		}

		const char* const fm_start = fm_ptr;

		FloatNx2_format_param param;

		fm_ptr = parse_flags(
			fm_ptr,
			param.left_justify, param.explicit_sign, param.space_sign,
			param.explicit_decimal_point, param.pad_zeros
		);
		bool custom_width;
		fm_ptr = parse_width(fm_ptr, param.width, custom_width);
		if (custom_width) {
			param.width = va_arg(args, int);
			if (param.width < 0) {
				param.left_justify = true;
				param.width = -param.width;
			}
		}
		bool custom_precision;
		fm_ptr = parse_precision(fm_ptr, param.precision, custom_precision);
		if (custom_precision) {
			param.precision = va_arg(args, int);
			if (param.precision < 0) {
				param.precision = 0;
			}
		}
		char specifier;
		fm_ptr = parse_specifier(fm_ptr, specifier, PRIFloatNx2);

		switch(specifier) {
			case 'a':
			case 'A':
			case 'e':
			case 'E':
			case 'f':
			case 'F':
			case 'g':
			case 'G':
				break;
			case '\0':
			default: // Invalid specifier
				// va_end(args);
				strncpy(buf, format, len);
				return (int)strlen(format);
		}

		FloatNx2 value = va_arg(args, FloatNx2);
		std::string output_str = "";
		if (fm_start > format) { /* Copy text before % */
			size_t copy_amount = (size_t)(fm_start - format);
			output_str.append(format, copy_amount - 1);
		}

		switch (specifier) {
			case 'f':
				output_str += FloatNx2_write(param, value, 10, false);
				break;
			case 'F':
				output_str += FloatNx2_write(param, value, 10, true);
				break;
			case 'a':
				output_str += "<unsupported %a FloatNx2_snprintf>";
				break;
			case 'A':
				output_str += "<unsupported %A FloatNx2_snprintf>";
				break;
			case 'e':
				output_str += "<unsupported %A FloatNx2_snprintf>";
				break;
			case 'E':
				output_str += "<unsupported %A FloatNx2_snprintf>";
				break;
			case 'g':
				output_str += "<unsupported %A FloatNx2_snprintf>";
				break;
			case 'G':
				output_str += "<unsupported %A FloatNx2_snprintf>";
				break;
		}
		
		{ /* Copy the remainding text after % */
			output_str.append(fm_ptr);
		}
		strncpy(buf, output_str.c_str(), len);

		// va_end(args);
		return (int)strlen(format);
	}
};

#endif /* DOUBLE_FLOATN_SNPRINTF_HPP */