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

#include <iostream>
#include <string>

#include "../proc/proc.h"
#include "../IO/io.h"

#include "Controller.h"


using namespace std;

Controller::Controller() : view(&model, this) {
	model.title = "Scaler Comparison App";

	// start with the default image
	Image defaultImage;

	Loader::instance().load("data/default.png", &defaultImage);
	applyImage(defaultImage);

	continueRunning = true;
}

Controller::~Controller() {

}

static void displayAvailableScalers() {
	// List all available scalers
	cout << "Available scalers: " << endl;
	for (int i = 0; i < ScalerFactory::instance().typeCount(); i++) {
		cout << '\t' << ScalerFactory::instance().typeName(i) << endl;
	}
	cout << endl;
}

Err Controller::runApplication() {
	Err e = Err::Success;

	// start by displaying the available scalers
	displayAvailableScalers();

	while (continueRunning) {
		tick();
	}

	return e;
}

void Controller::quitRequestedByUser() {
	continueRunning = false;
}

void Controller::saveRequestedByUser() {
	for (int i = 0; i < (int)model.scaledImages.size(); i++) {
		std::string filename = model.sourceImageFilename + ScalerFactory::instance().typeName(i) + ".png";
		Loader::instance().save(filename.c_str(), model.scaledImages[i]);
	}
}

void Controller::userDroppedFile(const std::string &filename) {

	cout << "Dropped file: " << filename << endl;

	model.scaleFactor = 1;
	model.sourceImageFilename = filename;

	// load the new image
	Image img;
	Err e = Loader::instance().load(filename.c_str(), &img);
	if (e == Err::Success) {
		applyImage(img);
	} else {
		cout << "E: Failed to load image file: " << filename << endl;
	}
}

void Controller::userChangedScaleFactor(int newScaleFactor) {
	if (model.scaleFactor == newScaleFactor) {
		return;
	}

	cout << "New scale factor: " << newScaleFactor << endl;
	model.scaleFactor = newScaleFactor;
	applyImage(model.sourceImage);
}

Err Controller::tick() {
	Err e = Err::Success;

	e = view.tick(); ree;

	return e;
}

void Controller::applyImage(const Image &img) {
	// save to model
	model.sourceImage = img;

	// create all scalings.
	model.scaledImages.resize(0);
	// Create scalers
	for (int i = 0; i < ScalerFactory::instance().typeCount(); i++) {
		Image tmp;

		Scaler *scaler = ScalerFactory::instance().newScaler((ScalerType)i);
		scaler->scale(img, img.w() * model.scaleFactor, img.h() * model.scaleFactor, &tmp);
		delete scaler;

		model.scaledImages.push_back(tmp);
	}
	
	// notify view
	view.onNewImage();
}