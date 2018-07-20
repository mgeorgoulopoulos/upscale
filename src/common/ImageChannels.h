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

#ifndef __IMAGE_CHANNELS_H__
#define __IMAGE_CHANNELS_H__

#include "image.h"

class ImageChannels {
public:
	ImageChannels(int w, int h, lcomp v = 0);
	ImageChannels(const Image &img);
	ImageChannels(const ImageChannels &img);
	virtual ~ImageChannels();

	std::vector<lcomp> red, green, blue;

public:
	int w() const { return mW; }
	int h() const { return mH; }

	void operator += (lcomp v) {
		for (int i = 0; i < (int)red.size(); i++) {
			green[i] += v;
			blue[i] += v;
			red[i] += v;
		}
	}

	void operator -= (lcomp  v) {
		*this += -v;
	}

	void operator *= (lcomp v) {
		for (int i = 0; i < (int)red.size(); i++) {
			green[i] *= v;
			blue[i] *= v;
			red[i] *= v;
		}
	}

	void operator /= (lcomp v) {
		for (int i = 0; i < (int)red.size(); i++) {
			green[i] /= v;
			blue[i] /= v;
			red[i] /= v;
		}
	}

	void operator = (lcomp v) {
		for (int i = 0; i < (int)red.size(); i++) {
			green[i] = v;
			blue[i] = v;
			red[i] = v;
		}
	}

private:
	int mW, mH;
};

inline ImageChannels operator + (const ImageChannels &a, const ImageChannels &b) {
	ImageChannels ret(a);
	if (a.red.size() != b.red.size()) {
		return ret;
	}

	for (int i = 0; i < (int)a.red.size(); i++) {
		ret.red[i] = a.red[i] + b.red[i];
		ret.green[i] = a.green[i] + b.green[i];
		ret.blue[i] = a.blue[i] + b.blue[i];
	}

	return ret;
}

inline ImageChannels operator - (const ImageChannels &a, const ImageChannels &b) {
	ImageChannels ret(a);
	if (a.red.size() != b.red.size()) {
		return ret;
	}

	for (int i = 0; i < (int)a.red.size(); i++) {
		ret.red[i] = a.red[i] - b.red[i];
		ret.green[i] = a.green[i] - b.green[i];
		ret.blue[i] = a.blue[i] - b.blue[i];
	}

	return ret;
}

#endif // ndef __IMAGE_CHANNELS_H__