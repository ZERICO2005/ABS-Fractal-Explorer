/*
**	Author: zerico2005 (2023)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/
#include "Common_Def.h"
#include "fracMulti.h"
#include "fractal.h"
#include "render.h"
/* BOILERPLATE */

#define BREAKOUT 4096.0

#define FractalParameters BufferBox* buf, Render_Data ren, ABS_Mandelbrot param, uint32_t p0, uint32_t p1, std::atomic<bool>& ABORT_RENDERING

#define Block0(fpX) \
uint8_t* data = buf->vram;\
u32 subSample = ren.subSample;\
u32 resX = buf->resX;\
u32 resY = buf->resY;\
u32 dataPtr = p0 * 3;\
u32 maxItr = param.maxItr;\
fpX r = param.r;\
fpX i = param.i;\
fpX zoom = param.zoom;\
u32 y = p0 / resX;\
u32 x = p0 % resX;\
u32 sample = ren.sample;\
fpX cr = (fpX)0.0;\
fpX ci = (fpX)0.0;\
fpX zr = (fpX)0.0;\
fpX zi = (fpX)0.0;\
resX *= sample;\
resY *= sample;\
x *= sample;\
y *= sample;\
uint32_t sResX = resX - 1;\
uint32_t sResY = resY - 1;\
fpX numY = ((fpX)sResY / (fpX)2.0);\
fpX numX = ((fpX)sResX / (fpX)2.0);\
fpX numZ = (sResX >= sResY) ? numY * pow((fpX)10.0, zoom) : numX * pow((fpX)10.0, zoom);

#define Block1(fpX) for (; y < resY; y += sample) {\
for (; x < resX; x += sample) {\
		if (p0 == p1 || ABORT_RENDERING == true) {\
			return;\
		}\
		uint32_t outR = 0;\
		uint32_t outG = 0;\
		uint32_t outB = 0;\
		for (u32 v = 0; v < sample; v++) {\
			for (u32 u = 0; u < sample; u++) {\
				if (param.juliaSet == true) {\
					cpu_pixel_to_coordinate(x, y, &zr, &zi, &param, resX, resY, subSample);\
					cr = param.zr;\
					ci = param.zi;\
				} else {\
					cpu_pixel_to_coordinate(x, y, &cr, &ci, &param, resX, resY, subSample);\
					zr = (param.startingZ == false) ? (fpX)0.0 : param.zr;\
					zi = (param.startingZ == false) ? (fpX)0.0 : param.zi;\
				}\
				fpX low = (fpX)4.0;\
				fpX temp;\
				fpX zs = (fpX)0.0;\
				for (u32 itr = 0; itr < maxItr; itr++) {

#define Block2(fpX,l) zs = zr * zr + zi * zi;\
					if (zs < low) {\
						low = zs;\
					} else if (zs > param.breakoutValue) {\
						fp64 smooth = log(1.0 + fmax(0.0, (fp64)itr - (fp64)log2(log2(zs) / (fpX)2.0) / log2(l)));\
						outR += (uint32_t)(param.rA * (511.5 - 511.5 * cos(TAU * (param.rF * smooth + param.rP))));\
						outG += (uint32_t)(param.gA * (511.5 - 511.5 * cos(TAU * (param.gF * smooth + param.gP))));\
						outB += (uint32_t)(param.bA * (511.5 - 511.5 * cos(TAU * (param.bF * smooth + param.bP))));\
						break;\
					}\
				}\
				if (zs <= BREAKOUT) {\
					outR += 0;\
					outG += 0;\
					outB += (uint32_t)(511.5 - 511.5 * param.iA * cos(log(low) * param.iF + param.iP));\
				}\
				x++;\
			}\
			x -= sample;\
			y++;\
		}\
		y-= sample;\
		uint32_t div = sample * sample * 4;\
		outR /= div;\
		outG /= div;\
		outB /= div;\
		data[dataPtr] = outR; dataPtr++;\
		data[dataPtr] = outG; dataPtr++;\
		data[dataPtr] = outB; dataPtr++;\
		p0++;\
	}\
	x = 0;\
}

/* BOILERPLATE */

