#include <windows.h>
#include<memory>

#include"DX9DemoBase.h"
#include"SampleDemo.h"


//窗口的回调函数
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

//****wWinMain函数，程序入口点函数************************************** 

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow )
{
	UNREFERENCED_PARAMETER( prevInstance );
	UNREFERENCED_PARAMETER( cmdLine );

	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof( WNDCLASSEX ) ;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
	wndClass.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"DX9sample";

	if( !RegisterClassEx( &wndClass ) )
		return -1;

	RECT rc = { 0, 0, 1920,  1080};
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );

	HWND hwnd = CreateWindow( L"DX9sample", L"DX9实验.demo", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top,
		NULL, NULL, hInstance, NULL );

	if( !hwnd )
		return -1;

	ShowWindow( hwnd, cmdShow );

	SampleDemo demo;

	// Demo的初始化
	HRESULT result = demo.intialize( hInstance, hwnd );

	if( result == S_FALSE )
		return -1;

	MSG msg = { 0 };

	while( msg.message != WM_QUIT )
	{
		if( PeekMessage( &msg, 0, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

		// 更新以及绘制
		demo.Update( 0.0f );
		demo.Render( );
		break;
	}

	// Demo善后工作
	demo.shutdown( );

	return static_cast<int>( msg.wParam );
}



//****消息处理函数*********************************** 
LRESULT CALLBACK WndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT paintStruct;
	HDC hDC;

	switch( message )
	{
	case WM_PAINT:
		hDC = BeginPaint( hwnd, &paintStruct );
		EndPaint( hwnd, &paintStruct );
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam); 
	}

	return 0;
}
