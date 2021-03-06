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

#ifndef __SCALER_EEP_H__
#define __SCALER_EEP_H__

#include "Scaler2x.h"

// TODO: Remove
#include "../common/Image.h"

class ScalerEEP : public Scaler2x {
public:
	ScalerEEP();
	virtual ~ScalerEEP();

private:
	Err scale2x(const Image &src, Image *dst) override;
	
	// sampling functions
	pixel sampleEvenEven(const Image &src, int i, int j) const;
	pixel sampleEvenOdd(const Image &src, int i, int j) const;
	pixel sampleOddEven(const Image &src, int i, int j) const;
	pixel sampleOddOdd(const Image &src, int i, int j) const;
};

#endif // ndef __SCALER_EEP_H__