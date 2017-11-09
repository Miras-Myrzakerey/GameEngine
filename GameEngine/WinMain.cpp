#include "WinMain.h"
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//Additional includes
#include "Engine.h"

#ifndef _STRING_H
#include "string.h"
#endif
#ifndef _IOSTREAM_H
#include "iostream.h"
#endif

#ifndef _DELETEMACRO_H
	#include "deletemacros.h"
#endif

#if defined (DEBUG) | defined (_DEBUG)
int _tmain(int argc, char* argv[]) {
	UNREFERENCED_PARAMETER(argc);
	UNREFERENCED_PARAMETER(argv);

	//Logger::Log("Starting the Program");

	//Run this shit
	WinMain((HINSTANCE)GetModuleHandle(NULL), 0, 0, SW_SHOW);
}
#endif

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

#if defined (DEBUG) | defined (_DEBUG)
	HeapSetInformation(NULL, HeapEnableTerminationOnCorruption, NULL, 0);

	//Enable run-time memory leak check for debug

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(0);

#endif

	//Create the engine
	Engine* pEngine = new Engine();

	//Kick of the game
	int result = pEngine->RunLoop();

	//Delete the Engine
	SafeDelete(pEngine);
	
	return result;
}
