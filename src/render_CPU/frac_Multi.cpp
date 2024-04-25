/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "frac_Multi_Internal.h"
#include "frac_Multi.h"

#include "../render_Configuration.hpp"

#include "frac_Multi_Generic.hpp"
#include "frac_Multi_SSE2.h"
#include "frac_Multi_AVX.h"
#include "frac_Multi_AVX512F.h"

using Render_Func = void(*)();

struct Function_Lookup {
	Mandelbrot_Type frac_type;
	Render_Func func;
	Rendering_Configuration::Rendering_Preset render_preset;
};

const Function_Lookup Function_Lookup_Table[] = {
	/* CPU-Generic Float32 */
	{Mandelbrot_Polar    , (Render_Func)    polarRender_Generic<fp32, fp32>, Rendering_Configuration::Render_Preset_CPU_Generic_Float32},
	{Mandelbrot_Quadratic, (Render_Func)quadraticRender_Generic<fp32, fp32>, Rendering_Configuration::Render_Preset_CPU_Generic_Float32},
	{Mandelbrot_Cubic    , (Render_Func)    cubicRender_Generic<fp32, fp32>, Rendering_Configuration::Render_Preset_CPU_Generic_Float32},
	{Mandelbrot_Quartic  , (Render_Func)  quarticRender_Generic<fp32, fp32>, Rendering_Configuration::Render_Preset_CPU_Generic_Float32},
	{Mandelbrot_Quintic  , (Render_Func)  quinticRender_Generic<fp32, fp32>, Rendering_Configuration::Render_Preset_CPU_Generic_Float32},
	{Mandelbrot_Sextic   , (Render_Func)   sexticRender_Generic<fp32, fp32>, Rendering_Configuration::Render_Preset_CPU_Generic_Float32},
	/* CPU-Generic Float64 */
	{Mandelbrot_Polar    , (Render_Func)    polarRender_Generic<fp64, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float64},
	{Mandelbrot_Quadratic, (Render_Func)quadraticRender_Generic<fp64, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float64},
	{Mandelbrot_Cubic    , (Render_Func)    cubicRender_Generic<fp64, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float64},
	{Mandelbrot_Quartic  , (Render_Func)  quarticRender_Generic<fp64, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float64},
	{Mandelbrot_Quintic  , (Render_Func)  quinticRender_Generic<fp64, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float64},
	{Mandelbrot_Sextic   , (Render_Func)   sexticRender_Generic<fp64, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float64},
	#ifdef enableFP80andFP128
		/* CPU-Generic Float80 */
		{Mandelbrot_Polar    , (Render_Func)    polarRender_Generic<fp80, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float80},
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_Generic<fp80, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float80},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_Generic<fp80, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float80},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_Generic<fp80, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float80},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_Generic<fp80, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float80},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_Generic<fp80, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float80},
		/* CPU-Generic Float128 */
		{Mandelbrot_Polar    , (Render_Func)    polarRender_Generic<fp128, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float128},
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_Generic<fp128, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float128},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_Generic<fp128, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float128},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_Generic<fp128, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float128},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_Generic<fp128, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float128},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_Generic<fp128, fp64>, Rendering_Configuration::Render_Preset_CPU_Generic_Float128},
	#endif
	#ifdef ENABLE_SSE2_RENDERING
		/* CPU-SSE2 Float32 */
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_SSE2_FP32, Rendering_Configuration::Render_Preset_CPU_SSE2_Float32},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_SSE2_FP32, Rendering_Configuration::Render_Preset_CPU_SSE2_Float32},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_SSE2_FP32, Rendering_Configuration::Render_Preset_CPU_SSE2_Float32},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_SSE2_FP32, Rendering_Configuration::Render_Preset_CPU_SSE2_Float32},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_SSE2_FP32, Rendering_Configuration::Render_Preset_CPU_SSE2_Float32},
		/* CPU-SSE2 Float64 */
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_SSE2_FP64, Rendering_Configuration::Render_Preset_CPU_SSE2_Float64},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_SSE2_FP64, Rendering_Configuration::Render_Preset_CPU_SSE2_Float64},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_SSE2_FP64, Rendering_Configuration::Render_Preset_CPU_SSE2_Float64},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_SSE2_FP64, Rendering_Configuration::Render_Preset_CPU_SSE2_Float64},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_SSE2_FP64, Rendering_Configuration::Render_Preset_CPU_SSE2_Float64},
	#endif
	#ifdef ENABLE_AVX_RENDERING
		/* CPU-SSE2 Float32 */
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_AVX_FP32, Rendering_Configuration::Render_Preset_CPU_AVX_Float32},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_AVX_FP32, Rendering_Configuration::Render_Preset_CPU_AVX_Float32},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_AVX_FP32, Rendering_Configuration::Render_Preset_CPU_AVX_Float32},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_AVX_FP32, Rendering_Configuration::Render_Preset_CPU_AVX_Float32},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_AVX_FP32, Rendering_Configuration::Render_Preset_CPU_AVX_Float32},
		/* CPU-SSE2 Float64 */
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_AVX_FP64, Rendering_Configuration::Render_Preset_CPU_AVX_Float64},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_AVX_FP64, Rendering_Configuration::Render_Preset_CPU_AVX_Float64},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_AVX_FP64, Rendering_Configuration::Render_Preset_CPU_AVX_Float64},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_AVX_FP64, Rendering_Configuration::Render_Preset_CPU_AVX_Float64},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_AVX_FP64, Rendering_Configuration::Render_Preset_CPU_AVX_Float64},
	#endif
	#ifdef ENABLE_AVX512_F_RENDERING
		/* CPU-SSE2 Float32 */
		{Mandelbrot_Polar    , (Render_Func)    polarRender_AVX512_F_FP32, Rendering_Configuration::Render_Preset_CPU_AVX512_Float32},
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_AVX512_F_FP32, Rendering_Configuration::Render_Preset_CPU_AVX512_Float32},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_AVX512_F_FP32, Rendering_Configuration::Render_Preset_CPU_AVX512_Float32},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_AVX512_F_FP32, Rendering_Configuration::Render_Preset_CPU_AVX512_Float32},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_AVX512_F_FP32, Rendering_Configuration::Render_Preset_CPU_AVX512_Float32},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_AVX512_F_FP32, Rendering_Configuration::Render_Preset_CPU_AVX512_Float32},
		/* CPU-SSE2 Float64 */
		{Mandelbrot_Polar    , (Render_Func)    polarRender_AVX512_F_FP64, Rendering_Configuration::Render_Preset_CPU_AVX512_Float64},
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_AVX512_F_FP64, Rendering_Configuration::Render_Preset_CPU_AVX512_Float64},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_AVX512_F_FP64, Rendering_Configuration::Render_Preset_CPU_AVX512_Float64},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_AVX512_F_FP64, Rendering_Configuration::Render_Preset_CPU_AVX512_Float64},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_AVX512_F_FP64, Rendering_Configuration::Render_Preset_CPU_AVX512_Float64},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_AVX512_F_FP64, Rendering_Configuration::Render_Preset_CPU_AVX512_Float64},
	#endif
};

