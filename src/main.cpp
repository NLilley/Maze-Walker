#include <windows.h>		
#include "middleware\glew\include\GL\glew.h"
#include <gl\gl.h>			
#include <gl\glu.h>
#include "main.h"
#include "GLController.h"
#include "World.h"
#include <time.h>
#include <iostream>
#include <fstream>


MyApplication application;			

LRESULT CALLBACK WndProc(	HWND	hWnd,			
						 UINT	uMsg,			
						 WPARAM	wParam,			
						 LPARAM	lParam)			
{
	switch (uMsg)									
	{
	case WM_ACTIVATE:							
		{
			if (!HIWORD(wParam))					
			{
				application.isWindowActive=TRUE;						
			}
			else
			{
				application.isWindowActive=FALSE;						
			}

			return 0;								
		}

	case WM_SYSCOMMAND:							
		{
			switch (wParam)							
			{
			case SC_SCREENSAVE:					
			case SC_MONITORPOWER:				
				return 0;							
			}
			break;									
		}

	case WM_CLOSE:								
		{
			PostQuitMessage(0);						
			return 0;								
		}

	case WM_KEYDOWN:							
		{
			application.keyDown(wParam);					
			return 0;								
		}

	case WM_KEYUP:								
		{
			application.keyUp(wParam);				
			return 0;								
		}

	case WM_SIZE:								
		{
			application.glController.ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								
		}
	}


	return DefWindowProc(hWnd,uMsg,wParam,lParam); //Get rid of all unhandled messages by passing them to DefWindowProc.
}

int WINAPI WinMain(	HINSTANCE	hInstance,			
				   HINSTANCE	hPrevInstance,		
				   LPSTR		lpCmdLine,			
				   int			nCmdShow)			
{
	MSG		msg;									
	BOOL	isDone=FALSE;		

	SetCursorPos(application.screenWidth / 2, application.screenHeight / 2);
	GetCursorPos(&application.mousePosition);

	if (!application.createProgramWindow(WINDOW_TITLE,application.screenWidth,application.screenHeight, application.screenBPP, application.isFullscreen))
	{
		return 0;									
	}

	while(!isDone)									
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	
		{
			if (msg.message==WM_QUIT)				
			{

				isDone=TRUE;							
			}
			else								
			{
				TranslateMessage(&msg);				
				DispatchMessage(&msg);				
			}
		}
		else										
		{
			if(application.currentTime != GetTickCount())
			{
				application.timeDelta = GetTickCount() - application.currentTime;
				application.currentTime = GetTickCount();
			}

			/*
			std::ofstream logStream;
			logStream.open("fpsStream.txt",std::ios::app);
			logStream << "\n" << application.timeDelta;
			logStream.close();
			*/

			//While the program is running.
			if (application.isWindowActive)								
			{
				if (application.keys[VK_ESCAPE])				
				{
					isDone=TRUE;					
				}
				else								
				{

					application.pollInput();

					application.world.performBotAI(application.timeDelta);

					application.glController.drawWorld(&application.world);	

					if(application.isDrawSplashScreen) {
						application.glController.drawSplashScreen();
					}

					//currently unecessary
					//world.printWorldMap();

					SwapBuffers(application.hDC);				
				}
			}



		}
	}

	application.killProgramWindow();									
	return (msg.wParam);							
}

