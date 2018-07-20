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


#include <chrono>
#include <iostream>

#include "IO/IO.h"
#include "proc/proc.h"

using namespace std;

void benchmark() {

	Image src;
	Err e = Loader::instance().load("data/default.png", &src);
	if (e != Err::Success) {
		cout << "Problem" << endl;
		return;
	}
	cout << "Loading Success!" << endl;

	int dstW = src.w() * 2;
	int dstH = src.h() * 2;

	Image dst;
	dst.setSize(dstW, dstH);

	chrono::steady_clock c;

	for (int i = 0; i < ScalerFactory::instance().typeCount(); i++) {
		Scaler *scaler = ScalerFactory::instance().newScaler((ScalerType)i);

		auto before = c.now();

		const int benchCount = 100;
		for (int i = 0; i < benchCount; i++) {
			scaler->scale(src, dstW, dstH, &dst);
		}

		auto duration = c.now() - before;

		// Display results
		double milliseconds = chrono::duration<double, nano>(duration).count() / (benchCount * 1000000.0);
		cout << ScalerFactory::instance().typeName(i) << "\t" << milliseconds << endl;

		delete scaler;
	}
}