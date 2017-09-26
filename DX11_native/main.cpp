#include <windows.h>
#include <memory>
#include "Tex_effect_Demo.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );

	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof( WNDCLASSEX ) ;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	wndClass.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"DX11tex";

	if( !RegisterClassEx( &wndClass ) )
		return -1;

	RECT rc = { 0, 0, 1920,  1080};
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );

	HWND hwnd = CreateWindow( L"DX11tex", L"DX11effect.demo", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		NULL, NULL, hInstance, NULL );


	if (!hwnd)
		return -1;

	ShowWindow(hwnd, nShowCmd);

	std::auto_ptr<Dx11DemoBase> demo(new texDemo());//使用智能指针
	//获取对某个对象的所有权(ownership)  动态分配对象以及当对象不再需要时自动执行清理。

	/*Dx11DemoBase* demo;  //引用型
	texDemo demo_t;
	demo = &demo_t;*/

	bool result = demo->Initialize(hInstance, hwnd);
	if (!result)
		return -1;

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		demo->Update(0.0f);
		demo->Render();
		break;
	}

	demo->ShutDown();
	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hDc;
	switch (message)
	{
	case WM_PAINT:
		hDc = BeginPaint(hWnd, &paintStruct);
		EndPaint(hWnd, &paintStruct);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}