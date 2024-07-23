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
#include "frac_Multi_AVX512_F.h"

#include "../floats/double_Float32.hpp"
#include "../floats/double_Float64.hpp"
#include "../floats/double_Float80.hpp"

using Render_Func = void(*)();

struct Function_Lookup {
	Mandelbrot_Type frac_type;
	Render_Func func;
	size_t spacing;
	Rendering_Configuration::Rendering_Preset render_preset;
};


const Function_Lookup Function_Lookup_Table[] = {
	/* CPU-Generic Float32 */
	{Mandelbrot_Polar    , (Render_Func)    polarRender_Generic<fp32, fp32>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float32},
	{Mandelbrot_Quadratic, (Render_Func)quadraticRender_Generic<fp32, fp32>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float32},
	{Mandelbrot_Cubic    , (Render_Func)    cubicRender_Generic<fp32, fp32>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float32},
	{Mandelbrot_Quartic  , (Render_Func)  quarticRender_Generic<fp32, fp32>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float32},
	{Mandelbrot_Quintic  , (Render_Func)  quinticRender_Generic<fp32, fp32>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float32},
	{Mandelbrot_Sextic   , (Render_Func)   sexticRender_Generic<fp32, fp32>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float32},
	/* CPU-Generic Float32x2 */
	{Mandelbrot_Polar    , (Render_Func)    polarRender_Generic<fp32x2, fp32>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float32x2},
	{Mandelbrot_Quadratic, (Render_Func)quadraticRender_Generic<fp32x2, fp32>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float32x2},
	{Mandelbrot_Cubic    , (Render_Func)    cubicRender_Generic<fp32x2, fp32>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float32x2},
	{Mandelbrot_Quartic  , (Render_Func)  quarticRender_Generic<fp32x2, fp32>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float32x2},
	{Mandelbrot_Quintic  , (Render_Func)  quinticRender_Generic<fp32x2, fp32>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float32x2},
	{Mandelbrot_Sextic   , (Render_Func)   sexticRender_Generic<fp32x2, fp32>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float32x2},
	/* CPU-Generic Float64 */
	{Mandelbrot_Polar    , (Render_Func)    polarRender_Generic<fp64, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float64},
	{Mandelbrot_Quadratic, (Render_Func)quadraticRender_Generic<fp64, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float64},
	{Mandelbrot_Cubic    , (Render_Func)    cubicRender_Generic<fp64, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float64},
	{Mandelbrot_Quartic  , (Render_Func)  quarticRender_Generic<fp64, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float64},
	{Mandelbrot_Quintic  , (Render_Func)  quinticRender_Generic<fp64, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float64},
	{Mandelbrot_Sextic   , (Render_Func)   sexticRender_Generic<fp64, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float64},
	/* CPU-Generic Float64 */
	{Mandelbrot_Polar    , (Render_Func)    polarRender_Generic<fp64x2, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float64x2},
	{Mandelbrot_Quadratic, (Render_Func)quadraticRender_Generic<fp64x2, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float64x2},
	{Mandelbrot_Cubic    , (Render_Func)    cubicRender_Generic<fp64x2, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float64x2},
	{Mandelbrot_Quartic  , (Render_Func)  quarticRender_Generic<fp64x2, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float64x2},
	{Mandelbrot_Quintic  , (Render_Func)  quinticRender_Generic<fp64x2, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float64x2},
	{Mandelbrot_Sextic   , (Render_Func)   sexticRender_Generic<fp64x2, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float64x2},
	#ifdef Enable_Float80
		/* CPU-Generic Float80 */
		{Mandelbrot_Polar    , (Render_Func)    polarRender_Generic<fp80, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float80},
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_Generic<fp80, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float80},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_Generic<fp80, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float80},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_Generic<fp80, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float80},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_Generic<fp80, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float80},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_Generic<fp80, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float80},
		/* CPU-Generic Float80x2 */
		{Mandelbrot_Polar    , (Render_Func)    polarRender_Generic<fp80x2, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float80x2},
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_Generic<fp80x2, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float80x2},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_Generic<fp80x2, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float80x2},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_Generic<fp80x2, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float80x2},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_Generic<fp80x2, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float80x2},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_Generic<fp80x2, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float80x2},
	#endif
	#ifdef Enable_Float128
		/* CPU-Generic Float128 */
		{Mandelbrot_Polar    , (Render_Func)    polarRender_Generic<fp128, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float128},
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_Generic<fp128, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float128},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_Generic<fp128, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float128},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_Generic<fp128, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float128},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_Generic<fp128, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float128},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_Generic<fp128, fp64>, Spacing_Generic, Rendering_Configuration::Render_Preset_CPU_Generic_Float128},
	#endif
	#ifdef ENABLE_SSE2_RENDERING
		/* CPU-SSE2 Float32 */
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_SSE2_FP32, Spacing_SSE2_Float32, Rendering_Configuration::Render_Preset_CPU_SSE2_Float32},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_SSE2_FP32, Spacing_SSE2_Float32, Rendering_Configuration::Render_Preset_CPU_SSE2_Float32},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_SSE2_FP32, Spacing_SSE2_Float32, Rendering_Configuration::Render_Preset_CPU_SSE2_Float32},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_SSE2_FP32, Spacing_SSE2_Float32, Rendering_Configuration::Render_Preset_CPU_SSE2_Float32},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_SSE2_FP32, Spacing_SSE2_Float32, Rendering_Configuration::Render_Preset_CPU_SSE2_Float32},
		/* CPU-SSE2 Float64 */
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_SSE2_FP64, Spacing_SSE2_Float64, Rendering_Configuration::Render_Preset_CPU_SSE2_Float64},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_SSE2_FP64, Spacing_SSE2_Float64, Rendering_Configuration::Render_Preset_CPU_SSE2_Float64},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_SSE2_FP64, Spacing_SSE2_Float64, Rendering_Configuration::Render_Preset_CPU_SSE2_Float64},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_SSE2_FP64, Spacing_SSE2_Float64, Rendering_Configuration::Render_Preset_CPU_SSE2_Float64},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_SSE2_FP64, Spacing_SSE2_Float64, Rendering_Configuration::Render_Preset_CPU_SSE2_Float64},
	#endif
	#ifdef ENABLE_AVX_RENDERING
		/* CPU-SSE2 Float32 */
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_AVX_FP32, Spacing_AVX_Float32, Rendering_Configuration::Render_Preset_CPU_AVX_Float32},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_AVX_FP32, Spacing_AVX_Float32, Rendering_Configuration::Render_Preset_CPU_AVX_Float32},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_AVX_FP32, Spacing_AVX_Float32, Rendering_Configuration::Render_Preset_CPU_AVX_Float32},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_AVX_FP32, Spacing_AVX_Float32, Rendering_Configuration::Render_Preset_CPU_AVX_Float32},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_AVX_FP32, Spacing_AVX_Float32, Rendering_Configuration::Render_Preset_CPU_AVX_Float32},
		/* CPU-SSE2 Float64 */
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_AVX_FP64, Spacing_AVX_Float64, Rendering_Configuration::Render_Preset_CPU_AVX_Float64},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_AVX_FP64, Spacing_AVX_Float64, Rendering_Configuration::Render_Preset_CPU_AVX_Float64},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_AVX_FP64, Spacing_AVX_Float64, Rendering_Configuration::Render_Preset_CPU_AVX_Float64},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_AVX_FP64, Spacing_AVX_Float64, Rendering_Configuration::Render_Preset_CPU_AVX_Float64},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_AVX_FP64, Spacing_AVX_Float64, Rendering_Configuration::Render_Preset_CPU_AVX_Float64},
	#endif
	#ifdef ENABLE_AVX512_F_RENDERING
		/* CPU-SSE2 Float32 */
		{Mandelbrot_Polar    , (Render_Func)    polarRender_AVX512_F_FP32, Spacing_AVX512_F_Float32, Rendering_Configuration::Render_Preset_CPU_AVX512_Float32},
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_AVX512_F_FP32, Spacing_AVX512_F_Float32, Rendering_Configuration::Render_Preset_CPU_AVX512_Float32},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_AVX512_F_FP32, Spacing_AVX512_F_Float32, Rendering_Configuration::Render_Preset_CPU_AVX512_Float32},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_AVX512_F_FP32, Spacing_AVX512_F_Float32, Rendering_Configuration::Render_Preset_CPU_AVX512_Float32},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_AVX512_F_FP32, Spacing_AVX512_F_Float32, Rendering_Configuration::Render_Preset_CPU_AVX512_Float32},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_AVX512_F_FP32, Spacing_AVX512_F_Float32, Rendering_Configuration::Render_Preset_CPU_AVX512_Float32},
		/* CPU-SSE2 Float64 */
		{Mandelbrot_Polar    , (Render_Func)    polarRender_AVX512_F_FP64, Spacing_AVX512_F_Float64, Rendering_Configuration::Render_Preset_CPU_AVX512_Float64},
		{Mandelbrot_Quadratic, (Render_Func)quadraticRender_AVX512_F_FP64, Spacing_AVX512_F_Float64, Rendering_Configuration::Render_Preset_CPU_AVX512_Float64},
		{Mandelbrot_Cubic    , (Render_Func)    cubicRender_AVX512_F_FP64, Spacing_AVX512_F_Float64, Rendering_Configuration::Render_Preset_CPU_AVX512_Float64},
		{Mandelbrot_Quartic  , (Render_Func)  quarticRender_AVX512_F_FP64, Spacing_AVX512_F_Float64, Rendering_Configuration::Render_Preset_CPU_AVX512_Float64},
		{Mandelbrot_Quintic  , (Render_Func)  quinticRender_AVX512_F_FP64, Spacing_AVX512_F_Float64, Rendering_Configuration::Render_Preset_CPU_AVX512_Float64},
		{Mandelbrot_Sextic   , (Render_Func)   sexticRender_AVX512_F_FP64, Spacing_AVX512_F_Float64, Rendering_Configuration::Render_Preset_CPU_AVX512_Float64},
	#endif
};

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
	for (uint32_t t = 0; t < threadCount; t++) {
		renderThread.at(t).join();
	}
}

