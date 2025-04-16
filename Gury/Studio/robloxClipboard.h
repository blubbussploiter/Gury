#pragma once

#include "../Gury/Game/Objects/instance.h"

namespace RBX
{

	class StudioClipboard
	{

	private:

		Instances* content;

	public:

		void clearContent();

		void copy(Instance* what);
		void copyMultiple(Instances* which);

		void cut(Instance* what);
		void cutMultiple(Instances* which);

		void paste(Instance* where);

		bool isEmpty() {
			return content->size() > 0;
		}

		static StudioClipboard* get();

		StudioClipboard()
		{
			content = new Instances();
		}

	};
}