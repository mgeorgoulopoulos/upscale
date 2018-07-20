/*
 * Copyright(c) 2018 Michael Georgoulopoulos
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "ScalerOCV.h"

#include "../common/Image.h"

using namespace cv;
using namespace std;

static color Vec3ToColor(Vec3b v) {
	return RGB(v[2], v[1], v[0]);
}

static Vec3b ColorToVec3(color c) {
	Vec3b ret;
	ret[2] = R(c);
	ret[1] = G(c);
	ret[0] = B(c);
	return ret;
}

static Err matToImage(const Mat &mat, Image *image) {
	Err e = Err::Success;

	image->create(mat.cols, mat.rows);

	// read all pixels into the output image
	for (int i = 0; i < mat.rows; i++) {
		for (int j = 0; j < mat.cols; j++) {
			Vec3b p = mat.at<Vec3b>(i, j);
			color imgPixel = Vec3ToColor(p);
			image->pixels.at(j + image->w() * i) = imgPixel;
		}
	}

	return e;
}

static Err imageToMat(const Image &image, Mat *mat) {
	Err e = Err::Success;

	// Create an openCV image from ours
	mat->create(image.h(), image.w(), CV_8UC3);

	// Transfer all pixels
	for (int j = 0; j < image.h(); j++) {
		for (int i = 0; i < image.w(); i++) {
			color p = image.getPixel(i, j);
			Vec3b ocvImagePixel = ColorToVec3(p);
			mat->at<Vec3b>(j, i) = ocvImagePixel;
		}
	}

	return e;
}


ScalerOCV::ScalerOCV(ScalerOCV::Filter filter) {
	switch (filter) {
	case Filter::Nearest:
		mFilter = INTER_NEAREST;
		break;
	case Filter::Linear:
		mFilter = INTER_LINEAR;
		break;
	case Filter::Cubic:
		mFilter = INTER_CUBIC;
		break;
	case Filter::Lanczos:
		mFilter = INTER_LANCZOS4;
		break;
	}
}

ScalerOCV::~ScalerOCV() {

}

Err ScalerOCV::scale(const Image &src, int dstW, int dstH, Image *dst) {
	Err e = Err::Success;

	Mat mat;
	e = imageToMat(src, &mat); ree;

	Mat dstMat;
	resize(mat, dstMat, Size(dstW, dstH), 0, 0, mFilter);

	e = matToImage(dstMat, dst); ree;

	return e;
}

