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

#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "LoaderOCV.h"

using namespace cv;
using namespace std;

LoaderOCV::LoaderOCV() {

}

LoaderOCV::~LoaderOCV() {
	
}

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

Err LoaderOCV::load(const char *filename, Image *img) const {
	Err e = Err::Success;

	// Read the file
	Mat mat;
	mat = imread(filename, CV_LOAD_IMAGE_COLOR); 

	// Check for invalid input
	if (!mat.data) {
		std::cout << "Could not open or find the image: " << filename << std::endl;
		return Err::NotFound;
	}

	// Make sure this is an rgb image
	if (mat.channels() != 3) {
		cout << filename << ": Unsupported channel count: " << mat.channels() << endl;
		return Err::Error;
	}

	e = matToImage(mat, img); ree;
	

	return e;
}

Err LoaderOCV::save(const char *filename, const Image &img) const {
	Err e = Err::Success;

	Mat  ocvImage;
	e = imageToMat(img, &ocvImage); ree;

	if (false == imwrite(filename, ocvImage)) {
		return Err::Error;
	}

	return e;
}