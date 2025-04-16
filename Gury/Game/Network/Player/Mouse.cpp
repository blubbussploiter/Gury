
#include "../Gury/Game/World/camera.h"
#include "../Gury/Game/Objects/Humanoid/humanoid.h"
#include "../Gury/Application/appmanager.h"
#include "../Gury/Game/World/ray.h"
#include "../Gury/Game/strings.h"
#include "../Gury/Game/Services/selection.h"

#include "Mouse.h"
#include "players.h"

TextureRef cursor_far;
TextureRef cursor_close;
TextureRef cursor_clicker;

float szx = 125, szy = 125;

RBX::Mouse* mouse;
HCURSOR oldCursor;

RBX::PVInstance* RBX::Mouse::getTarget() /* ignore part for dragger tool */
{
	RBX::Camera* camera = RBX::Camera::get();
	RBX::ISelectable* selected;
	Ray ray;

	ray = camera->camera->worldRay(x, y, RBX::RBXManager::get()->getApplication()->getViewport());

	selected = RBX::World::getPartFromG3DRay<Instance>(ray, hitWorld);
	target = (RBX::PVInstance*)(selected);

	return target;
}

RBX::ModelInstance* RBX::Mouse::getModelTarget()
{
	RBX::Camera* camera = RBX::Camera::get();
	Ray ray;

	ray = camera->camera->worldRay(x, y, RBX::RBXManager::get()->getApplication()->getViewport());

	return (RBX::ModelInstance*)(RBX::World::getModelFromG3DRay<Instance>(ray, hitWorld));
}

void RBX::Mouse::doUserInput(UserInput* ui)
{
	if (!ui->keyDown(SDL_RIGHT_MOUSE_KEY))
	{
		x = ui->getMouseX();
		y = ui->getMouseY();
		cx = x - szx / 2;
		cy = y - szy / 2;
	}
}

void RBX::Mouse::updateCursorInfo(bool overGui)
{
	if (cursor_clicker.isNull() || cursor_close.isNull()) return;
	if (overGui && 
		SelectionService::get()->selectionAllowed &&
		SelectionService::get()->getPossibleSelectedItem() != 0)
	{
		currentglId = cursor_clicker->getOpenGLID();
	}
	else
	{
		currentglId = cursor_close->getOpenGLID();
	}
}

void RBX::Mouse::render(RenderDevice* rd)
{
	if (cursor_far.isNull())
	{
		cursor_far = Texture::fromFile(GetFileInPath("/content/textures/ArrowFarCursor.png"));
		cursor_close = Texture::fromFile(GetFileInPath("/content/textures/ArrowCursor.png"));
		cursor_clicker = Texture::fromFile(GetFileInPath("/content/textures/DragCursor.png"));
		currentglId = cursor_close->getOpenGLID();
	}

	RBX::Experimental::Application* application = RBXManager::get()->getApplication();
	if (application && application->inGuryWindow())
	{

		rd->push2D();

		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindTexture(GL_TEXTURE_2D, currentglId);
		glBegin(GL_QUADS);

		glTexCoord2d(0.0, 0.0);
		glVertex2f(cx, cy);

		glTexCoord2d(1.0, 0.0);
		glVertex2f(cx + szx,cy);

		glTexCoord2d(1.0, 1.0);
		glVertex2f(cx + szx,cy + szy);

		glTexCoord2d(0.0, 1.0);
		glVertex2f(cx, cy + szy);

		glEnd();
		glDisable(GL_TEXTURE_2D);

		rd->pop2D();
	}
}

HCURSOR RBX::Mouse::get_cursor()
{
	if (!cursor)
	{
		int width = GetSystemMetrics(SM_CXCURSOR);
		int height = GetSystemMetrics(SM_CYCURSOR);

		BYTE* andMask = new BYTE[width * height / 8];
		BYTE* xorMask = new BYTE[width * height / 8];

		memset(andMask, 0xFF, width * height / 8);
		memset(xorMask, 0x00, width * height / 8); 

		cursor = CreateCursor(0, 0, 0, width, height, andMask, xorMask);
	}
	return cursor;
}

RBX::Mouse* RBX::Mouse::get()
{
	if (!mouse) mouse = new RBX::Mouse();
	return mouse;
}
