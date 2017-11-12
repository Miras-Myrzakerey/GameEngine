#ifndef _CONTEXT_H
#define _CONTEXT_H

//Forward declaration
class Window;

//Defines
#define WINDOW (context.pWnd)

struct Context {
	float dTime;

	Window* pWnd;
};

#endif
