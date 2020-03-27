#include "stdafx.h"
#include "OISInputManager.h"
#include "OISException.h"
#include "OISMouse.h"
#include "OISEvents.h"
#include "OISForceFeedback.h"
#include <iostream>
#include <vector>
#include <sstream>
#include "resource.h"
using namespace OIS;
#include "ZKeyboardMouseHook.h"

ZKeyboardMouseHook* ZKeyboardMouseHook::m_pInstance = NULL;
class EventHandler : public MouseListener
{
public:
	EventHandler() {}
	~EventHandler() {}
	bool mouseMoved( const MouseEvent &arg ) {
		const OIS::MouseState& s = arg.state;
		std::cout << "\nMouseMoved: Abs("
				  << s.X.abs << ", " << s.Y.abs << ", " << s.Z.abs << ") Rel("
				  << s.X.rel << ", " << s.Y.rel << ", " << s.Z.rel << ")";
		return true;
	}
	bool mousePressed( const MouseEvent &arg, MouseButtonID id ) {
		const OIS::MouseState& s = arg.state;
		std::cout << "\nMouse button #" << id << " pressed. Abs("
				  << s.X.abs << ", " << s.Y.abs << ", " << s.Z.abs << ") Rel("
				  << s.X.rel << ", " << s.Y.rel << ", " << s.Z.rel << ")";
		return true;
	}
	bool mouseReleased( const MouseEvent &arg, MouseButtonID id ) {
		const OIS::MouseState& s = arg.state;
		std::cout << "\nMouse button #" << id << " released. Abs("
				  << s.X.abs << ", " << s.Y.abs << ", " << s.Z.abs << ") Rel("
				  << s.X.rel << ", " << s.Y.rel << ", " << s.Z.rel << ")";
		return true;
	}
};
ZKeyboardMouseHook::ZKeyboardMouseHook() 
{
	m_pInstance = this;
	ParamList pl;
	g_InputManager = InputManager::createInputSystem(pl);
	g_InputManager->enableAddOnFactory(InputManager::AddOn_All);
	g_m = (Mouse*)g_InputManager->createInputObject( OISMouse, true );
	EventHandler handler;
	g_m->setEventCallback( &handler );
	const MouseState &ms = g_m->getMouseState();
	ms.width = 1024;
	ms.height = 768;
}
ZKeyboardMouseHook::~ZKeyboardMouseHook()
{
	ZKeyboardMouseHook::m_pInstance = NULL;
}
void ZKeyboardMouseHook::Thread() {
try
	{

		while(true)
		{
			  Sleep(90);
			  MSG  msg;
			  while( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
			  {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			  }

			if( g_m )
			{
				g_m->capture();
				if( !g_m->buffered() )
					handleNonBufferedMouse();
			}
		}
	}
	catch( const Exception &ex )
	{
		#if defined OIS_WIN32_PLATFORM
		  MessageBox( NULL, ex.eText, "An exception has occurred!", MB_OK |
				MB_ICONERROR | MB_TASKMODAL);
		#else
		  std::cout << "\nOIS Exception Caught!\n" << "\t" << ex.eText << "[Line "
			<< ex.eLine << " in " << ex.eFile << "]\nExiting App";
		#endif
	}
	catch(std::exception &ex)
	{
		std::cout << "Caught std::exception: what = " << ex.what() << std::endl;
	}

	//Destroying the manager will cleanup unfreed devices
	if( g_InputManager )
		InputManager::destroyInputSystem(g_InputManager);
}

LRESULT DlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return FALSE;
}

void ZKeyboardMouseHook::handleNonBufferedMouse()
{
	const MouseState &ms = g_m->getMouseState();
	std::cout << "\nMouse: Abs(" << ms.X.abs << " " << ms.Y.abs << " " << ms.Z.abs
		<< ") B: " << ms.buttons << " Rel(" << ms.X.rel << " " << ms.Y.rel << " " << ms.Z.rel << ")";
}