GLvoid MyApplication::killProgramWindow()				
{
	if (isFullscreen)									
	{
		ChangeDisplaySettings(NULL,0);					
		ShowCursor(TRUE);								
	}


	if (hRC)											
	{
		if (!wglMakeCurrent(NULL,NULL))					
		{
			MessageBox(NULL,L"Release Of DC And RC Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))					
		{
			MessageBox(NULL,L"Release Rendering Context Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					
	{
		MessageBox(NULL,L"Release Device Context Failed.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										
	}

	if (hWnd && !DestroyWindow(hWnd))					
	{
		MessageBox(NULL,L"Could Not Release hWnd.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										
	}

	if (!UnregisterClass(L"OpenGL",hInstance))			
	{
		MessageBox(NULL,L"Could Not Unregister Class.",L"SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									
	}

	//cleanup the memory used in glController.
	application.glController.cleanup();
}

BOOL MyApplication::createProgramWindow(LPCWSTR title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			
	WNDCLASS	wc;						
	DWORD		dwExStyle;				
	DWORD		dwStyle;				
	RECT		WindowRect;				

	WindowRect.left=(long)0;			
	WindowRect.right=(long)width;		
	WindowRect.top=(long)0;				
	WindowRect.bottom=(long)height;		

	hInstance			= GetModuleHandle(NULL);				
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	
	wc.lpfnWndProc		= (WNDPROC) WndProc;					
	wc.cbClsExtra		= 0;									
	wc.cbWndExtra		= 0;									
	wc.hInstance		= hInstance;							
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			
	wc.hbrBackground	= NULL;									
	wc.lpszMenuName		= NULL;									
	wc.lpszClassName	= L"OpenGL";							

	if (!RegisterClass(&wc))									
	{
		MessageBox(NULL,L"Failed To Register The Window Class.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											
	}

	if (isFullscreen)											
	{
		DEVMODE dmScreenSettings;								
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		
		dmScreenSettings.dmPelsWidth	= width;				
		dmScreenSettings.dmPelsHeight	= height;				
		dmScreenSettings.dmBitsPerPel	= bits;					
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;


		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{

			if (MessageBox(NULL,L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?",L"NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				isFullscreen=FALSE;		
			}
			else
			{

				MessageBox(NULL,L"Program Will Now Close.",L"ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;							
			}
		}
	}

	if (isFullscreen)										
	{
		dwExStyle=WS_EX_APPWINDOW;							
		dwStyle=WS_POPUP;									
		ShowCursor(FALSE);									
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;		
		dwStyle=WS_OVERLAPPEDWINDOW;						
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							
		L"OpenGL",							
		title,								
		dwStyle |							
		WS_CLIPSIBLINGS |					
		WS_CLIPCHILDREN,					
		0, 0,							// Window Position
		WindowRect.right-WindowRect.left,	// Calculate Window Width
		WindowRect.bottom-WindowRect.top,	// Calculate Window Height
		NULL,								
		NULL,								
		hInstance,							
		NULL)))								
	{
		application.killProgramWindow();								
		MessageBox(NULL,L"Window Creation Error.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	static	PIXELFORMATDESCRIPTOR pfd=				
	{
		sizeof(PIXELFORMATDESCRIPTOR),				
		1,											
		PFD_DRAW_TO_WINDOW |						
		PFD_SUPPORT_OPENGL |						
		PFD_DOUBLEBUFFER,							
		PFD_TYPE_RGBA,								
		bits,										
		0, 0, 0, 0, 0, 0,							
		0,											
		0,											
		0,											
		0, 0, 0, 0,									
		16,											
		0,											
		0,											
		PFD_MAIN_PLANE,								
		0,											
		0, 0, 0										
	};

	if (!(hDC=GetDC(hWnd)))							
	{
		application.killProgramWindow();								
		MessageBox(NULL,L"Can't Create A GL Device Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	
	{
		application.killProgramWindow();								
		MessageBox(NULL,L"Can't Find A Suitable PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		
	{
		application.killProgramWindow();								
		MessageBox(NULL,L"Can't Set The PixelFormat.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	if (!(hRC=wglCreateContext(hDC)))				
	{
		application.killProgramWindow();								
		MessageBox(NULL,L"Can't Create A GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							
	}

	if(!wglMakeCurrent(hDC,hRC))				
	{
		application.killProgramWindow();							
		MessageBox(NULL,L"Can't Activate The GL Rendering Context.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								
	}

	ShowWindow(hWnd,SW_SHOW);						
	SetForegroundWindow(hWnd);						
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	glController.ReSizeGLScene(width, height);		

	if (!glController.InitGL())									
	{
		application.killProgramWindow();								
		MessageBox(NULL,L"Initialization Failed.",L"ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;							
	}

	return TRUE;									
}

void MyApplication::keyDown(int key)
{

	keys[key] = true;

}

void MyApplication::keyUp(int key)
{
	keys[key] = false;
}

void MyApplication::pollInput()
{
	ShowCursor(false);

	POINT newMousePosition;
	GetCursorPos(&newMousePosition);
	glm::vec3 positionDelta;

	application.currentTime = GetTickCount();

	application.mouseDelta.x = newMousePosition.x - application.mousePosition.x;
	application.mouseDelta.y = newMousePosition.y - application.mousePosition.y;

	application.glController.worldCamera.roateX(mouseDelta.x);
	application.glController.worldCamera.roateY(mouseDelta.y);

	SetCursorPos(screenWidth / 2, screenHeight /2);



	if(keys[VK_F1])
	{
		keys[VK_F1]=FALSE;					

		application.killProgramWindow();						
		application.isFullscreen =! application.isFullscreen;			

		if (!application.createProgramWindow(WINDOW_TITLE, application.screenWidth, application.screenHeight, application.screenBPP, application.isFullscreen))
		{
			PostQuitMessage(-1);
		}
	}

	if(keys[VK_F2])
	{
		application.glController.worldCamera.cameraLocation = world.bot->botPosition;
	}

	if(keys[VK_F3])
	{
		if(application.glController.drawLines == false)
		{
			application.glController.drawLines = true;
			keys[VK_F3] = false;
		}

		else if(application.glController.drawLines == true)
		{
			application.glController.drawLines = false;
			keys[VK_F3] = false;
		}

	}

	if(keys[VK_F4])
	{
		application.isDrawSplashScreen = !application.isDrawSplashScreen;
		keys[VK_F4] = false;
	}

	if(keys[VK_RIGHT])
	{
		positionDelta = ((glController.worldCamera.cameraMoveSpeed * timeDelta)  * 
			glm::normalize(glm::cross(glController.worldCamera.viewDirection,glController.worldCamera.up)));

		application.glController.worldCamera.cameraLocation += positionDelta;

		world.movePosition(positionDelta);

	}

	if(keys[VK_LEFT])
	{
		positionDelta =  ((glController.worldCamera.cameraMoveSpeed * timeDelta)  * 
			glm::normalize(glm::cross(glController.worldCamera.viewDirection,glController.worldCamera.up)));

		application.glController.worldCamera.cameraLocation -= positionDelta;
		world.movePosition(-1.0f * positionDelta);
	}

	if(keys[VK_UP])
	{
		positionDelta = ((glController.worldCamera.cameraMoveSpeed * timeDelta)  * glController.worldCamera.viewDirection);

		application.glController.worldCamera.cameraLocation += positionDelta;
		world.movePosition(positionDelta);
	}

	if(keys[VK_DOWN])
	{
		positionDelta = ((glController.worldCamera.cameraMoveSpeed * timeDelta)  * glController.worldCamera.viewDirection);

		application.glController.worldCamera.cameraLocation -= positionDelta;
		world.movePosition(-1.0f * positionDelta);
	}




}