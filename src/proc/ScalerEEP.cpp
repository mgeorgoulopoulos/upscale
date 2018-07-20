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

#include "ScalerEEP.h"

#include "../common/common.h"

static inline int pixelDiff(pixel a, pixel b) {
	int ret = 0;

	ret += abs(R(a) - R(b));
	ret += abs(G(a) - G(b));
	ret += abs(B(a) - B(b));

	return ret;
}

static inline int pixelAverage(pixel x, pixel y) {
	comp r = (R(x) + R(y)) >> 1;
	comp g = (G(x) + G(y)) >> 1;
	comp b = (B(x) + B(y)) >> 1;
	return RGB(r, g, b);
}

static inline int pixelAverageWeighed(pixel x, pixel y, int w) {
	int omw = 256 - w;

	comp r = (R(x) * w + R(y) * omw) >> 8;
	comp g = (G(x) * w + G(y) * omw) >> 8;
	comp b = (B(x) * w + B(y) * omw) >> 8;
	return RGB(r, g, b);
}

ScalerEEP::ScalerEEP() {

}

ScalerEEP::~ScalerEEP() {

}


Err ScalerEEP::scale2x(const Image &src, Image *dst) {
	Err e = Err::Success;

	dst->setSize(src.w() * 2, src.h() * 2);

	// fill all destination pixels
	for (int j = 1; j < dst->h(); j++) {
		for (int i = 1; i < dst->w(); i++) {
			int category = ((i & 1) << 1) | (j & 1);

			pixel p;

			switch (category) {
			case 0:
				// 00 : even-even
				p = sampleEvenEven(src, i, j);
				break;
			case 1:
				// 01 : even-odd
				p = sampleEvenOdd(src, i, j);
				break;
			case 2:
				// 10 : odd-even
				p = sampleOddEven(src, i, j);
				break;
			case 3:
				// 11 : odd-odd
				p = sampleOddOdd(src, i, j);
				break;
			}

			dst->setPixel(i, j, p);
		}
	}

	return e;
}

pixel ScalerEEP::sampleEvenEven(const Image &src, int i, int j) const {
	return src.getPixel(i/2, j/2);
}

// This gives 4x the weight to the closer pixels and 1/4th the weight to the other pixels having twice the distance.
static const int evenOddWeight = 205;

pixel ScalerEEP::sampleEvenOdd(const Image &src, int i, int j) const {
	int srcI = i / 2;
	int srcJ = j / 2;

	pixel a = pixelAverage(src.getPixel(srcI, srcJ), src.getPixel(srcI, srcJ + 1));
	pixel b1 = pixelAverage(src.getPixel(srcI - 1, srcJ), src.getPixel(srcI - 1, srcJ + 1));
	pixel b2 = pixelAverage(src.getPixel(srcI + 1, srcJ), src.getPixel(srcI + 1, srcJ + 1));
	pixel b = pixelAverage(b1, b2);

	return pixelAverageWeighed(a, b, evenOddWeight);
}

pixel ScalerEEP::sampleOddEven(const Image &src, int i, int j) const {
	int srcI = i / 2;
	int srcJ = j / 2;

	pixel a = pixelAverage(src.getPixel(srcI, srcJ), src.getPixel(srcI+1, srcJ));
	pixel b1 = pixelAverage(src.getPixel(srcI, srcJ - 1), src.getPixel(srcI + 1, srcJ - 1));
	pixel b2 = pixelAverage(src.getPixel(srcI, srcJ + 1), src.getPixel(srcI + 1, srcJ + 1));
	pixel b = pixelAverage(b1, b2);

	return pixelAverageWeighed(a, b, evenOddWeight);
}

pixel ScalerEEP::sampleOddOdd(const Image &src, int i, int j) const {
	// Get the diagonal neighbours
	// A   B
	//   X  
	// C   D

	int ai = i / 2;
	int aj = j / 2;

	pixel a = src.getPixel(ai, aj);
	pixel b = src.getPixel(ai + 1, aj);
	pixel c = src.getPixel(ai, aj + 1);
	pixel d = src.getPixel(ai + 1, aj + 1);

	// calculate diffs
	int diffAD = pixelDiff(a, d);
	int diffBC = pixelDiff(b, c);

	if (diffAD == diffBC && diffAD == 0) {
		// all pixels are the same
		return a;
	}

	// calculate averages
	pixel avgAD = pixelAverage(a, d);
	pixel avgBC = pixelAverage(b, c);

	// if one diagonal has zero diff, use that
	if (diffAD == 0) {
		return avgAD;
	}
	if (diffBC == 0) {
		return avgBC;
	}

	int smallDiff, largeDiff;
	pixel smallDiffAvg, largeDiffAvg;

	if (diffAD < diffBC) {
		smallDiff = diffAD;
		largeDiff = diffBC;
		smallDiffAvg = avgAD;
		largeDiffAvg = avgBC;
	} else {
		smallDiff = diffBC;
		largeDiff = diffAD;
		smallDiffAvg = avgBC;
		largeDiffAvg = avgAD;
	}
	
	// calculate weight W
	float W = 1.0f / (1.0f + ((float)largeDiff / (float)smallDiff));

	// convert W to [0, 256] range as an optimization
	int wi = (comp) std::max(0.0f, std::min(256.0f, (W * 256.0f)));

	return pixelAverageWeighed(largeDiffAvg, smallDiffAvg, wi);
}