/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "frac_Multi_Internal.h"
#include "frac_Multi.h"

#include "frac_Multi_Generic.h"
#include "frac_Multi_SSE2.h"
#include "frac_Multi_AVX.h"
#include "frac_Multi_AVX512F.h"

void renderCPU_ABS_Mandelbrot(BufferBox* buf, Render_Data ren, ABS_Mandelbrot param, std::atomic<bool>& ABORT_RENDERING, uint32_t threadCount) {
	if (validateBufferBox(buf) == false) {
		printError("BufferBox* buf is NULL or has invalid data in renderCPU_ABS_Mandelbrot()");
		return;
	}
	std::vector<std::thread> renderThread;
	/* Thread Creation */
		#define makeThread(k, fpX, fpColor) \
			renderThread.push_back(std::thread(k<fpX, fpColor>,\
				buf->vram, preCalc_Param, p0, p1, std::ref(ABORT_RENDERING)\
			))
		
		#define generateThreads(k, fpX, fpColor) \
			{\
				PreCalc_Param<fpX, fpColor> preCalc_Param;\
				Generate_PreCalc_Param(\
					preCalc_Param,\
					buf, ren, param\
				);\
				for (uint32_t t = 0; t < threadCount; t++) {\
					size_t p0 = (((size_t)buf->resX * (size_t)buf->resY) * (size_t)t) / (size_t)threadCount;\
					size_t p1 = (((size_t)buf->resX * (size_t)buf->resY) * (size_t)(t + 1)) / (size_t)threadCount;\
					makeThread(k, fpX, fpColor);\
				}\
			}

		#define makeThread_Alternate(k, fpX, fpColor) \
			renderThread.push_back(std::thread(k,\
				buf->vram, preCalc_Param, p0, p1, std::ref(ABORT_RENDERING)\
			))

		#define generateThreads_Alternate(k, fpX, fpColor, spacing) \
			{\
				PreCalc_Param<fpX, fpColor> preCalc_Param;\
				Generate_PreCalc_Param(\
					preCalc_Param,\
					buf, ren, param\
				);\
				for (uint32_t t = 0; t < threadCount; t++) {\
					size_t p0 = (((size_t)buf->resX * (size_t)buf->resY) * (size_t)t) / (size_t)threadCount;\
					size_t p1 = (((size_t)buf->resX * (size_t)buf->resY) * (size_t)(t + 1)) / (size_t)threadCount;\
					p0 = (p0 / spacing) * spacing;\
					p1 = (p1 / spacing) * spacing;\
					makeThread_Alternate(k, fpX, fpColor);\
				}\
			}
		
	/* Thread Creation */
	
	// Default is FP64
	if (param.polarMandelbrot == true) {
		switch(ren.CPU_Precision) {
			case 32: generateThreads(polarRender, fp32, fp32); break;
			#ifdef enableFP80andFP128
			case 80: generateThreads(polarRender, fp80, fp64); break;
			case 128: generateThreads(polarRender, fp128, fp64); break;
			#endif
			default:
			case 64: generateThreads(polarRender, fp128, fp64); break;
		};
	} else {
		switch(ren.CPU_Precision) {
			case 32: {
				switch(param.power) {
					#ifdef ENABLE_AVX_RENDERING
						case 2: generateThreads_Alternate(quadraticRender_AVX_FP32 , fp32, fp32, 8); break;
						case 3: generateThreads_Alternate(    cubicRender_AVX_FP32 , fp32, fp32, 8); break;
						case 4: generateThreads_Alternate(  quarticRender_AVX_FP32 , fp32, fp32, 8); break;
						case 5: generateThreads_Alternate(  quinticRender_AVX_FP32 , fp32, fp32, 8); break;
						case 6: generateThreads_Alternate(   sexticRender_AVX_FP32 , fp32, fp32, 8); break;
					#else
						#ifdef ENABLE_SSE2_RENDERING
							case 2: generateThreads_Alternate(quadraticRender_SSE2_FP32, fp32, fp32, 4); break;
							case 3: generateThreads_Alternate(    cubicRender_SSE2_FP32, fp32, fp32, 4); break;
							case 4: generateThreads_Alternate(  quarticRender_SSE2_FP32, fp32, fp32, 4); break;
							case 5: generateThreads_Alternate(  quinticRender_SSE2_FP32, fp32, fp32, 4); break;
							case 6: generateThreads_Alternate(   sexticRender_SSE2_FP32, fp32, fp32, 4); break;
						#else
							case 2: generateThreads          (quadraticRender_Generic  , fp32, fp32   ); break;
							case 3: generateThreads          (    cubicRender_Generic  , fp32, fp32   ); break;
							case 4: generateThreads          (  quarticRender_Generic  , fp32, fp32   ); break;
							case 5: generateThreads          (  quinticRender_Generic  , fp32, fp32   ); break;
							case 6: generateThreads          (   sexticRender_Generic  , fp32, fp32   ); break;
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
						case 2: generateThreads_Alternate(quadraticRender_AVX_FP64 , fp64, fp64, 4); break;
						case 3: generateThreads_Alternate(    cubicRender_AVX_FP64 , fp64, fp64, 4); break;
						case 4: generateThreads_Alternate(  quarticRender_AVX_FP64 , fp64, fp64, 4); break;
						case 5: generateThreads_Alternate(  quinticRender_AVX_FP64 , fp64, fp64, 4); break;
						case 6: generateThreads_Alternate(   sexticRender_AVX_FP64 , fp64, fp64, 4); break;
					#else
						#ifdef ENABLE_SSE2_RENDERING
							case 2: generateThreads_Alternate(quadraticRender_SSE2_FP64, fp64, fp64, 2); break;
							case 3: generateThreads_Alternate(    cubicRender_SSE2_FP64, fp64, fp64, 2); break;
							case 4: generateThreads_Alternate(  quarticRender_SSE2_FP64, fp64, fp64, 2); break;
							case 5: generateThreads_Alternate(  quinticRender_SSE2_FP64, fp64, fp64, 2); break;
							case 6: generateThreads_Alternate(   sexticRender_SSE2_FP64, fp64, fp64, 2); break;
						#else
							case 2: generateThreads          (quadraticRender_Generic  , fp64, fp64   ); break;
							case 3: generateThreads          (    cubicRender_Generic  , fp64, fp64   ); break;
							case 4: generateThreads          (  quarticRender_Generic  , fp64, fp64   ); break;
							case 5: generateThreads          (  quinticRender_Generic  , fp64, fp64   ); break;
							case 6: generateThreads          (   sexticRender_Generic  , fp64, fp64   ); break;
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
						case 2: generateThreads(quadraticRender_Generic, fp80, fp80); break;
						case 3: generateThreads(    cubicRender_Generic, fp80, fp80); break;
						case 4: generateThreads(  quarticRender_Generic, fp80, fp80); break;
						case 5: generateThreads(  quinticRender_Generic, fp80, fp80); break;
						case 6: generateThreads(   sexticRender_Generic, fp80, fp80); break;
					default:
						printfInterval(0.5,"\nError: Unknown render parameters\nPower: %u CPU_Precision: %u",param.power,ren.CPU_Precision);
						return;
				}
			} break;
			#endif
			#ifdef enableFP80andFP128
			case 128: {
				switch(param.power) {
						case 2: generateThreads(quadraticRender_Generic, fp128, fp128); break;
						case 3: generateThreads(    cubicRender_Generic, fp128, fp128); break;
						case 4: generateThreads(  quarticRender_Generic, fp128, fp128); break;
						case 5: generateThreads(  quinticRender_Generic, fp128, fp128); break;
						case 6: generateThreads(   sexticRender_Generic, fp128, fp128); break;
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