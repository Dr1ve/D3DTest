#include "MyRender.h"
#include "MyInput.h"


int main()
{
#ifndef _DEBUG
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);
#endif // _DEBUG

	Framework framework;

	MyRender *render = new MyRender();
	MyInput *input = new MyInput(render);
	
	FrameworkDesc desc;
	desc.wnd.width = 1024;
	desc.wnd.height = 768;
	/*desc.wnd.width = 640;
	desc.wnd.height = 480;*/
	desc.render = render;
	
	framework.Init(desc);
	framework.AddInputListener(input);

	framework.Run();
	
	framework.Close();

	delete input;

	return 0;
}