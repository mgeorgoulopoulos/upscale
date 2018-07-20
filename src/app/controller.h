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

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "../common/Common.h"

#include "model.h"
#include "view.h"

class Controller {
public:
	Controller();
	virtual ~Controller();

	Err runApplication();

	void quitRequestedByUser();
	void saveRequestedByUser();
	void userDroppedFile(const std::string &filename);
	void userChangedScaleFactor(int newScaleFactor);

private:
	Err tick();

	// Sets up a new main image. Calculates all its scalings, stores it in model and notifies the view.
	void applyImage(const Image &img);

private:
	Model model;
	View view;

	bool continueRunning;
};

#endif // _CONTROLLER_H_