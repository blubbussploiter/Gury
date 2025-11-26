#pragma once

#include "../Gury/Game/World/datamodel.h"
#include "../IRenderable.h"

namespace RBX {

	class Hint : public Render::IRenderable
	{
	private:
		std::string text;
	public:

		void render2D(RenderDevice* renderDevice);

		void setText(std::string text)
		{
			this->text = text;
		}

		std::string getText()
		{
			return text;
		}

		Hint()
		{
			setClassName("Hint");
			setName("Message");
		}

		RTTR_ENABLE(Instance);
	};
}