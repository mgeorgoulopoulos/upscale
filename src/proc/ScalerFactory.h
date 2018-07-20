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

#ifndef __SCALER_FACTORY_H__
#define __SCALER_FACTORY_H__

class Scaler;

enum class ScalerType {
	OpenCV_Nearest,
	OpenCV_Linear,
	OpenCV_Cubic,
	OpenCV_Lanczos,
	DDT,
	EEP,
	SelfSim2x,

	Count
};

static const char *scalerTypeNames[] = {
	"Nearest",
	"Linear",
	"Cubic",
	"Lanczos",
	"DDT",
	"EEP",
	"SelfSim",
};

class ScalerFactory {
public:
	static ScalerFactory &instance();

	int typeCount() const { return (int)ScalerType::Count; }
	const char *typeName(int scalerType) const { return scalerTypeNames[scalerType]; }

	Scaler *newScaler(ScalerType type) const;

protected:
	ScalerFactory();
	virtual ~ScalerFactory();

private:
	static ScalerFactory *inst;
};



#endif // ndef __SCALER_FACTORY_H__