void find_Render_Function(
	Render_Func& output_func,
	__attribute__((unused)) size_t& spacing,
	const Mandelbrot_Type frac_type,
	const Rendering_Configuration::Rendering_Preset render_preset,
	const Render_Configurator& CPU_Rendering_Config
) {
	using namespace Rendering_Configuration;
	output_func = nullptr;
	for (size_t i = 0; i < sizeof(Function_Lookup_Table) / sizeof(Function_Lookup); i++) {
		if (
			(Function_Lookup_Table[i].frac_type == frac_type) &&
			(Function_Lookup_Table[i].render_preset == render_preset)
		) {
			output_func = Function_Lookup_Table[i].func;
			return;
		}
	}
	
	constexpr Rendering_Method Render_Method_Attempt_Order[] = {
		Render_Method_CPU_AVX512,
		Render_Method_CPU_AVX,
		Render_Method_CPU_SSE2,
		Render_Method_CPU_Generic
	};
	
	Rendering_Precision target_precision;
	Rendering_Method target_method;
	CPU_Rendering_Config.get_Rendering_Precision_and_Method_from_Preset(
		render_preset, target_precision, target_method
	);

	for (size_t m = 0; m < sizeof(Render_Method_Attempt_Order) / sizeof(Rendering_Method); m++) {
		if (
			(CPU_Rendering_Config.validate_Rendering_Method(Render_Method_Attempt_Order[m]) == false) ||
			(CPU_Rendering_Config.validate_Rendering_Precision_and_Method(target_precision, Render_Method_Attempt_Order[m]) == false)
		) {
			continue;
		}
		for (size_t i = 0; i < sizeof(Function_Lookup_Table) / sizeof(Function_Lookup); i++) {
			Rendering_Precision render_precision;
			Rendering_Method render_method;
			CPU_Rendering_Config.get_Rendering_Precision_and_Method_from_Preset(
				Function_Lookup_Table[i].render_preset,
				render_precision, render_method
			);

			if (
				(Function_Lookup_Table[i].frac_type == frac_type) &&
				(render_precision == target_precision) &&
				(render_method == Render_Method_Attempt_Order[m])
			) {
				output_func = Function_Lookup_Table[i].func;
				return;
			}
		}
	}
	printfInterval(0.5,"Error: frac_type(%d) is not available with %s\n",
		frac_type, Rendering_Preset_Name[render_preset]
	);
}

