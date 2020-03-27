#include <windows.h>
class ZKeyboardMouseHook
{
private:
protected:
	static ZKeyboardMouseHook*	m_pInstance;
    LRESULT DlgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	InputManager *g_InputManager;
	Mouse *g_m;
	void handleNonBufferedMouse();
	public:
	void Thread();
	ZKeyboardMouseHook();
	virtual ~ZKeyboardMouseHook();
};