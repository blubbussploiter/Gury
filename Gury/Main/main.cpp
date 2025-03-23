//#include "Game/rbx.h"

//#include "Game/Services/crashreporter.h"
//#include "Game/strings.h"

//#include "CStudioMainWindow.h"

#include <thread>

#include "../Gury/Studio/supergurymfc.h"

int main()
{

	//CStudioApp* studio;
	//studio = new CStudioApp();

	//RBX::CrashReporter::singleton()->start();
	/*

	GAppSettings settings;

	settings.window.width = 820;
	settings.window.height = 480;
	settings.window.resizable = true;
	settings.dataDir = ConFileInPath("\\content\\");

	Rendering::G3DApp app(settings);

	app.debugController.setActive(false);

	app.run();
	*/

	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 1))
	{
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		return 1;
	}

	theApp.InitApplication();
	theApp.InitInstance();

	theApp.Run();

	return 0;
}