template<typename fpX, typename fpColor>
bool load_Fractal_Render(
	Mandelbrot_Type Fractal_Type,
	Rendering_Configuration::Rendering_Preset Render_Preset,
	BufferBox* buf, const Render_Data& ren, const ABS_Mandelbrot& param,
	const Render_Configurator& CPU_Render_Config,
	std::atomic<bool>& ABORT_RENDERING, uint32_t threadCount
) {
	size_t spacing = 1;
	void (*func_type)(uint8_t*, PreCalc_Param<fpX, fpColor>, size_t, size_t, std::atomic<bool>&);
	Render_Func ren_func = nullptr;
	find_Render_Function(ren_func, spacing, Fractal_Type, Render_Preset, CPU_Render_Config);
	
	if (ren_func != nullptr) {
		generate_Threads<fpX, fpColor>(
			(decltype(func_type))ren_func, spacing,
			buf, ren, param, ABORT_RENDERING, threadCount
		);
		return true;
	}
	return false;
}

// void test() {
// 	fp128 y = (fp128)1.0 / (fp128)3.0;
// 	fp64x2 x = y;
// 	x.print();
// 	x = (fp64x2)1.0;
// 	x = exp(x);
// 	x.print();
// 	x = -x;
// 	x.print();
// }

void renderCPU_ABS_Mandelbrot(
	BufferBox* buf, Render_Data ren, ABS_Mandelbrot param,
	const Render_Configurator& CPU_Render_Config,
	std::atomic<bool>& ABORT_RENDERING, uint32_t threadCount
) {
	//test();
	using namespace Rendering_Configuration;
	if (validateBufferBox(buf) == false) {
		printError("BufferBox* buf is NULL or has invalid data in renderCPU_ABS_Mandelbrot()");
		return;
	}
	
	/* Thread Creation */

	#define Thread_Arguments buf, ren, param, CPU_Render_Config, ABORT_RENDERING, threadCount

	Mandelbrot_Type Fractal_Type = get_Mandelbrot_Type(param);
	Rendering_Precision Render_Precision = CPU_Render_Config.get_Render_Precision();
	// Rendering_Method Render_Method = CPU_Render_Config.get_Render_Method();
	Rendering_Preset Render_Preset = CPU_Render_Config.get_Render_Preset();

	switch(Render_Precision) {
		case Render_Precision_Float16: {
			printfInterval(0.5, "Error: Rendering_Precision Float16 is not supported\n");
		} break;
		case Render_Precision_Float32: {
			load_Fractal_Render<fp32, fp32>(Fractal_Type, Render_Preset, Thread_Arguments);
		} break;
		case Render_Precision_Float32x2: {
			load_Fractal_Render<fp32x2, fp32>(Fractal_Type, Render_Preset, Thread_Arguments);
		} break;
		case Render_Precision_Float64: {
			load_Fractal_Render<fp64, fp64>(Fractal_Type, Render_Preset, Thread_Arguments);
		} break;
		case Render_Precision_Float64x2: {
			load_Fractal_Render<fp64x2, fp64>(Fractal_Type, Render_Preset, Thread_Arguments);
		} break;
		#ifdef Enable_Float80
			case Render_Precision_Float80: {
				load_Fractal_Render<fp80, fp64>(Fractal_Type, Render_Preset, Thread_Arguments);
			} break;
			case Render_Precision_Float80x2: {
				load_Fractal_Render<fp80x2, fp64>(Fractal_Type, Render_Preset, Thread_Arguments);
			} break;
		#endif
		#ifdef Enable_Float128
			case Render_Precision_Float128: {
				load_Fractal_Render<fp128, fp64>(Fractal_Type, Render_Preset, Thread_Arguments);
			} break;
		#endif
		default:
			printfInterval(0.5, "Error: Invalid rendering precision enum: %d\n", Render_Precision);
			break;
	}

	if (ABORT_RENDERING == true) {
		printFlush("Aborted %" PRIu32 " threads\n", threadCount);
	}
}