void find_Render_Function(
	Render_Func& output_func,
	const Mandelbrot_Type frac_type,
	const Rendering_Configuration::Rendering_Preset render_preset
) {
	using namespace Rendering_Configuration;
	for (size_t i = 0; i < sizeof(Function_Lookup_Table) / sizeof(Function_Lookup); i++) {
		if (
			(Function_Lookup_Table[i].frac_type == frac_type) &&
			(Function_Lookup_Table[i].render_preset == render_preset)
		) {
			output_func = Function_Lookup_Table[i].func;
			return;
		}
	}
	printfInterval(0.5,"\nError: frac_type(%d) is not available with %s",
		frac_type, Rendering_Preset_Name[render_preset]
	);
}

// template<typename fpX, typename fpColor>
// void generate_Threads(
// 	void (*func)(uint8_t*, PreCalc_Param<fpX, fpColor>, size_t, size_t, std::atomic<bool>&),
// 	std::vector<std::thread>& renderThread,
// 	BufferBox* buf, const Render_Data& ren, const ABS_Mandelbrot& param,
// 	std::atomic<bool>& ABORT_RENDERING, uint32_t threadCount
// ) {
// 	PreCalc_Param<fpX, fpColor> preCalc_Param;
// 	Generate_PreCalc_Param(
// 		preCalc_Param,
// 		buf, ren, param
// 	);
// 	for (uint32_t t = 0; t < threadCount; t++) {
// 		size_t p0 = (((size_t)buf->resX * (size_t)buf->resY) * (size_t)t) / (size_t)threadCount;
// 		size_t p1 = (((size_t)buf->resX * (size_t)buf->resY) * (size_t)(t + 1)) / (size_t)threadCount;
// 		renderThread.push_back(std::thread(func,
// 			buf->vram, preCalc_Param, p0, p1, std::ref(ABORT_RENDERING)
// 		));
// 	}
// }

