#include "Window.h"

//#include "Logger.h"

WindowData::WindowData() 
: SystemData(SystemType::Sys_Invalid)
, width(-1)
, height(-1)
, bits(-1)
, windowTitle(_T(""))
, fullscreen(false){

}

WindowData::WindowData(int w, int h, const std::tstring& title, int b, bool fs) 
: SystemData(SystemType::Sys_Window)
, width(w)
, height(h)
, bits(b)
, windowTitle(title)
, fullscreen(fs){

}

ResizeData::ResizeData()
: mustResize(false)
, newWidth(-1)
, newHeight(-1){

}

ResizeData::ResizeData(bool resize, int nw, int nh) 
: mustResize(resize)
, newWidth(nw)
, newHeight(nh){

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (msg == WM_CREATE) {
		//if the message is WM_CREATE, the lparam contains a pointer to CREATESTRUCT
		//the CREATESTRUCT contains the "this" pointer from the CreateWindow method
		//"this" pointer is stored in the createStruct pcs->lpCreateParams
		CREATESTRUCT* pCS = (CREATESTRUCT*)lparam;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG)pCS->lpCreateParams);
	} else {
		//Retrieve the stored "this" pointer 
		Window* pWindow = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (pWindow) {
			return pWindow->HandleEvent(hWnd, msg, wparam, lparam);
		}
	}

	return DefWindowProc(hWnd, msg, wparam, lparam);
}

LRESULT Window::HandleEvent(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {
		case WM_ACTIVATE: 
		{
			if (!HIWORD(wparam)) {
				this->Activate();
			} else {
				this->Deactivate();
			}

			return 0;
		}
		case WM_SIZE:
		{
			UINT width = LOWORD(lparam);
			UINT height = HIWORD(lparam);

			m_ResizeData.mustResize = true;
			m_ResizeData.newWidth = width;
			m_ResizeData.newHeight = height;

			return 0;
		}
		case WM_DISPLAYCHANGE:
		{
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}
	}

	return DefWindowProc(hWnd, msg, wparam, lparam);
}

Window::Window(const WindowData& data)
: System(data)
, m_hWindow(NULL)
, m_hDC(NULL)
, m_hInst(NULL)
, m_Width(data.width)
, m_height(data.height)
, m_Bits(data.bits)
, m_bFullscreen(data.fullscreen)
, m_title(data.windowTitle)
, m_ResizeData(){
}
Window::~Window() {
}

