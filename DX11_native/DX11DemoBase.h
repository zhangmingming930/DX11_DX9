#pragma once
#include <d3d11.h>
#include <D3DX11.h>
#include <DxErr.h>

#include <d3d11.h>
#include <d3dx11.h>
#include <d3dcompiler.h>
#include <xnamath.h>

class Dx11DemoBase
{
public:
    Dx11DemoBase();
    virtual ~Dx11DemoBase();

    bool Initialize(HINSTANCE hInstance, HWND hWnd);
    void ShutDown();//�ͷ��ڴ�

    virtual bool LoadContent();//��������Demo�Լ������ݣ������������壬��ɫ��
    virtual void UnLoadContent();//�ͷž���Demo�е�����

    virtual void Update(float dt) = 0;
    virtual void Render() = 0;

protected:
    HINSTANCE m_hInstance;
    HWND m_hWnd;

    D3D_DRIVER_TYPE m_driverType;
    D3D_FEATURE_LEVEL m_featureLevel;

    ID3D11Device* m_pd3dDevice;
    ID3D11DeviceContext* m_pImmediateContext;
    IDXGISwapChain* m_pSwapChain;
    ID3D11RenderTargetView* m_pRenderTargetView;
};