
#pragma once

#include <G3DAll.h>
#include "application.h"

namespace RBX
{

	class RBXManager
	{
	private:

		std::vector<Experimental::Application*> applications;
		Experimental::Application* currentApplication;
		bool bOneTimeInitialized, bMainLoopStarted;

	public:

		std::string toLoad;
		std::string fileName;

		Experimental::Application* instantiate(HWND wnd);
		Experimental::Application* getApplication();

		void closeCurrentApplication();

		void setCurrentApplication(Experimental::Application* app);
		void setCurrentApplicationFromIndex(size_t index);

		std::string getDocumentCaption(Experimental::Application* app);
		std::string getPlaceFormattedName(std::string prefix="Place");

		void initOneTimeAppliances();

		void start();

		bool mainLoopStarted();

		static RBXManager* get();
		static bool isReady()
		{
			return (get()->getApplication() != 0);
		}
	};
}