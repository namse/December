#include "main.h"
#include "AppDelegate.h"
#include "cocos2d.h"
#include "Header.h"

USING_NS_CC;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	if (DEBUG_PRINT_PACKET)
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
	}

    // create the application instance
    AppDelegate app;
    return Application::getInstance()->run();
}
