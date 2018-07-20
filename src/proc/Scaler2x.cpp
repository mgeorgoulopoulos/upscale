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

#include "Scaler2x.h"

#include "../common/common.h"

Scaler2x::Scaler2x() {

}

Scaler2x::~Scaler2x() {

}


Err Scaler2x::scale(const Image &src, int dstW, int dstH, Image *dst) {
	Err e = Err::Success;

	if (src.w() <= 0 || src.h() <= 0 || dstW <= 0 || dstH <= 0) {
		return Err::BadArgument;
	}

	// Couldn't hurt
	if (dstW == src.w() && dstH == src.h()) {
		*dst = src;
		return Err::Success;
	}

	Image tmp = src;

	// Double in size until we reach or exceed destination size
	while (tmp.w() < dstW || tmp.h() < dstH) {
		Image tmp2;
		scale2x(tmp, &tmp2);
		tmp = tmp2;
	}

	// Finally, use a linear scaler to reach exactly destination size
	e = scaleLinear(tmp, dstW, dstH, dst); ree;

	return e;
}