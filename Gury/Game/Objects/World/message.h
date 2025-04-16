#pragma once

#include "../Gury/Game/World/datamodel.h"
#include "../IRenderable.h"

namespace RBX {

	class Message : public Render::IRenderable
	{
	private:
		std::string text;
	public:

		void renderFullScreen(RenderDevice* renderDevice);
		void renderPersonalMsg(RenderDevice* renderDevice);

		void render2D(RenderDevice* renderDevice);

		void setText(std::string text)
		{
			this->text = text;
		}

		std::string getText()
		{
			return text;
		}

		Message()
		{
			setClassName("Message");
			setName("Message");
		}

		RTTR_ENABLE(Instance);
	};
}