template<typename fpX, typename fpColor>
void generate_Threads(
	void (*func)(uint8_t*, PreCalc_Param<fpX, fpColor>, size_t, size_t, std::atomic<bool>&),
	size_t spacing,
	BufferBox* buf, const Render_Data& ren, const ABS_Mandelbrot& param,
	std::atomic<bool>& ABORT_RENDERING, uint32_t threadCount
) {
	std::vector<std::thread> renderThread;
	PreCalc_Param<fpX, fpColor> preCalc_Param;
	Generate_PreCalc_Param(
		preCalc_Param,
		buf, ren, param
	);
	for (uint32_t t = 0; t < threadCount; t++) {
		size_t p0 = (((size_t)buf->resX * (size_t)buf->resY) * (size_t)t) / (size_t)threadCount;
		size_t p1 = (((size_t)buf->resX * (size_t)buf->resY) * (size_t)(t + 1)) / (size_t)threadCount;
		p0 = (p0 / spacing) * spacing;
		p1 = (p1 / spacing) * spacing;
		renderThread.push_back(std::thread(func,
			buf->vram, preCalc_Param, p0, p1, std::ref(ABORT_RENDERING)
		));
	}
}

void renderCPU_ABS_Mandelbrot(
	BufferBox* buf, Render_Data ren, ABS_Mandelbrot param,
	int_enum render_preset,
	std::atomic<bool>& ABORT_RENDERING, uint32_t threadCount
) {
	using namespace Rendering_Configuration;
	if (validateBufferBox(buf) == false) {
		printError("BufferBox* buf is NULL or has invalid data in renderCPU_ABS_Mandelbrot()");
		return;
	}
	

	constexpr size_t Spacing_Generic = 1;

	constexpr size_t Spacing_SSE2_FP32 = 4;
	constexpr size_t Spacing_SSE2_FP64 = 2;
	constexpr size_t Spacing_AVX_FP32 = 8;
	constexpr size_t Spacing_AVX_FP64 = 4;

	/* Thread Creation */

	#define Thread_Arguments \
		buf, ren, param, ABORT_RENDERING, threadCount
	
	// Default is FP64
	if (param.polarMandelbrot == true) {
		switch(ren.CPU_Precision) {
			case 32: // generate_Threads(polarRender_Generic<fp32, fp32>, Thread_Arguments); break;
			{
				void (*func_type)(uint8_t*, PreCalc_Param<fp32, fp32>, size_t, size_t, std::atomic<bool>&);
				Render_Func ren_func = nullptr;
				find_Render_Function(ren_func, Mandelbrot_Polar, Render_Preset_CPU_Generic_Float32);
				generate_Threads<fp32,fp32>((decltype(func_type))ren_func, Spacing_Generic, Thread_Arguments); break;
				break;
			}
			#ifdef enableFP80andFP128
			case 80: generate_Threads(polarRender_Generic<fp64, fp64>, Spacing_Generic, Thread_Arguments); break;
			case 128: generate_Threads(polarRender_Generic<fp80, fp64>, Spacing_Generic, Thread_Arguments); break;
			#endif
			default:
			case 64: generate_Threads(polarRender_Generic<fp128, fp64>, Spacing_Generic, Thread_Arguments); break;
		};
	} else {
		switch(ren.CPU_Precision) {
			case 32: {
				switch(param.power) {
					#ifdef ENABLE_AVX_RENDERING
						case Mandelbrot_Quadratic: generate_Threads(quadraticRender_AVX_FP32, Spacing_AVX_FP32, Thread_Arguments); break;
						case Mandelbrot_Cubic    : generate_Threads(    cubicRender_AVX_FP32, Spacing_AVX_FP32, Thread_Arguments); break;
						case Mandelbrot_Quartic  : generate_Threads(  quarticRender_AVX_FP32, Spacing_AVX_FP32, Thread_Arguments); break;
						case Mandelbrot_Quintic  : generate_Threads(  quinticRender_AVX_FP32, Spacing_AVX_FP32, Thread_Arguments); break;
						case Mandelbrot_Sextic   : generate_Threads(   sexticRender_AVX_FP32, Spacing_AVX_FP32, Thread_Arguments); break;
					#else
						#ifdef ENABLE_SSE2_RENDERING
							case Mandelbrot_Quadratic: generate_Threads(quadraticRender_SSE2_FP32, Spacing_SSE2_FP32, Thread_Arguments); break;
							case Mandelbrot_Cubic    : generate_Threads(    cubicRender_SSE2_FP32, Spacing_SSE2_FP32, Thread_Arguments); break;
							case Mandelbrot_Quartic  : generate_Threads(  quarticRender_SSE2_FP32, Spacing_SSE2_FP32, Thread_Arguments); break;
							case Mandelbrot_Quintic  : generate_Threads(  quinticRender_SSE2_FP32, Spacing_SSE2_FP32, Thread_Arguments); break;
							case Mandelbrot_Sextic   : generate_Threads(   sexticRender_SSE2_FP32, Spacing_SSE2_FP32, Thread_Arguments); break;
						#else
							case Mandelbrot_Quadratic: generate_Threads(quadraticRender_Generic<fp32, fp32>, Spacing_Generic, Thread_Arguments); break;
							case Mandelbrot_Cubic    : generate_Threads(    cubicRender_Generic<fp32, fp32>, Spacing_Generic, Thread_Arguments); break;
							case Mandelbrot_Quartic  : generate_Threads(  quarticRender_Generic<fp32, fp32>, Spacing_Generic, Thread_Arguments); break;
							case Mandelbrot_Quintic  : generate_Threads(  quinticRender_Generic<fp32, fp32>, Spacing_Generic, Thread_Arguments); break;
							case Mandelbrot_Sextic   : generate_Threads(   sexticRender_Generic<fp32, fp32>, Spacing_Generic, Thread_Arguments); break;
						#endif
					#endif
					default:
						printfInterval(0.5,"\nError: Unknown render parameters\nPower: %u CPU_Precision: %u",param.power,ren.CPU_Precision);
						return;
				}
			} break;
			default:
			case 64: {
				switch(param.power) {
					#ifdef ENABLE_AVX_RENDERING
						case Mandelbrot_Quadratic: generate_Threads(quadraticRender_AVX_FP64, Spacing_AVX_FP64, Thread_Arguments); break;
						case Mandelbrot_Cubic    : generate_Threads(    cubicRender_AVX_FP64, Spacing_AVX_FP64, Thread_Arguments); break;
						case Mandelbrot_Quartic  : generate_Threads(  quarticRender_AVX_FP64, Spacing_AVX_FP64, Thread_Arguments); break;
						case Mandelbrot_Quintic  : generate_Threads(  quinticRender_AVX_FP64, Spacing_AVX_FP64, Thread_Arguments); break;
						case Mandelbrot_Sextic   : generate_Threads(   sexticRender_AVX_FP64, Spacing_AVX_FP64, Thread_Arguments); break;
					#else
						#ifdef ENABLE_SSE2_RENDERING
							case Mandelbrot_Quadratic: generate_Threads(quadraticRender_SSE2_FP64, Spacing_SSE2_FP64, Thread_Arguments); break;
							case Mandelbrot_Cubic    : generate_Threads(    cubicRender_SSE2_FP64, Spacing_SSE2_FP64, Thread_Arguments); break;
							case Mandelbrot_Quartic  : generate_Threads(  quarticRender_SSE2_FP64, Spacing_SSE2_FP64, Thread_Arguments); break;
							case Mandelbrot_Quintic  : generate_Threads(  quinticRender_SSE2_FP64, Spacing_SSE2_FP64, Thread_Arguments); break;
							case Mandelbrot_Sextic   : generate_Threads(   sexticRender_SSE2_FP64, Spacing_SSE2_FP64, Thread_Arguments); break;
						#else
							case Mandelbrot_Quadratic: generate_Threads(quadraticRender_Generic<fp64, fp64>, Spacing_Generic, Thread_Arguments); break;
							case Mandelbrot_Cubic    : generate_Threads(    cubicRender_Generic<fp64, fp64>, Spacing_Generic, Thread_Arguments); break;
							case Mandelbrot_Quartic  : generate_Threads(  quarticRender_Generic<fp64, fp64>, Spacing_Generic, Thread_Arguments); break;
							case Mandelbrot_Quintic  : generate_Threads(  quinticRender_Generic<fp64, fp64>, Spacing_Generic, Thread_Arguments); break;
							case Mandelbrot_Sextic   : generate_Threads(   sexticRender_Generic<fp64, fp64>, Spacing_Generic, Thread_Arguments); break;
						#endif
					#endif
					default:
						printfInterval(0.5,"\nError: Unknown render parameters\nPower: %u CPU_Precision: %u",param.power,ren.CPU_Precision);
						return;
				}
			} break;
			#ifdef enableFP80andFP128
			case 80: {
				switch(param.power) {
						case Mandelbrot_Quadratic: generate_Threads(quadraticRender_Generic<fp80, fp64>, Spacing_Generic, Thread_Arguments); break;
						case Mandelbrot_Cubic    : generate_Threads(    cubicRender_Generic<fp80, fp64>, Spacing_Generic, Thread_Arguments); break;
						case Mandelbrot_Quartic  : generate_Threads(  quarticRender_Generic<fp80, fp64>, Spacing_Generic, Thread_Arguments); break;
						case Mandelbrot_Quintic  : generate_Threads(  quinticRender_Generic<fp80, fp64>, Spacing_Generic, Thread_Arguments); break;
						case Mandelbrot_Sextic   : generate_Threads(   sexticRender_Generic<fp80, fp64>, Spacing_Generic, Thread_Arguments); break;
					default:
						printfInterval(0.5,"\nError: Unknown render parameters\nPower: %u CPU_Precision: %u",param.power,ren.CPU_Precision);
						return;
				}
			} break;
			#endif
			#ifdef enableFP80andFP128
			case 128: {
				switch(param.power) {
						case Mandelbrot_Quadratic: generate_Threads(quadraticRender_Generic<fp128, fp64>, Spacing_Generic, Thread_Arguments); break;
						case Mandelbrot_Cubic    : generate_Threads(    cubicRender_Generic<fp128, fp64>, Spacing_Generic, Thread_Arguments); break;
						case Mandelbrot_Quartic  : generate_Threads(  quarticRender_Generic<fp128, fp64>, Spacing_Generic, Thread_Arguments); break;
						case Mandelbrot_Quintic  : generate_Threads(  quinticRender_Generic<fp128, fp64>, Spacing_Generic, Thread_Arguments); break;
						case Mandelbrot_Sextic   : generate_Threads(   sexticRender_Generic<fp128, fp64>, Spacing_Generic, Thread_Arguments); break;
					default:
						printfInterval(0.5,"\nError: Unknown render parameters\nPower: %u CPU_Precision: %u",param.power,ren.CPU_Precision);
						return;
				}
			} break;
			#endif
		}
	}

	for (uint32_t t = 0; t < threadCount; t++) {
		renderThread.at(t).join();
	}
	if (ABORT_RENDERING == true) {
		printFlush("\nAborted %" PRIu32 " threads", threadCount);
	}
}