#define quadraticRender(fpX) \
	Block0(fpX)\
	fpX zr1, zr2, zi1, zi2, s1, s2, s3;\
	uint8_t f[8];\
	for (uint8_t q = 0; q < 8; q++) {\
		f[q] = ((param.formula >> q) & 1) ? 1 : 0;\
	}\
	\
	s1 = (f[0]) ? (fpX)-1.0 : (fpX)1.0;\
	s2 = (f[1]) ? (fpX)-1.0 : (fpX)1.0;\
	s3 = (f[2]) ? (fpX)-2.0 : (fpX)2.0;\
	\
	Block1(fpX)\
	zr1 = (f[3]) ? abs(zr) : zr;\
	zi1 = (f[4]) ? abs(zi) : zi;\
	zr2 = (f[5]) ? abs(zr) : zr;\
	zi2 = (f[6]) ? abs(zi) : zi;\
	\
	if (f[7] == 0) {\
		temp = s1 * ((zr1 * zr) - s2 * (zi1 * zi)) + cr;\
		zi = (zr2 * zi2 * s3) + ci;\
		zr = temp;\
	} else {\
		temp = s1 * abs((zr1 * zr) - s2 * (zi1 * zi)) + cr;\
		zi = (zr2 * zi2 * s3) + ci;\
		zr = temp;\
	}\
	\
	Block2(fpX,2.0)
/* quadraticRender(fpX) */
	
void quadraticRenderFP32(FractalParameters) { quadraticRender(fp32) }
void quadraticRenderFP64(FractalParameters) { quadraticRender(fp64) }
#ifdef enableFP80andFP128
	void quadraticRenderFP80(FractalParameters) { quadraticRender(fp80) }
	void quadraticRenderFP128(FractalParameters) { quadraticRender(fp128) }
#endif
	
