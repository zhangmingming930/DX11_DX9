#include"DX9DemoBase.h"

DX9DemoBase::DX9DemoBase():
m_pD3D(NULL),
m_pd3dDevice(NULL)
{

}

DX9DemoBase::~DX9DemoBase()
{
	shutdown();
}


HRESULT
DX9DemoBase:: intialize(HINSTANCE hInstance, HWND hwnd )
{
	 m_hInstance = hInstance;
	 m_hWnd =  hwnd;


	// 创建 3D 对象.
	if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )       return E_FAIL;
	// 告诉系统设置需要创建的参数，就是填充 D3DPRESENT_PARAMETERS 的参数
	D3DPRESENT_PARAMETERS d3dpp; 
	ZeroMemory( &d3dpp, sizeof( d3dpp ));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.EnableAutoDepthStencil = FALSE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.BackBufferWidth             = 1920;
	d3dpp.BackBufferHeight             = 1080;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	// 创建设备
	if( FAILED( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pd3dDevice ) ) )  return E_FAIL;
	
	return LoadContent( );
}

void 
DX9DemoBase::  shutdown ()
{

	UnLoadContent();

	SAFE_RELEASE( m_pD3D );
	SAFE_RELEASE( m_pd3dDevice );
}



HRESULT 
DX9DemoBase:: LoadContent()  //载入具体的Demo自己的内容，如纹理，几何体，着色器
 { 
	 //重载，进行相关实现
	return S_OK;
 }

void 
DX9DemoBase:: UnLoadContent()
 {
 //重载，进行释放相关实现

 }