bool Window::Initialize() {
	System::Initialize();
	
	unsigned int pixelFormat;

	DWORD dwExStyle;					//Window Extended Style
	DWORD dwStyle;						//Window Style

	RECT wndRect;						//Grabs Rectangle Upper Left / Lower Right Values
	wndRect.left = (long)0;				//Set left value to 0
	wndRect.right = (long)m_Width;		//Set right value to requested width
	wndRect.top = (long)0;				//Set top value to 0
	wndRect.bottom = (long)m_height;	//Set bottom value to requested height

	WNDCLASS wndClass;					//Windows Class Structure
	std::tstring className = m_title;	//Window Title

	wndClass.style = CS_DBLCLKS | CS_DROPSHADOW | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = NULL;
	wndClass.cbWndExtra = NULL;
	wndClass.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = className.c_str();

	if (!RegisterClass(&wndClass)) {
		//Logger::Log(_T("Failed to register window"), LOGTYPE_ERROR, true);
		return false;
	}

	if (m_bFullscreen) {
		DEVMODE dmScreenSettings;								//Device Mode
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings)); //Make sure memory is cleared
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);		//Size of the devmode structure
		dmScreenSettings.dmPelsWidth = m_Width;					//Selected screen width
		dmScreenSettings.dmPelsHeight = m_height;				//Selected screen height
		dmScreenSettings.dmBitsPerPel = m_Bits;					//Selected bits per pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Try to set selected mode and get results. 
		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL) {
			//If the mode fails, use windowed
			//Logger::Log(_T("The Requested Fullscreen Mode Is Not Supported By\nYour Video Card."), LOGTYPE_WARNING, true);

			dwExStyle = WS_EX_APPWINDOW; //Window Extended Style
			dwStyle = WS_POPUP;			 //Windows Style
			//ShowCursor(FALSE);		 //Hide mouse pointer (optional)
		}
	} else {
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; //Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;					//Windows Style
	}

	AdjustWindowRectEx(&wndRect, dwStyle, FALSE, dwExStyle); // Adjust window to true requested size

	m_hWindow = CreateWindowEx(
		dwExStyle,
		className.c_str(),
		className.c_str(),
		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0,
		wndRect.right - wndRect.left, //Window width
		wndRect.bottom - wndRect.top, //Window height
		NULL,
		NULL,
		m_hInst,
		this);

	if (!m_hWindow) {
		//Logger::Log(_T("Failed to window handle = NULL"), LOGTYPE_ERROR, true);
		return false;
	}

	static PIXELFORMATDESCRIPTOR pfd =		//PFD tells windows how we want things to be
	{
		sizeof(PIXELFORMATDESCRIPTOR),		//Size of this pfd
		1,									//Version 
		PFD_DRAW_TO_WINDOW,					//Format must support window (duh)
		PFD_SUPPORT_OPENGL,					//Format must support opengl
		PFD_DOUBLEBUFFER,					//Must support double buffering
		PFD_TYPE_RGBA,						//Request an rgba format
		m_Bits,								//Color depth
		0, 0, 0, 0, 0, 0,					//Color bits ignored
		0,									//No alpha buffer
		0,									//Shift bit ignored
		0,									//No accumulation buffer
		0, 0, 0, 0,							//Accumulation bits ignored		
		16,									//16bit z-buffer (depth buffer)
		0,									//No stencil buffer
		0,									//No auxiliary buffer
		PFD_MAIN_PLANE,						//Plain drawing layer
		0,									//Reserved
		0,									//Layer Masks ignored
	};

	if (!(m_hDC = GetDC(m_hWindow))) {
		//Logger::Log(_T("Can't Create Device Context"), LOGTYPE_ERROR, true);
		return false;
	}

	if (!(pixelFormat = ChoosePixelFormat(m_hDC, &pfd))) {
		//Logger::Log(_T("Can't Find A Suitable PixelFormat"), LOGTYPE_ERROR, true);
		return false;
	}

	if (!SetPixelFormat(m_hDC, pixelFormat, &pfd)) {
		//Logger::Log(_T("Can't Set The PixelFormat"), LOGTYPE_ERROR, true);
		return false;
	}

	ShowWindow(m_hWindow, SW_SHOW);
	SetForegroundWindow(m_hWindow);
	SetFocus(m_hWindow);

	m_ResizeData.mustResize = true;
	m_ResizeData.newWidth = m_Width;
	m_ResizeData.newHeight = m_height;

	if (!this->CenterWindow()) {
		//Logger::Log(_T("Failed to Center Window"), LOGTYPE_ERROR, true);
		return false;
	}

	//Disable closing button debug window
	HWND hConsoleWnd = GetConsoleWindow();
	if (hConsoleWnd != NULL) {
		HMENU hConsoleMenu = GetSystemMenu(hConsoleWnd, FALSE);
		if (hConsoleMenu != 0) {
			BOOL bRet = RemoveMenu(hConsoleMenu, SC_CLOSE, MF_BYCOMMAND);
			if (!bRet) {
				//Logger::Log(_T("Failed to Remove Close Button"), LOGTYPE_ERROR, true);
				return false;
			}
		}
	}

	//Logger::Log(_T("Window is Initialized"), LOGTYPE_INFO, false);

	//YAY
	return true;
}
bool Window::Update(Context& context) {
	if (WINDOW != this) {
		WINDOW = this;
	}

	return true;
}
bool Window::Shutdown() {
	if (m_bFullscreen) {
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(true);
	}

	//Are we able to release the DC
	//Set DC to null
	if (m_hDC && !ReleaseDC(m_hWindow, m_hDC)) {
		//Logger::Log(_T("Release DC failed"), LOGTYPE_ERROR, true);
		m_hDC = NULL;
		return false;
	}

	//Are we able to destroy the window
	//Set window to null
	if (m_hWindow && !DestroyWindow(m_hWindow)) {
		//Logger::Log(_T("Could not destroy the window"), LOGTYPE_ERROR, true);
		m_hWindow = NULL;
		return false;
	}

	//Are we able to unregister class
	//Set instance to null
	if (!UnregisterClass(m_title.c_str(), m_hInst)) {
		//Logger::Log(_T("Could not unregister class"), LOGTYPE_ERROR, true);
		m_hInst = NULL;
		return false;
	}

	return true;
}

bool Window::CenterWindow() {
	DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
	DWORD dwStyle = WS_OVERLAPPEDWINDOW;

	RECT R = { 0, 0, m_Width, m_height };

	//Calculates the required size of the window rectange
	//The window rectangle can then be passed to the createwindow function to create a window whose client area is desired size

	AdjustWindowRectEx(&R, dwStyle, false, dwExStyle);

	int windowWidth = R.right - R.left;
	int windowHeight = R.bottom - R.top;

	int posx = GetSystemMetrics(SM_CXSCREEN) / 2 - windowWidth / 2;
	int posy = GetSystemMetrics(SM_CYSCREEN) / 2 - windowHeight / 2;

	if (SetWindowPos(m_hWindow, NULL, posx, posy, windowWidth, windowHeight, SWP_SHOWWINDOW) == NULL) {
		return false;
	}

	return true;
}