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

#include "ScalerFactory.h"

#include "ScalerOCV.h"
#include "ScalerSelfSim2x.h"
#include "ScalerDDT.h"
#include "ScalerEEP.h"

ScalerFactory *ScalerFactory::inst = nullptr;


ScalerFactory &ScalerFactory::instance() {
	if (inst == nullptr) {
		inst = new ScalerFactory();
	}
	return *inst;
}

ScalerFactory::ScalerFactory() {

}

ScalerFactory::~ScalerFactory() {
}

Scaler *ScalerFactory::newScaler(ScalerType type) const {
	switch (type) {
	case ScalerType::OpenCV_Nearest:
		return new ScalerOCV(ScalerOCV::Filter::Nearest);
	case ScalerType::OpenCV_Linear:
		return new ScalerOCV(ScalerOCV::Filter::Linear);
	case ScalerType::OpenCV_Cubic:
		return new ScalerOCV(ScalerOCV::Filter::Cubic);
	case ScalerType::OpenCV_Lanczos:
		return new ScalerOCV(ScalerOCV::Filter::Lanczos);
	case ScalerType::DDT:
		return new ScalerDDT();
	case ScalerType::EEP:
		return new ScalerEEP();
	case ScalerType::SelfSim2x:
		return new ScalerSelfSim2x();
	}
	return nullptr;
}