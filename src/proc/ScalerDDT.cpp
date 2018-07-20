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

#include "ScalerDDT.h"

#include "../common/common.h"

ScalerDDT::ScalerDDT() {

}

ScalerDDT::~ScalerDDT() {

}

Err ScalerDDT::scale(const Image &src, int dstW, int dstH, Image *dst) {
	Err e = Err::Success;

	// Couldn't hurt
	if (dstW == src.w() && dstH == src.h()) {
		*dst = src;
		return Err::Success;
	}

	if (src.w() <= 1 || src.h() <= 1) {
		return Err::BadArgument;
	}

	e = createEdges(src); ree;

	dst->setSize(dstW, dstH);

	for (int j = 0; j < dstH; j++) {
		for (int i = 0; i < dstW; i++) {
			dst->setPixel(i, j, bilinear(src, i, dstW, j, dstH));
		}
	}

	return e;
}

Err ScalerDDT::createEdges(const Image &src) {
	Err e = Err::Success;

	if (src.w() <= 1 || src.h() <= 1) {
		return Err::BadArgument;
	}

	int edgesW = src.w() - 1;
	int edgesH = src.h() - 1;
	edges.resize(edgesW * edgesH);

	for (int j = 0; j < edgesH; j++) {
		for (int i = 0; i < edgesW; i++) {
			// Get 4 neighbouring colors
			color nw = src.getPixel(i, j);
			color ne = src.getPixel(i + 1, j);
			color se = src.getPixel(i + 1, j + 1);
			color sw = src.getPixel(i, j + 1);

			// calculate brighness
			comp lnw = L(nw);
			comp lne = L(ne);
			comp lse = L(se);
			comp lsw = L(sw);

			// decide edge direction
			bool slash = true;
			if (abs(lnw - lse) < abs(lne - lsw)) {
				slash = false;
			}

			// store edge
			edges[i + edgesW * j] = slash;
		}
	}

	// extended method: post-process
	auto tmp = edges;

	for (int j = 0; j < edgesH; j++) {
		if (j < 1 || j >= edgesH - 2) {
			continue;
		}
		for (int i = 0; i < edgesW; i++) {
			if (i < 1 || i >= edgesW - 2) {
				continue;
			}
			
			// count slashes
			int slashCount = 0;
			for (int nj = -1; nj <= 1; nj++) {
				for (int ni = -1; ni <= 1; ni++) {
					if (ni == 0 && nj == 0) {
						continue;
					}

					if (tmp[(i + ni) + (j + nj) * edgesW]) {
						slashCount++;
					}
				}
			}

			int idx = i + j * edgesW;

			if (slashCount == 4) {
				// draw - keep original
				edges[idx] = tmp[idx];
			} else if (slashCount > 4) {
				edges[idx] = true; // slash
			} else {
				edges[idx] = false; // backslash
			}
		}
	}

	return e;
}

pixel ScalerDDT::bilinear(const Image &src, int i, int dstW, int j, int dstH) {	

	// We want to map the center of the first pixel (u=0.5) to the center of the first source pixel (u=0.5 also)
	// We want to map the center of the last pixel (u=width-0.5) to the center of the source last pixel (u=sourceWidth-0.5)
	// So, in reality, scale is (source - 1) / (destination -  1);
	float scaleX = (float)(src.w()-1) / (float)(dstW-1);
	float scaleY = (float)(src.h()-1) / (float)(dstH-1);

	// map coordinates to original image
	float sif = (float) i * scaleX + 0.5f;
	float sjf = (float)j * scaleY + 0.5f;

	// find neighbouring pixels
	int si = (int)sif;
	int sj = (int)sjf;

	// get sub-pixel coordinates
	float u = sif - si;
	float v = sjf - sj;

	// get coordinates to the edge table
	int ei = si;
	if (u < 0.5f) {
		si--;
		ei--;
		u += 0.5f;
	} else {
		u -= 0.5f;
	}

	int ej = sj;
	if (v < 0.5f) {
		sj--;
		ej--;
		v += 0.5f;
	} else {
		v -= 0.5f;
	}

	// clamp to edge table dimensions
	ei = std::max(0, std::min(src.w() - 2, ei));
	ej = std::max(0, std::min(src.h() - 2, ej));

	// see which edge was selected for triangulation
	bool slash = edges[ei + ej * (src.w() - 1)];

	if (slash) {
		return sampleSlash(src, si, sj, u, v);
	} else {
		return sampleBackslash(src, si, sj, u, v);
	}
}

pixel ScalerDDT::sampleSlash(const Image &src, int i, int j, float u, float v) {
	if (u + v <= 1.0f) {
		// upper triangle
		pixel a = src.getPixel(i, j);
		pixel b = src.getPixel(i + 1, j);
		pixel c = src.getPixel(i, j + 1);
		return mix3(a, b, c, u, v);
	} else {
		// bottom triangle
		pixel a = src.getPixel(i + 1, j + 1);
		pixel b = src.getPixel(i, j + 1);
		pixel c = src.getPixel(i + 1, j);
		return mix3(a, b, c, 1.0f - u, 1.0f - v);
	}
}
	
pixel ScalerDDT::sampleBackslash(const Image &src, int i, int j, float u, float v) {
	if (u >= v) {
		// upper triangle
		pixel a = src.getPixel(i + 1, j);
		pixel b = src.getPixel(i, j);
		pixel c = src.getPixel(i + 1, j + 1);
		return mix3(a, b, c, 1.0f - u, v);
	} else {
		// bottom triangle
		pixel a = src.getPixel(i, j + 1);
		pixel b = src.getPixel(i + 1, j + 1);
		pixel c = src.getPixel(i, j);
		return mix3(a, b, c, u, 1.0f - v);
	}
}

pixel ScalerDDT::mix3(pixel a, pixel b, pixel c, float u, float v) {
	// unpack a
	float ar = R(a);
	float ag = G(a);
	float ab = B(a);

	// unpack b
	float br = R(b);
	float bg = G(b);
	float bb = B(b);

	// unpack c
	float cr = R(c);
	float cg = G(c);
	float cb = B(c);

	// cache
	float omu = 1.0f - u;
	float omv = 1.0f - v;

	// mix horizontally
	float r1 = ar * omu + br * u;
	float r2 = cr * omu + br * u;
	float g1 = ag * omu + bg * u;
	float g2 = cg * omu + bg * u;
	float b1 = ab * omu + bb * u;
	float b2 = cb * omu + bb * u;

	// mix vertically
	float fr = r1 * omv + r2 * v;
	float fg = g1 * omv + g2 * v;
	float fb = b1 * omv + b2 * v;

	// Convert to integer
	comp finalR = (comp)std::max(0.0f, std::min(255.0f, fr));
	comp finalG = (comp)std::max(0.0f, std::min(255.0f, fg));
	comp finalB = (comp)std::max(0.0f, std::min(255.0f, fb));

	// re-pack
	pixel ret = RGB(finalR, finalG, finalB);

	return ret;
}