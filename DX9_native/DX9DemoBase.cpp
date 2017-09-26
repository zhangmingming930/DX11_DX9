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


	// ���� 3D ����.
	if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )       return E_FAIL;
	// ����ϵͳ������Ҫ�����Ĳ������������ D3DPRESENT_PARAMETERS �Ĳ���
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
	// �����豸
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
DX9DemoBase:: LoadContent()  //��������Demo�Լ������ݣ������������壬��ɫ��
 { 
	 //���أ��������ʵ��
	return S_OK;
 }

void 
DX9DemoBase:: UnLoadContent()
 {
 //���أ������ͷ����ʵ��

 }