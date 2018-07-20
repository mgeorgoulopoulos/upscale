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

#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stdint.h>
#include <vector>
#include <algorithm>

// define our pixels as RGBA 32 bit
typedef uint32_t pixel;
typedef uint32_t color;

// Define our pixel component as 8 bit unsigned integer
typedef uint8_t comp;

// define a larger component on which we can perform math
typedef int lcomp;

inline lcomp compToLcomp(comp c) {
	return (lcomp)c;
}

inline comp lcompToComp(lcomp l) {
	return (comp)std::max(0, std::min(255, (int)l));
}

inline comp R(pixel p) {
	return (p >> 24) & 0xFF;
}

inline comp G(pixel p) {
	return (p >> 16) & 0xFF;
}

inline comp B(pixel p) {
	return (p >> 8) & 0xFF;
}

inline pixel RGB(comp r, comp g, comp b) {
	return (r << 24) | (g << 16) | (b << 8);
}

inline int L(pixel p) {
	return R(p) + G(p) + B(p);
}

class ImageChannels;

class Image {
public:
	Image();
	Image(const ImageChannels &imgc);
	virtual ~Image();

	std::vector<pixel> pixels;

	void setW(int newW);
	void setH(int newH);

	void setSize(int newW, int newH);

	int w() const { return mW; }
	int h() const { return mH; }

	/*!
	\brief Creates a black image of the specified dimensions.
	*/
	void create(int w, int h);

	/*!
	\brief Clears with the given color.
	*/
	void clear(color c);

	/*!
	\brief Will read all available pixels from other.
	*/
	void copyFrom(const Image &other);

	/*!
	Checks bounds and returns black if outside image bounds.
	*/
	color getPixel(int x, int y) const;

	/*!
	Does bound checking.
	*/
	void setPixel(int x, int y, color p);

private:
	int mW, mH;
};

#endif // ndef __IMAGE_H__