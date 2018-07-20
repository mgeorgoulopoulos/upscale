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

#include "ScalerSelfSim2x.h"

#include "../common/common.h"

#define PATCH_SIZE 5
#define PATCH_SEARCH_SIZE 11

namespace {
	class Patch {

	};
}



ScalerSelfSim2x::ScalerSelfSim2x() {

}

ScalerSelfSim2x::~ScalerSelfSim2x() {

}


static lcomp diffPatch(const ImageChannels &small, int smallPatchX, int smallPatchY,
	const ImageChannels &large, int largePatchX, int largePatchY) {

	int stride0 = small.w();
	int pixel0X = smallPatchX - PATCH_SIZE / 2;
	int pixel0Y = smallPatchY - PATCH_SIZE / 2;
	int pixel0 = pixel0X + stride0 * pixel0Y;
	const lcomp *r0 = &small.red[pixel0];
	const lcomp *g0 = &small.green[pixel0];
	const lcomp *b0 = &small.blue[pixel0];

	int stride1 = large.w();
	int pixel1X = largePatchX - PATCH_SIZE / 2;
	int pixel1Y = largePatchY - PATCH_SIZE / 2;
	int pixel1 = pixel1X + stride1 * pixel1Y;
	const lcomp *r1 = &large.red[pixel1];
	const lcomp *g1 = &large.green[pixel1];
	const lcomp *b1 = &large.blue[pixel1];

	lcomp ret = 0;
	for (int j = 0; j < PATCH_SIZE; j++) {
		for (int i = 0; i < PATCH_SIZE; i++) {
			lcomp dr = *r1 - *r0;
			lcomp dg = *g1 - *g0;
			lcomp db = *b1 - *b0;
			//ret += dr * dr + dg * dg + db * db;
			ret += abs(dr) + abs(dg) + abs(db);

			r0++; g0++; b0++;
			r1++; g1++; b1++;
		}

		r0 += stride0 - PATCH_SIZE;
		g0 += stride0 - PATCH_SIZE;
		b0 += stride0 - PATCH_SIZE;

		r1 += stride1 - PATCH_SIZE;
		g1 += stride1 - PATCH_SIZE;
		b1 += stride1 - PATCH_SIZE;
	}

	return ret;
}

static Err locateBestPatch(const ImageChannels &small, const ImageChannels &large,
	int largePatchX, int largePatchY, int *bestX, int *bestY) {
	Err e = Err::Success;

	// first, project large image patch to small image
	int smallPatchX = largePatchX / 2;
	int smallPatchY = largePatchY / 2;

	// calculate how much freedom the search has
	int freedom = (PATCH_SEARCH_SIZE - (PATCH_SEARCH_SIZE / 2)) - (PATCH_SIZE - (PATCH_SIZE / 2));

	// define search boundaries
	int searchStartX = smallPatchX - freedom;
	int searchStartY = smallPatchY - freedom;

	// search
	bool first = true;
	lcomp minDiff;
	for (int i = searchStartX; i < searchStartX + 2 * freedom; i++) {
		if (i - PATCH_SIZE / 2 < 0) {
			continue;
		}
		if (i + PATCH_SIZE / 2 >= small.w()) {
			continue;
		}
		for (int j = searchStartY; j < searchStartY + 2 * freedom; j++) {
			if (j - PATCH_SIZE / 2 < 0) {
				continue;
			}
			if (j + PATCH_SIZE / 2 >= small.h()) {
				continue;
			}

			lcomp diff = diffPatch(small, i, j, large, largePatchX, largePatchY);

			if (first || diff < minDiff) {
				first = false;
				minDiff = diff;
				*bestX = i;
				*bestY = j;
			}
		}
	}

	return e;
}

static Err applyPatch(const ImageChannels &small, int smallPatchX, int smallPatchY,
	ImageChannels *large, int largePatchX, int largePatchY) {

	Err e = Err::Success;

	int stride0 = small.w();
	int pixel0X = smallPatchX - PATCH_SIZE / 2;
	int pixel0Y = smallPatchY - PATCH_SIZE / 2;
	int pixel0 = pixel0X + stride0 * pixel0Y;
	const lcomp *r0 = &small.red[pixel0];
	const lcomp *g0 = &small.green[pixel0];
	const lcomp *b0 = &small.blue[pixel0];

	int stride1 = large->w();
	int pixel1X = largePatchX - PATCH_SIZE / 2;
	int pixel1Y = largePatchY - PATCH_SIZE / 2;
	int pixel1 = pixel1X + stride1 * pixel1Y;
	lcomp *r1 = &large->red[pixel1];
	lcomp *g1 = &large->green[pixel1];
	lcomp *b1 = &large->blue[pixel1];

	for (int j = 0; j < PATCH_SIZE; j++) {
		for (int i = 0; i < PATCH_SIZE; i++) {
			*r1 += *r0;
			*g1 += *g0;
			*b1 += *b0;

			r0++; g0++; b0++;
			r1++; g1++; b1++;
		}

		r0 += stride0 - PATCH_SIZE;
		g0 += stride0 - PATCH_SIZE;
		b0 += stride0 - PATCH_SIZE;
		r1 += stride1 - PATCH_SIZE;
		g1 += stride1 - PATCH_SIZE;
		b1 += stride1 - PATCH_SIZE;
	}

	return e;
}

Err ScalerSelfSim2x::scale2x(const Image &src, Image *dst) {
	Err e = Err::Success;

	// First, split input image into low and high frequency sub-imags
	Image tmp1, tmp2, low, high;

	// low-freq is downscaled and then upscaled again
	e = scaleLinear(src, src.w() / 2, src.h() / 2, &tmp1); ree;
	e = scaleLinear(tmp1, src.w(), src.h(), &low); ree;

	// Break to channels to process easily
	ImageChannels srcC(src);
	ImageChannels lowC(low);
	ImageChannels highC(srcC - lowC);

	// Now, upsample. This will produce a larger but blurry picture
	// (Picture with only lower part of the frequency band).
	Image largeLow;
	e = scaleLinear(src, src.w() * 2, src.h() * 2, &largeLow); ree;
	ImageChannels largeLowC(largeLow);

	// Create an empty image (black) upon which we will paste the hi-freq patches (additive paste)
	ImageChannels largeHighC(largeLowC.w(), largeLowC.h());

	// match each one of the possible 5x5 patches into the blurred version of the small picture
	for (int i = PATCH_SIZE / 2; i < largeLowC.w() - PATCH_SIZE; i++) {

		for (int j = PATCH_SIZE / 2; j < largeLowC.h() - PATCH_SIZE; j++) {
			int bestX, bestY;
			e = locateBestPatch(srcC, largeLowC, i, j, &bestX, &bestY); ree;

			// additively paste the hi-freq patch
			e = applyPatch(highC, bestX, bestY, &largeHighC, i, j); ree;
		}
	}

	// Every pixel in the high freq image has now received the contribution of 5x5 incoming patches. Average.
	largeHighC /= PATCH_SIZE * PATCH_SIZE;

	// Finally, merge low and high frequency bands of the scaled image
	ImageChannels largeC = largeLowC + largeHighC;

	// write output
	*dst = largeC;

	return e;
}

