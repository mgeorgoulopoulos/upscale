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

#ifndef _VIEW_H_
#define _VIEW_H_

#include "../common/Common.h"

#include "model.h"

// fwd declaration
struct ViewPrivateData;
class Controller;

class View {
public:
	View(Model *model, Controller *controller);
	virtual ~View();

	Err tick();

	void onNewImage();

private:
	Err processEvents();
	Err render();

	// event handling
	void onMouseMove(int x, int y, int dx, int dy, bool left, bool right);
	void onKey(int key, bool down);

private:
	Model *model;
	Controller *controller;
	ViewPrivateData *data;
};

#endif // _VIEW_H_