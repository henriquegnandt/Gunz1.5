#include <windows.h>
class ZMouseHook
{
private:
protected:
	static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
	static DWORD WINAPI StayAlive(LPVOID pJobContext);
public:
HHOOK MouseHook;
	void StayAlive();
	void InstallHook(HINSTANCE a);
};