#define cubicRender(fpX) \
	\
	Block0(fpX)\
	fpX zr1, zr2, zr3, zi1, zi2, zi3, s1, s2, s3, s4, s5, s6;\
	uint8_t f[14];\
	for (uint8_t q = 0; q < 14; q++) {\
		f[q] = ((param.formula >> q) & 1) ? 1 : 0;\
	}\
	s1 = (f[0]) ? (fpX)-1.0 : (fpX)1.0;\
	s2 = (f[1]) ? (fpX)-3.0 : (fpX)3.0;\
	s3 = (f[2]) ? (fpX)-3.0 : (fpX)3.0;\
	s4 = (f[3]) ? (fpX)-1.0 : (fpX)1.0;\
	s5 = (f[4]) ? (fpX)-1.0 : (fpX)1.0;\
	s6 = (f[5]) ? (fpX)-1.0 : (fpX)1.0;\
	\
	Block1(fpX)\
	zr1 = (f[6]) ? abs(zr) : zr;\
	zi1 = (f[7]) ? abs(zi) : zi;\
	zr2 = (f[8]) ? abs(zr) : zr;\
	zi2 = (f[9]) ? abs(zi) : zi;\
	zr3 = (f[10]) ? abs(zr) : zr;\
	zi3 = (f[11]) ? abs(zi) : zi;\
	\
	if (f[12] == 0) {\
		if (f[13] == 0) {\
		temp = s5 * ((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;\
		zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;\
		zr = temp;\
		} else {\
		temp = s5 * abs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;\
		zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;\
		zr = temp;\
		}\
	} else {\
		if (f[13] == 0) {\
		temp = s5 * abs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;\
		zi = s6 * ((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;\
		zr = temp;\
		} else {\
		temp = s5 * abs((s1 * zr1 * zr * zr) - (s2 * zr2 * zi1 * zi)) + cr;\
		zi = s6 * abs((s3 * zr3 * zr * zi2) - (s4 * zi3 * zi * zi)) + ci;\
		zr = temp;\
		}\
	}\
	\
	Block2(fpX,3.0)
/* cubicRender(fpX) */
	
void cubicRenderFP32(FractalParameters) { cubicRender(fp32) }
void cubicRenderFP64(FractalParameters) { cubicRender(fp64) }
#ifdef enableFP80andFP128
	void cubicRenderFP80(FractalParameters) { cubicRender(fp80) }
	void cubicRenderFP128(FractalParameters) { cubicRender(fp128) }
#endif
	
#define quarticRender(fpX) \
	Block0(fpX)\
	fpX zr1, zr2, zr3, zr4, zi1, zi2, zi3, zi4, s1, s2, s3, s4, s5, s6, s7;\
	uint8_t f[17];\
	for (uint8_t q = 0; q < 17; q++) {\
		f[q] = ((param.formula >> q) & 1) ? 1 : 0;\
	}\
	s1 = (f[0]) ? (fpX)-1.0 : (fpX)1.0;\
	s2 = (f[1]) ? (fpX)-6.0 : (fpX)6.0;\
	s3 = (f[2]) ? (fpX)-1.0 : (fpX)1.0;\
	s4 = (f[3]) ? (fpX)-4.0 : (fpX)4.0;\
	s5 = (f[4]) ? (fpX)-4.0 : (fpX)4.0;\
	s6 = (f[5]) ? (fpX)-1.0 : (fpX)1.0;\
	s7 = (f[6]) ? (fpX)-1.0 : (fpX)1.0;\
	\
	Block1(fpX)\
	zr1 = (f[7]) ? abs(zr) : zr;\
	zi1 = (f[8]) ? abs(zi) : zi;\
	zr2 = (f[9]) ? abs(zr) : zr;\
	zi2 = (f[10]) ? abs(zi) : zi;\
	zr3 = (f[11]) ? abs(zr) : zr;\
	zi3 = (f[12]) ? abs(zi) : zi;\
	zr4 = (f[13]) ? abs(zr) : zr;\
	zi4 = (f[14]) ? abs(zi) : zi;\
	\
	if (f[15] == 0) {\
		if (f[16] == 0) {\
		temp = s6 * (s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;\
		zi = s7 * (s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;\
		zr = temp;\
		} else {\
		temp = s6 * (s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;\
		zi = s7 * abs(s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;\
		zr = temp;\
		}\
	} else {\
		if (f[16] == 0) {\
		temp = s6 * abs(s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;\
		zi = s7 * (s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;\
		zr = temp;\
		} else {\
		temp = s6 * abs(s1 * (zr1 * zr * zr * zr) - s2 * (zr2 * zr * zi1 * zi) + s3 * (zi2 * zi * zi * zi)) + cr;\
		zi = s7 * abs(s4 * (zr3 * zr * zr * zi3) - s5 * (zr4 * zi4 * zi * zi)) + ci;\
		zr = temp;\
		}\
	}\
	\
	Block2(fpX,4.0)
/* quarticRender(fpX) */
	
void quarticRenderFP32(FractalParameters) { quarticRender(fp32) }
void quarticRenderFP64(FractalParameters) { quarticRender(fp64) }
#ifdef enableFP80andFP128
	void quarticRenderFP80(FractalParameters) { quarticRender(fp80) }
	void quarticRenderFP128(FractalParameters) { quarticRender(fp128) }
#endif
	
#define quinticRender(fpX) \
	Block0(fpX)\
	fpX zr1, zr2, zr3, zr4, zr5, zi1, zi2, zi3, zi4, zi5, s1, s2, s3, s4, s5, s6, s7, s8;\
	uint8_t fS[6];\
	uint8_t fA[10];\
	uint8_t fO[4];\
	for (uint8_t q = 0; q < 6; q++) { /* 0-5 */ \
		fS[q] = ((param.formula >> q) & 1) ? 1 : 0;\
	}\
	for (uint8_t q = 6; q < 8; q++) { /* 6-7 */ \
		fO[q - 6] = ((param.formula >> q) & 1) ? 1 : 0;\
	}\
	for (uint8_t q = 8; q < 18; q++) { /* 8-17 */ \
		fA[q - 8] = ((param.formula >> q) & 1) ? 1 : 0;\
	}\
	for (uint8_t q = 18; q < 20; q++) { /* 18-19 */ \
		fO[q - 16] = ((param.formula >> q) & 1) ? 1 : 0;\
	}\
	s1 = (fS[0]) ? (fpX)-1.0 : (fpX)1.0;\
	s2 = (fS[1]) ? (fpX)-10.0 : (fpX)10.0;\
	s3 = (fS[2]) ? (fpX)-5.0 : (fpX)5.0;\
	s4 = (fS[3]) ? (fpX)-5.0 : (fpX)5.0;\
	s5 = (fS[4]) ? (fpX)-10.0 : (fpX)10.0;\
	s6 = (fS[5]) ? (fpX)-1.0 : (fpX)1.0;\
	s7 = (fO[0]) ? (fpX)-1.0 : (fpX)1.0;\
	s8 = (fO[1]) ? (fpX)-1.0 : (fpX)1.0;\
	\
	Block1(fpX)\
	zr1 = (fA[0]) ? abs(zr) : zr;\
	zi1 = (fA[1]) ? abs(zi) : zi;\
	zr2 = (fA[2]) ? abs(zr) : zr;\
	zi2 = (fA[3]) ? abs(zi) : zi;\
	zr3 = (fA[4]) ? abs(zr) : zr;\
	zi3 = (fA[5]) ? abs(zi) : zi;\
	zr4 = (fA[6]) ? abs(zr) : zr;\
	zi4 = (fA[7]) ? abs(zi) : zi;\
	zr5 = (fA[8]) ? abs(zr) : zr;\
	zi5 = (fA[9]) ? abs(zi) : zi;\
	\
	if (fO[2] == 0) {\
		if (fO[3] == 0) {\
			temp = s7 * (s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;\
			zi = s8 * (s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;\
			zr = temp;\
		} else {\
			temp = s7 * (s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;\
			zi = s8 * abs(s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;\
			zr = temp;\
		}\
	} else {\
		if (fO[3] == 0) {\
			temp = s7 * abs(s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;\
			zi = s8 * (s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;\
			zr = temp;\
		} else {\
			temp = s7 * abs(s1 * (zr1 * zr * zr * zr * zr) - s2 * (zr2 * zr * zr * zi1 * zi) + s3 * (zr3 * zi2 * zi * zi * zi)) + cr;\
			zi = s8 * abs(s4 * (zr4 * zr * zr * zr * zi3) - s5 * (zr5 * zr * zi4 * zi * zi) + s6 * (zi5 * zi * zi * zi * zi)) + ci;\
			zr = temp;\
		}\
	}\
	\
	Block2(fpX,5.0)
/* quinticRender(fpX) */
	
void quinticRenderFP32(FractalParameters) { quinticRender(fp32) }
void quinticRenderFP64(FractalParameters) { quinticRender(fp64) }
#ifdef enableFP80andFP128
	void quinticRenderFP80(FractalParameters) { quinticRender(fp80) }
	void quinticRenderFP128(FractalParameters) { quinticRender(fp128) }
#endif

void renderRow(ABS_Mandelbrot* param, uint8_t* data, uint32_t resX, uint32_t resY, uint8_t subSample, uint32_t p0, uint32_t p1) { // Deprecated
	u32 dataPtr = p0 * 3; // RGB
	u32 maxItr = param->maxItr;
	fp64 typeC = (fp64)param->power;
	
	fp64 r = param->r;
	fp64 i = param->i;
	fp64 zoom = param->zoom;
	
	u32 y = p0 / resX;
	u32 x = p0 % resX;
	
	for (; y < resY; y++) {
		fp64 ci = -((((fp64)y - (((fp64)resY - 1.0) / 2.0)) / (((fp64)resY - 1.0) / 2.0)) / pow(10.0, zoom)) + i;
		for (; x < resX; x++) {
			if (p0 == p1) {
				return;
			}
			fp64 cr = ((((fp64)x - (((fp64)resX - 1.0) / 2.0)) / (((fp64)resY - 1.0) / 2.0)) / pow(10.0, zoom)) + r;
			
			fp64 zr = param->zr; // Default 0.0
			fp64 zi = param->zi; // Default 0.0
			fp64 low = 4.0; // Squared
			fp64 temp;
			fp64 zs = 0.0;
			
			for (u32 itr = 0; itr < maxItr; itr++) {

				temp = zr * zr - abs(zi) * zi + cr;
				zi = zr * zi * 2 + ci;
				zr = temp;
				zs = zr * zr + zi * zi;
				
				if (zs < low) {
					low = zs;
				} else if (zs > BREAKOUT) {
					fp64 smooth = log(1.0 + fmax(0.0, (fp64)itr - log2(log2(zs) / 2.0) / log2(typeC))); //log2(sqrt(zs)) / log2(base)
					// data[dataPtr] = (uint8_t)floor(0.9 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.5)))); dataPtr++;
					// data[dataPtr] = (uint8_t)floor(1.0 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.9)))); dataPtr++;
					// data[dataPtr] = (uint8_t)floor(1.0 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.1)))); dataPtr++;
					data[dataPtr] = (uint8_t)floor(1.0 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.5)))); dataPtr++; data[dataPtr] = (uint8_t)floor(1.0 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.5)))); dataPtr++; data[dataPtr] = (uint8_t)floor(1.0 * (127.5 - 127.5 * cos(TAU * (0.45 * smooth + 0.5)))); dataPtr++;
					break;
				}

			}
			if (zs <= BREAKOUT) {
				//data[dataPtr] = 0; dataPtr++;
				//data[dataPtr] = 0; dataPtr++;
				data[dataPtr] = (uint8_t)floor(127.5 - 127.5 * cos(log(low) / 2.0)); dataPtr++;
				data[dataPtr] = (uint8_t)floor(127.5 - 127.5 * cos(log(low) / 2.0)); dataPtr++;
				data[dataPtr] = (uint8_t)floor(127.5 - 127.5 * cos(log(low) / 2.0)); dataPtr++;
			}
			p0++;
		}
		x = 0;
	}
	//y = 0;
}

void renderCPU_ABS_Mandelbrot(BufferBox* buf, Render_Data ren, ABS_Mandelbrot param, std::atomic<bool>& ABORT_RENDERING, uint32_t tc) {
	if (validateBufferBox(buf) == false) {
		printError("BufferBox* buf is NULL or has invalid data in renderCPU_ABS_Mandelbrot()");
		return;
	}
	uint32_t resX = buf->resX * ren.sample;
	uint32_t resY =	buf->resY * ren.sample;
	u32 dataPtr = 0;
	std::vector<std::thread> renderThread;
	/* Thread Creation */
		#define makeThread(k) renderThread.push_back(std::thread(k, buf, ren, param, p0, p1, std::ref(ABORT_RENDERING)))
		#define generateThreads(k) \
		for (u32 t = 0; t < tc; t++) { \
			u32 p0 = ((buf->resX * buf->resY) * t) / tc;\
			u32 p1 = ((buf->resX * buf->resY) * (t + 1)) / tc; \
			makeThread(k); \
		}
	/* Thread Creation */

	printfInterval(0.3,"\nr: %.6lf i: %.6lf zoom: 10^%.4lf maxItr: %u",param.r,param.i,param.zoom,param.maxItr);

	// Default is FP64
	if (param.power == 2) {
		if (ren.CPU_Precision == 32) { generateThreads(quadraticRenderFP32); } else
		#ifdef enableFP80andFP128
			if (ren.CPU_Precision == 80) { generateThreads(quadraticRenderFP80); } else
			if (ren.CPU_Precision == 128) { generateThreads(quadraticRenderFP128); } else
		#endif
		{ generateThreads(quadraticRenderFP64); }
	} else if (param.power == 3) {
		if (ren.CPU_Precision == 32) { generateThreads(cubicRenderFP32); } else
		#ifdef enableFP80andFP128
			if (ren.CPU_Precision == 80) { generateThreads(cubicRenderFP80); } else
			if (ren.CPU_Precision == 128) { generateThreads(cubicRenderFP128); } else
		#endif
		{ generateThreads(cubicRenderFP64); }
	} else if (param.power == 4) {
		if (ren.CPU_Precision == 32) { generateThreads(quarticRenderFP32); } else
		#ifdef enableFP80andFP128
			if (ren.CPU_Precision == 80) { generateThreads(quarticRenderFP80); } else
			if (ren.CPU_Precision == 128) { generateThreads(quarticRenderFP128); } else
		#endif
		{ generateThreads(quarticRenderFP64); }
	} else if (param.power == 5) {
		if (ren.CPU_Precision == 32) { generateThreads(quinticRenderFP32); } else
		#ifdef enableFP80andFP128
			if (ren.CPU_Precision == 80) { generateThreads(quinticRenderFP80); } else
			if (ren.CPU_Precision == 128) { generateThreads(quinticRenderFP128); } else
		#endif
		{ generateThreads(quinticRenderFP64); }
	} else {
		printfInterval(0.5,"\nError: Unknown render parameters\nPower: %u CPU_Precision: %u",param.power,ren.CPU_Precision);
		return;
		//generateThreads(renderRow); //Original Method
	}
	for (u32 t = 0; t < tc; t++) {
		renderThread.at(t).join();
	}
	if (ABORT_RENDERING == true) {
		printFlush("\nAborted %u threads",tc);
	}
}