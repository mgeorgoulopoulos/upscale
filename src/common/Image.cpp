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

#include <algorithm>

#include "Image.h"
#include "ImageChannels.h"

Image::Image() : mW(0), mH(0) {

}

Image::Image(const ImageChannels &imgc) {
	mW = imgc.w();
	mH = imgc.h();
	pixels.resize(imgc.red.size());

	for (int i = 0; i < pixels.size(); i++) {
		comp r = lcompToComp(imgc.red[i]);
		comp g = lcompToComp(imgc.green[i]);
		comp b = lcompToComp(imgc.blue[i]);
		pixels[i] = RGB(r, g, b);
	}
}

Image::~Image() {

}

void Image::create(int w, int h) {
	mW = w;
	mH = h;
	pixels.resize(mW * mH);
	clear(0);
}

void Image::clear(color c) {
	for (int j = 0; j < mH; j++) {
		for (int i = 0; i < mW; i++) {
			pixels.at(i + mW * j) = c;
		}
	}
}

void Image::setW(int newW) {
	// tmp copy
	Image tmp = *this;
	mW = newW;
	pixels.resize(w() * h());
	copyFrom(tmp);
}

void Image::setH(int newH) {
	// tmp copy
	Image tmp = *this;
	mH = newH;
	pixels.resize(w() * h());
	copyFrom(tmp);
}

void Image::setSize(int newW, int newH) {
	// tmp copy
	Image tmp = *this;
	mW = newW;
	mH = newH;
	pixels.resize(w() * h());
	copyFrom(tmp);
}

void Image::copyFrom(const Image &other) {
	if (other.w() * other.h() <= 0) {
		return;
	}
	for (int j = 0; j < h(); j++) {
		for (int i = 0; i < w(); i++) {
			setPixel(i, j, other.getPixel(i, j));
		}
	}
}

color Image::getPixel(int x, int y) const {
	// clamp coordinates to border
	x = std::max(0, std::min(w() - 1, x));
	y = std::max(0, std::min(h() - 1, y));
	return pixels.at(x + w() * y);
}

void Image::setPixel(int x, int y, color p) {
	if (x < 0 || x >= w() || y < 0 || y >= h()) return;
	pixels.at(x + w() * y) = p;
}