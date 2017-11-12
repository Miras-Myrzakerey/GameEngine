#ifndef _WINDOW_H
#define _WINDOW_H

#include "System.h"

//C Runtime 
#ifndef _WINDOWS_
	#include <Windows.h>
#endif

//Additional includes
#ifndef _CONTEXT_H
	#include "context.h"
#endif
#ifndef _STRING_H
	#include "string.h"
#endif

//Structs
struct WindowData : public SystemData {
	WindowData();
	WindowData(int w, int h, const std::tstring& title = _T("Game Engine"), int b = 32, bool fs = false);

	int width, height, bits;
	std::tstring windowTitle;

	bool fullscreen;
};

struct ResizeData {
	ResizeData();
	ResizeData(bool resize, int nw, int nh);

	bool mustResize;

	int newWidth, newHeight;
};

class Window : public System
{
	friend class Engine;
public:

	int getWidth();
	int getHeight();

	HWND GetWIndowHandle();
	HDC GetDeviceContext();
	HINSTANCE GetInstance();

	ResizeData& GetResizeData() { return m_ResizeData; }

	LRESULT HandleEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lparam);
protected:
	Window(const WindowData& data);
	virtual ~Window();

	virtual bool Initialize();
	virtual bool Update(Context& context);
	virtual bool Shutdown();

private:
	bool CenterWindow();

	LRESULT CALLBACK Window_Procedure(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

	HWND m_hWindow;
	HDC m_hDC;
	HINSTANCE m_hInst;

	int m_Width, m_height, m_Bits;

	bool m_bFullscreen;

	ResizeData m_ResizeData;

	std::tstring m_title;
};

#endif