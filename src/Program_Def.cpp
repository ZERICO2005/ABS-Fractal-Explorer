/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"

/* Relative File Path */
	static std::mutex mutex_RelativeFilePath;
	static std::string string_RelativeFilePath = "./";
	std::string get_RelativeFilePath() {
		std::lock_guard<std::mutex> lock(mutex_RelativeFilePath);
		return string_RelativeFilePath;
	}
	void set_RelativeFilePath(const std::string& path) {
		std::lock_guard<std::mutex> lock(mutex_RelativeFilePath);
		string_RelativeFilePath = path;
	}

/* FloatCoodinate */

#ifdef Enable_FloatMPFR
	
	#ifdef Enable_Float128
		#define MPFR_WANT_FLOAT128
	#endif
	#include "mpfr.h"
	#define PRImpfr "R"

	static constexpr mpfr_prec_t MPFR_PRECISION = 320;

	// Converts double dekker floats to mpfr_t
	static void fpCord_to_mpfr(
		mpfr_t& cord_value,
		const fpCord& cord
	) {
		#if defined(Enable_Float128) && !defined(Enable_Float80)
			mpfr_set_float128(cord_value, cord, MPFR_RNDZ);
		#else
			mpfr_t cord_hi, cord_lo;
			mpfr_inits2(MPFR_PRECISION, cord_hi, cord_lo, nullptr);
			#if defined(Enable_Float80)
				mpfr_set_ld(cord_hi, cord.hi, MPFR_RNDZ);
				mpfr_set_ld(cord_lo, cord.lo, MPFR_RNDZ);
			#else
				mpfr_set_d(cord_hi, cord.hi, MPFR_RNDZ);
				mpfr_set_d(cord_lo, cord.lo, MPFR_RNDZ);
			#endif
			mpfr_add(cord_value, cord_hi, cord_lo, MPFR_RNDZ);
			mpfr_clears(cord_hi, cord_lo, nullptr);
		#endif
	}

	int FloatCoordinate_snprintf(char* buf, size_t len, const char* format, fpCord cord) {
		mpfr_t cord_value;
		mpfr_init2(cord_value, MPFR_PRECISION);
		fpCord_to_mpfr(cord_value, cord);
		int ret_value = mpfr_snprintf(
			buf, len, format, cord_value
		);
		mpfr_clear(cord_value);
		return ret_value;
	}

	std::string FloatCoordinate_toString(const char* format, fpCord cord) {
		mpfr_t cord_value;
		mpfr_init2(cord_value, MPFR_PRECISION);
		fpCord_to_mpfr(cord_value, cord);
		
		int buf_size = mpfr_snprintf(
			nullptr, 0, format, cord_value
		);
		if (buf_size < 0) {
			std::string str = "<Failed to convert cordinate to string: Formatting Error>";
			return str;
		}
		char* buf = (char*)calloc((size_t)buf_size + 1, sizeof(char));
		if (buf == nullptr) {
			std::string str = "<Failed to convert cordinate to string: Allocation Error>";
			return str;
		}
		int ret_code = mpfr_snprintf(
			buf, (size_t)buf_size, format, cord_value
		);
		if (ret_code < 0) {
			std::string str = "<Failed to convert cordinate to string: Write Error>";
			return str;
		}
		std::string str;
		str.assign(buf);
		FREE(buf);
		mpfr_clear(cord_value);
		return str;
	}

	// Converts mpfr_t to a double dekker float
	static void mpfr_to_fpCord(
		fpCord& cord,
		const mpfr_t& cord_value
	) {
		#if defined(Enable_Float128) && !defined(Enable_Float80)
			cord = mpfr_get_float128(cord_value, MPFR_RNDZ);
		#else
			mpfr_t cord_diff;
			mpfr_init2(cord_diff, MPFR_PRECISION);
			#if defined(Enable_Float80)
				cord.hi = mpfr_get_ld(cord_value, MPFR_RNDZ);
				{ // mpfr_sub_ld doesn't exist
					mpfr_t cord_hi_ld;
					mpfr_init2(cord_hi_ld, MPFR_PRECISION);
					mpfr_set_ld(cord_hi_ld, cord.hi, MPFR_RNDZ);
					mpfr_sub(cord_diff, cord_value, cord_hi_ld, MPFR_RNDZ);
					mpfr_clear(cord_hi_ld);
				}
				cord.lo = mpfr_get_ld(cord_diff, MPFR_RNDZ);
			#else
				cord.hi = mpfr_get_d(cord_value, MPFR_RNDZ);
				mpfr_sub_d(cord_diff, cord_value, cord.hi, MPFR_RNDZ);
				cord.lo = mpfr_get_d(cord_diff, MPFR_RNDZ);
			#endif
			mpfr_clear(cord_diff);
		#endif
	}

	fpCord stringTo_FloatCoordinate(const char* nPtr, char** endPtr) {
		mpfr_t cord_value;
		mpfr_init2(cord_value, MPFR_PRECISION);
		mpfr_strtofr(cord_value, nPtr, endPtr, 10, MPFR_RNDZ);
		fpCord cord;
		mpfr_to_fpCord(cord, cord_value);
		mpfr_clear(cord_value);
		return cord;
	}

