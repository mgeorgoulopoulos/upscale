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

#include <string>

#include "view.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include "../proc/proc.h"

#include "controller.h"

namespace {
	enum class State {
		None = 0,
		BigPicture,
		AllScalers
	};

	struct BigPictureData {
		int currentScalerIndex;
		int xPan, yPan;
		float scaleFactor;
	};
}

struct ViewPrivateData {
	SDL_Window *window;

	TTF_Font* font;

	std::vector<SDL_Texture*> scaledTextures;
	std::vector<SDL_Texture*> textTextures;

	SDL_Texture* textScalingFactor;

	State state;

	BigPictureData bigPicture;

	// some handy accessors
	int windowW() const {
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		return w;
	}

	int windowH() const {
		int w, h;
		SDL_GetWindowSize(window, &w, &h);
		return h;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////
// BIG PICTURE
/////////////////////////////////////////////////////////////////////////////////////////////////

static Err centerImageBigPicture(const Model &model, ViewPrivateData &data) {
	Err e = Err::Success;

	const Image &img = model.scaledImages[data.bigPicture.currentScalerIndex];

	data.bigPicture.xPan = 0;
	data.bigPicture.yPan = 0;
	data.bigPicture.scaleFactor = 1.0f;

	return e;
}

static Err enterBigPicture(const Model &model, ViewPrivateData &data) {
	Err e = Err::Success;

	data.bigPicture.currentScalerIndex = 0;

	e = centerImageBigPicture(model, data); ree;

	return e;
}

static Err renderBigPicture(const Model &model, const ViewPrivateData &data) {
	Err e = Err::Success;

	if (data.scaledTextures.size() <= 0) {
		return Err::Error;
	}

	const Image &img = model.scaledImages[data.bigPicture.currentScalerIndex];
	SDL_Texture *texture = data.scaledTextures[data.bigPicture.currentScalerIndex];

	// scale 
	int scaledW = (int)(img.w() * data.bigPicture.scaleFactor + 0.5f);
	int scaledH = (int)(img.h() * data.bigPicture.scaleFactor + 0.5f);

	// render image
	SDL_Rect r;
	r.x = (data.windowW() - scaledW) / 2 + data.bigPicture.xPan;
	r.y = (data.windowH() - scaledH) / 2 + data.bigPicture.yPan;
	r.w = scaledW;
	r.h = scaledH;
	SDL_RenderCopy(SDL_GetRenderer(data.window), texture, NULL, &r);

	// render text
	SDL_Texture *text = data.textTextures[data.bigPicture.currentScalerIndex];
	int w, h;
	SDL_QueryTexture(text, nullptr, nullptr, &w, &h);
	r.x = 20;
	r.y = data.windowH() - h - 20;
	r.w = w;
	r.h = h;
	SDL_RenderFillRect(SDL_GetRenderer(data.window), &r);
	SDL_RenderCopy(SDL_GetRenderer(data.window), text, NULL, &r);

	// render magnification level
	SDL_QueryTexture(data.textScalingFactor, nullptr, nullptr, &w, &h);
	r.x = data.windowW() - w - 20;
	r.y = data.windowH() - h - 20;
	r.w = w;
	r.h = h;
	SDL_RenderFillRect(SDL_GetRenderer(data.window), &r);
	SDL_RenderCopy(SDL_GetRenderer(data.window), data.textScalingFactor, NULL, &r);

	return e;
}

static void onMouseMoveBigPicture(int x, int y, int dx, int dy, bool left, bool right,
	const Model &model, ViewPrivateData &data) {

	// Pan the image with left click dragging
	if (left) {
		data.bigPicture.xPan += dx;
		data.bigPicture.yPan += dy;
	}

	if (right) {
		data.bigPicture.scaleFactor *= (float)pow(1.01, -dy);
		data.bigPicture.scaleFactor = std::max(1.0f, data.bigPicture.scaleFactor);
		data.bigPicture.scaleFactor = std::min(10.0f, data.bigPicture.scaleFactor);
	}
}

static void onKeyBigPicture(int key, bool down, const Model &model, ViewPrivateData &data) {
	if (down) {
		if (key == SDLK_RIGHT) {
			data.bigPicture.currentScalerIndex++;
			if (data.bigPicture.currentScalerIndex >= (int)model.scaledImages.size()) {
				data.bigPicture.currentScalerIndex = 0;
			}
		}

		if (key == SDLK_LEFT) {
			data.bigPicture.currentScalerIndex--;
			if (data.bigPicture.currentScalerIndex < 0) {
				data.bigPicture.currentScalerIndex = (int) (model.scaledImages.size() - 1);
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// ALL SCALERS
/////////////////////////////////////////////////////////////////////////////////////////////////

static Err enterAllScalers(const Model &model, ViewPrivateData &data) {
	return enterBigPicture(model, data);
}


static Err renderCellAllScalers(int imageIndex, const Model &model, 
	const ViewPrivateData &data, const SDL_Rect &cellRect) {

	Err e = Err::Success;

	// Setup clipping region
	SDL_RenderSetClipRect(SDL_GetRenderer(data.window), &cellRect);

	const Image &img = model.scaledImages[imageIndex];
	SDL_Texture *texture = data.scaledTextures[imageIndex];

	// scale 
	int scaledW = (int)(img.w() * data.bigPicture.scaleFactor + 0.5f);
	int scaledH = (int)(img.h() * data.bigPicture.scaleFactor + 0.5f);

	// render image
	SDL_Rect r;
	r.x = cellRect.x + (cellRect.w - scaledW) / 2 + data.bigPicture.xPan;
	r.y = cellRect.y + (cellRect.h - scaledH) / 2 + data.bigPicture.yPan;
	r.w = scaledW;
	r.h = scaledH;
	SDL_RenderCopy(SDL_GetRenderer(data.window), texture, NULL, &r);

	// render text
	SDL_Texture *text = data.textTextures[imageIndex];
	int w, h;
	SDL_QueryTexture(text, nullptr, nullptr, &w, &h);
	r.x = cellRect.x + 20;
	r.y = cellRect.y + cellRect.h - h - 20;
	r.w = w;
	r.h = h;
	SDL_RenderFillRect(SDL_GetRenderer(data.window), &r);
	SDL_RenderCopy(SDL_GetRenderer(data.window), text, NULL, &r);

	// Reset clipping region
	SDL_RenderSetClipRect(SDL_GetRenderer(data.window), nullptr);

	return e;
}

static Err renderAllScalers(const Model &model, const ViewPrivateData &data) {
	Err e = Err::Success;

	if (data.scaledTextures.size() <= 0) {
		return Err::Error;
	}

	const int rowCount = 2;
	const int columnCount = ((int)data.scaledTextures.size() + 1) / 2;
	if (columnCount == 0 || rowCount == 0) {
		return Err::Error;
	}

	const int cellW = data.windowW() / columnCount;
	const int cellH = data.windowH() / rowCount;

	for (int i = 0; i < (int)data.scaledTextures.size(); i++) {
		int cellX = i % columnCount;
		int cellY = i / columnCount;

		SDL_Rect cellRect;
		cellRect.x = cellX * cellW;
		cellRect.y = cellY * cellH;
		cellRect.w = cellW;
		cellRect.h = cellH;

		renderCellAllScalers(i, model, data, cellRect);
	}

	// render magnification level
	int w, h;
	SDL_QueryTexture(data.textScalingFactor, nullptr, nullptr, &w, &h);
	SDL_Rect r;
	r.x = data.windowW() - w - 20;
	r.y = data.windowH() - h - 20;
	r.w = w;
	r.h = h;
	SDL_RenderFillRect(SDL_GetRenderer(data.window), &r);
	SDL_RenderCopy(SDL_GetRenderer(data.window), data.textScalingFactor, NULL, &r);


	return e;
}

static void onMouseMoveAllScalers(int x, int y, int dx, int dy, bool left, bool right,
	const Model &model, ViewPrivateData &data) {
	onMouseMoveBigPicture(x, y, dx, dy, left, right, model, data);
}

static void onKeyAllScalers(int key, bool down, const Model &model, ViewPrivateData &data) {
	onKeyBigPicture(key, down, model, data);
}
/////////////////////////////////////////////////////////////////////////////////////////////////
// VIEW CLASS
/////////////////////////////////////////////////////////////////////////////////////////////////

static SDL_Texture *textureFromText(const std::string &text, ViewPrivateData &data) {
	SDL_Surface* s = TTF_RenderText_Solid(data.font, text.c_str(), { 255, 255, 255 });
	SDL_Texture* ret = SDL_CreateTextureFromSurface(SDL_GetRenderer(data.window), s);
	SDL_FreeSurface(s);
	return ret;
}

View::View(Model *model, Controller *controller) {
	data = new ViewPrivateData();

	this->model = model;
	this->controller = controller;

	// Initialize SDL
	SDL_Init(SDL_INIT_VIDEO);

	// create window and renderer
	data->window = SDL_CreateWindow(model->title.c_str(), 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 768, 0);
	SDL_SetWindowResizable(data->window, SDL_TRUE);
	SDL_CreateRenderer(data->window, -1, 0);

	// Enable drop files
	SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

	// Initialize SDL_ttf
	TTF_Init();

	data->font = TTF_OpenFont("data/FreeSans.ttf", 48);

	// Create textures for text
	for (int i = 0; i < ScalerFactory::instance().typeCount(); i++) {
		std::string type = ScalerFactory::instance().typeName(i);
		data->textTextures.push_back(textureFromText(type, *data));
	}
	data->textScalingFactor = nullptr;

	data->state = State::None;	
}

View::~View() {
	TTF_CloseFont(data->font);
	TTF_Quit();

	SDL_DestroyTexture(data->textScalingFactor);
	data->textScalingFactor = nullptr;

	for (auto t : data->scaledTextures) {
		SDL_DestroyTexture(t);
	}
	data->scaledTextures.resize(0);

	for (auto t : data->textTextures) {
		SDL_DestroyTexture(t);
	}
	data->textTextures.resize(0);

	SDL_DestroyRenderer(SDL_GetRenderer(data->window));
	SDL_DestroyWindow(data->window);

	SDL_Quit();

	delete data;
	data = nullptr;
}

Err View::tick() {
	Err e = Err::Success;

	e = processEvents(); ree;

	e = render(); ree;

	return e;
}

static SDL_Texture *textureFromImage(SDL_Renderer *renderer, const Image &img) {
	SDL_Texture *texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING,
		img.w(), img.h());

	void *texels = nullptr;
	int pitch;

	SDL_LockTexture(texture, nullptr, &texels, &pitch);

	uint8_t *src = (uint8_t*)&img.pixels[0];
	uint8_t *dst = (uint8_t*)texels;

	for (int j = 0; j < img.h(); j++) {
		memcpy(dst, src, img.w() * 4);
		src += img.w() * 4;
		dst += pitch;
	}

	SDL_UnlockTexture(texture);

	return texture;
}

void View::onNewImage() {
	for (SDL_Texture *t : data->scaledTextures) {
		SDL_DestroyTexture(t);
	}
	data->scaledTextures.resize(0);

	// Convert all images from model to textures which we can display
	for (const Image &i : model->scaledImages) {
		data->scaledTextures.push_back(textureFromImage(SDL_GetRenderer(data->window), i));
	}

	// Create texture for scaling factor text
	SDL_DestroyTexture(data->textScalingFactor);
	char tb[32];
	sprintf_s(tb, 32, "%dx", model->scaleFactor);
	data->textScalingFactor = textureFromText(tb, *data);

	// Enter big picture mode
	data->state = State::BigPicture;
	enterBigPicture(*model, *data);
}

Err View::processEvents() {
	Err e = Err::Success;

	SDL_Event evt;
	while (SDL_PollEvent(&evt)) {
		switch (evt.type) {
		case SDL_QUIT:
			controller->quitRequestedByUser();
			break;
		case (SDL_DROPFILE):
			controller->userDroppedFile(evt.drop.file);
			SDL_free(evt.drop.file);
			break;
		case SDL_MOUSEMOTION:
			onMouseMove(evt.motion.x, evt.motion.y, 
				evt.motion.xrel, evt.motion.yrel, 
				(evt.motion.state & SDL_BUTTON_LMASK) != 0, 
				(evt.motion.state & SDL_BUTTON_RMASK) != 0);
			break;
		case SDL_KEYDOWN:
			onKey(evt.key.keysym.sym, true);
			break;
		}
	}

	return e;
}



Err View::render() {
	Err e = Err::Success;

	SDL_SetRenderDrawColor(SDL_GetRenderer(data->window), 0, 0, 0, 255);
	SDL_RenderClear(SDL_GetRenderer(data->window));


	switch (data->state) {
	case State::BigPicture:
		e = renderBigPicture(*model, *data); ree;
		break;
	case State::AllScalers:
		e = renderAllScalers(*model, *data); ree;
		break;
	}

	SDL_RenderPresent(SDL_GetRenderer(data->window));

	return e;
}

void View::onMouseMove(int x, int y, int dx, int dy, bool left, bool right) {
	switch (data->state) {
	case State::BigPicture:
		onMouseMoveBigPicture(x, y, dx, dy, left, right, *model, *data);
		break;
	case State::AllScalers:
		onMouseMoveAllScalers(x, y, dx, dy, left, right, *model, *data);
		break;
	}
}

void View::onKey(int key, bool down) {
	if (down) {
		if (key == SDLK_UP) {
			int s = model->scaleFactor;
			s++;
			s = std::min(8, s);
			controller->userChangedScaleFactor(s);
		}
		if (key == SDLK_DOWN) {
			int s = model->scaleFactor;
			s--;
			s = std::max(1, s);
			controller->userChangedScaleFactor(s);
		}
		if (key == SDLK_ESCAPE) {
			controller->quitRequestedByUser();
		}
		if (key == SDLK_s) {
			controller->saveRequestedByUser();
		}

		if (key == SDLK_F1) {
			data->state = State::BigPicture;
			enterBigPicture(*model, *data);
		}

		if (key == SDLK_F2) {
			data->state = State::AllScalers;
			enterAllScalers(*model, *data);
		}
	}

	switch (data->state) {
	case State::BigPicture:
		onKeyBigPicture(key, down, *model, *data);
		break;
	case State::AllScalers:
		onKeyAllScalers(key, down, *model, *data);
		break;
	}
}