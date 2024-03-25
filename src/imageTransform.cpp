/*
**	Author: zerico2005 (2023-2024)
**	Project: ABS-Fractal-Explorer
**	License: MIT License
**	A copy of the MIT License should be included with
**	this project. If not, see https://opensource.org/license/MIT
*/

#include "Common_Def.h"
#include "Program_Def.h"
#include "copyBuffer.h"
#include "imageBuffer.h"
#include "render.h"
#include <opencv2/opencv.hpp>

int export_OpenCV_Render(BufferBox* buf, const cv::Mat& Mat_Render, uint32_t backgroundColor, BufferBox* backgroundImage, BufferBox* foregroundImage) {
	
	initBufferBox(buf,nullptr,Mat_Render.cols,Mat_Render.rows,Mat_Render.channels());
	buf->vram = (uint8_t*)malloc(Mat_Render.total() * Mat_Render.elemSize());
	if (buf->vram == nullptr) {
		return -1;
	}
	memcpy(buf->vram, Mat_Render.data, Mat_Render.total() * Mat_Render.elemSize());
	return 0;
}

cv::Mat Image_Place_Parallelogram(
	ImageBuffer* img, Render_Data* ren,
	int interpolation_mode,
	fp32 sx00, fp32 sy00,
	fp32 sx01, fp32 sy01, fp32 sx10, fp32 sy10,
	fp32 dx00, fp32 dy00,
	fp32 dx01, fp32 dy01, fp32 dx10, fp32 dy10
) {
	cv::Mat Mat_Image(img->resY, img->resX, CV_8UC(img->channels), img->vram);
	cv::Point2f srcCord[] = {
		cv::Point2f(sx00, sy00),
		cv::Point2f(sx10, sy10),
		cv::Point2f(sx01, sy01)
	};
	cv::Point2f dstCord[] = {
		cv::Point2f(dx00, dy00),
		cv::Point2f(dx10, dy10),
		cv::Point2f(dx01, dy01)
	};
	// printfInterval(0.5,
	// 	"\n{%7.2f,%7.2f} --- {%7.2f,%7.2f}"
	// 	"\n{%7.2f,%7.2f} --- {%7.2f,%7.2f}\n",
	// 	sx00,sy00,sx10,sy10,
	// 	sx01,sy01,sx10,sy01
	// );
	cv::Mat projectionMatrix = cv::getAffineTransform(srcCord, dstCord);
 	//cv::Mat Mat_Render(img->resY * ren->subSample, img->resX * ren->subSample, CV_8UC(img->channels));
	cv::Mat Mat_Render(ren->resY, ren->resX, CV_8UC(img->channels));
	cv::warpAffine(Mat_Image, Mat_Render, projectionMatrix, Mat_Render.size(),interpolation_mode);
	return Mat_Render;
}

cv::Mat Image_Place_Quadrilateral(
	ImageBuffer* img, Render_Data* ren,
	int interpolation_mode,
	fp32 sx00, fp32 sy00, fp32 sx11, fp32 sy11,
	fp32 sx01, fp32 sy01, fp32 sx10, fp32 sy10,
	fp32 dx00, fp32 dy00, fp32 dx11, fp32 dy11,
	fp32 dx01, fp32 dy01, fp32 dx10, fp32 dy10
) {
	cv::Mat Mat_Image(img->resY, img->resX, CV_8UC(img->channels), img->vram);
	cv::Point2f srcCord[] = {
		cv::Point2f(sx00, sy00),
		cv::Point2f(sx10, sy10),
		cv::Point2f(sx11, sy11),
		cv::Point2f(sx01, sy01)
	};
	cv::Point2f dstCord[] = {
		cv::Point2f(dx00, dy00),
		cv::Point2f(dx10, dy10),
		cv::Point2f(dx11, dy11),
		cv::Point2f(dx01, dy01)
	};
	cv::Mat projectionMatrix = cv::getPerspectiveTransform(srcCord, dstCord);
	cv::Mat Mat_Render(img->resY * ren->subSample, img->resX * ren->subSample, CV_8UC(img->channels));
	cv::warpPerspective(Mat_Image, Mat_Render, projectionMatrix, Mat_Render.size(),interpolation_mode);
	return Mat_Render;
}

int Image_Scaler_Parallelogram(
	BufferBox* buf, ImageBuffer* img, Render_Data* ren,
	uint32_t backgroundColor,
	BufferBox* backgroundImage, BufferBox* foregroundImage,
	int interpolation_mode,
	fp32 sx00, fp32 sy00,
	fp32 sx01, fp32 sy01, fp32 sx10, fp32 sy10,
	fp32 dx00, fp32 dy00,
	fp32 dx01, fp32 dy01, fp32 dx10, fp32 dy10
) {
	if (buf == nullptr || img == nullptr || img->vram == nullptr || img->allocated() == false || ren == nullptr) {
		return -1;
	}
	cv::Mat Mat_Render = Image_Place_Parallelogram(
		img,ren,
		interpolation_mode,
		sx00,sy00,
		sx01,sy01,sx10,sy10,
		dx00,dy00,
		dx01,dy01,dx10,dy10
	);
	if (export_OpenCV_Render(buf,Mat_Render,backgroundColor,backgroundImage,foregroundImage) == -1) {
		return -1;
	}
	return 0;
}

int Image_Scaler_Quadrilateral(
	BufferBox* buf, ImageBuffer* img, Render_Data* ren,
	uint32_t backgroundColor,
	BufferBox* backgroundImage, BufferBox* foregroundImage,
	int interpolation_mode,
	fp32 sx00, fp32 sy00, fp32 sx11, fp32 sy11,
	fp32 sx01, fp32 sy01, fp32 sx10, fp32 sy10,
	fp32 dx00, fp32 dy00, fp32 dx11, fp32 dy11,
	fp32 dx01, fp32 dy01, fp32 dx10, fp32 dy10
) {
	if (buf == nullptr || img == nullptr || img->vram == nullptr || img->allocated() == false || ren == nullptr) {
		return -1;
	}
	cv::Mat Mat_Render = Image_Place_Quadrilateral(
		img,ren,
		interpolation_mode,
		sx00,sy00,sx11,sy11,
		sx01,sy01,sx10,sy10,
		dx00,dy00,dx11,dy11,
		dx01,dy01,dx10,dy10
	);
	if (export_OpenCV_Render(buf,Mat_Render,backgroundColor,backgroundImage,foregroundImage) == -1) {
		return -1;
	}
	return 0;
}
