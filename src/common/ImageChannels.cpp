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
#include "ImageChannels.h"

ImageChannels::ImageChannels(int w, int h, lcomp v) : mW(w), mH(h) {
	red.resize(w * h);
	green.resize(w * h);
	blue.resize(w * h);
	*this = v;
}

ImageChannels::ImageChannels(const Image &img) {
	mW = img.w();
	mH = img.h();

	red.resize(img.pixels.size());
	green.resize(img.pixels.size());
	blue.resize(img.pixels.size());

	for (int i = 0; i < (int)red.size(); i++) {
		red[i] = compToLcomp(R(img.pixels[i]));
		green[i] = compToLcomp(G(img.pixels[i]));
		blue[i] = compToLcomp(B(img.pixels[i]));
	}
}

ImageChannels::ImageChannels(const ImageChannels &img) {
	mW = img.w();
	mH = img.h();
	red = img.red;
	blue = img.blue;
	green = img.green;
}

ImageChannels::~ImageChannels() {

}