#else

	int FloatCoordinate_snprintf(char* buf, size_t len, const char* format, fpCord cord) {
		#if defined(Enable_Float80)
			return Float80x2_snprintf(buf, len, format, cord);
		#elif defined(Enable_Float128)
			return quadmath_snprintf(buf, len, format, (fp128)cord);
		#else
			return Float64x2_snprintf(buf, len, format, cord);
		#endif
	}

	std::string FloatCoordinate_toString(const char* format, fpCord cord) {
		int buf_size = FloatCoordinate_snprintf(
			nullptr, 0, format, cord
		);
		if (buf_size < 0) {
			std::string str = "<Failed to convert cordinate to string: Formatting Error>";
			return str;
		}
		char* buf = (char*)calloc((size_t)buf_size + 1, sizeof(char));
		if (buf == nullptr) {
			std::string str = "<Failed to convert cordinate to string: Allocation Error>";
			return str;
		}
		int ret_code = FloatCoordinate_snprintf(
			buf, (size_t)buf_size, format, cord
		);
		if (ret_code < 0) {
			std::string str = "<Failed to convert cordinate to string: Write Error>";
			return str;
		}
		std::string str;
		str.assign(buf);
		FREE(buf);
		return str;
	}

	fpCord stringTo_FloatCoordinate(const char* nPtr, char** endPtr) {
		#if defined(Enable_Float80)
			return (fpCord)stringTo_Float80x2(nPtr, endPtr);
		#elif defined(Enable_Float128)
			return (fpCord)stringTo_Float128(nPtr, endPtr);
		#else
			return (fpCord)stringTo_Float64x2(nPtr, endPtr);
		#endif
	}

#endif

/* TimerBox */
	/* Constructors */
		TimerBox::TimerBox() {
			updateTime();
			setFreq((nano64_t)0);
			deltaTime = 0;
		}
		TimerBox::TimerBox(nano64_t freq) {
			updateTime();
			setFreq(freq);
			deltaTime = 0;
		}
		TimerBox::TimerBox(fp64 freq) {
			updateTime();
			setFreq(freq);
			deltaTime = 0;
		}
	/* resetTime */
		void TimerBox::updateTime() { resetTime = getNanoTime(); }
		void TimerBox::setTime(nano64_t time) { resetTime = time; }
		void TimerBox::setTime(fp64 time) { resetTime = SECONDS_TO_NANO(time); }
		nano64_t TimerBox::getResetTimeNano() const { return resetTime; }
		fp64 TimerBox::getResetTime() const { return NANO_TO_SECONDS(resetTime); }
	/* deltaTime */
		nano64_t TimerBox::getDeltaTimeNano() const {
			return deltaTime;
		}
		fp64 TimerBox::getDeltaTime() const {
			return NANO_TO_SECONDS(deltaTime);
		}
	/* freqTime */
		void TimerBox::setFreq(nano64_t freq) { freqTime = freq; }
		void TimerBox::setFreq(fp64 freq) { freqTime = SECONDS_TO_NANO(freq); }
		nano64_t TimerBox::getResetFreqNano() const { return freqTime; }
		fp64 TimerBox::getResetFreq() const { return NANO_TO_SECONDS(freqTime); }
	/* timeElapsed */
		nano64_t TimerBox::timeElapsedNano() const {
			return getNanoTime() - resetTime;
		}
		fp64 TimerBox::timeElapsed() const {
			return NANO_TO_SECONDS(getNanoTime() - resetTime);
		}
		nano64_t TimerBox::timeToTimerReadyNano() const {
			return (timeElapsedNano() < freqTime) ? (freqTime - timeElapsedNano()) : 0;
		}
		fp64 TimerBox::timeToTimerReady() const {
			return NANO_TO_SECONDS(timeToTimerReadyNano());
		}
	/* timerReset */
		bool TimerBox::timerReset(nano64_t freq) {
			nano64_t delta = getNanoTime() - resetTime;
			if (delta >= freq) {
				deltaTime = delta;
				resetTime = getNanoTime();
				return true;
			}
			return false;
		}
		bool TimerBox::timerReset() { return timerReset(freqTime); }
		bool TimerBox::timerReset(fp64 freq) { return timerReset(SECONDS_TO_NANO(freq)); }
	/* timerReady */
		bool TimerBox::timerReady(nano64_t freq) const {
			return (getNanoTime() - resetTime >= freq) ? true : false;
		}
		bool TimerBox::timerReady() const { return timerReady(freqTime); }
		bool TimerBox::timerReady(fp64 freq) const { return timerReady(SECONDS_TO_NANO(freq)); }
/* TimerBox */