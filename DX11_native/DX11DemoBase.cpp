#include "DX11DemoBase.h"

Dx11DemoBase::Dx11DemoBase() :
m_driverType(D3D_DRIVER_TYPE_NULL), 
m_featureLevel(D3D_FEATURE_LEVEL_11_0),
m_pd3dDevice(NULL),
m_pImmediateContext(NULL), 
m_pSwapChain(NULL),
m_pRenderTargetView(NULL)
{


}

Dx11DemoBase::~Dx11DemoBase()
{
    ShutDown();
}

bool Dx11DemoBase::LoadContent()
{
    return true;
}

void Dx11DemoBase::UnLoadContent()
{

}

void Dx11DemoBase::ShutDown()
{
    UnLoadContent();
    if (m_pRenderTargetView) m_pRenderTargetView->Release();
    if (m_pSwapChain) m_pSwapChain->Release();
    if (m_pd3dDevice) m_pd3dDevice->Release();
    if (m_pImmediateContext) m_pImmediateContext->Release();

    m_pRenderTargetView = NULL;
    m_pSwapChain = NULL;
    m_pd3dDevice = NULL;
    m_pImmediateContext = NULL;
}

//初始化
bool Dx11DemoBase::Initialize(HINSTANCE hInstance, HWND hWnd)
{
    HRESULT result;
    m_hInstance = hInstance;
    m_hWnd = hWnd;
	 
	RECT rc =  { 0, 0, 1920, 1080 } ;

    //GetClientRect(m_hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    D3D_DRIVER_TYPE driverTypes[] = 
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Windowed = true;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; ++driverTypeIndex)
    {
        result = D3D11CreateDeviceAndSwapChain(
            NULL,
            driverTypes[driverTypeIndex],
            NULL,
            createDeviceFlags,
            featureLevels,
            numFeatureLevels,
            D3D11_SDK_VERSION,
            &sd,
            &m_pSwapChain,
            &m_pd3dDevice,
            &m_featureLevel,
            &m_pImmediateContext
            );
        if (SUCCEEDED(result))
        {
            m_driverType = driverTypes[driverTypeIndex];
            break;
        }
    }

    if (FAILED(result))
    {
        return false;
    }

    ID3D11Texture2D *pBackBuffer = NULL;
    result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(result))
    {
        return false;
    }

    result = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
    if (pBackBuffer)
        pBackBuffer->Release();

    if (FAILED(result))
    {
        return false;
    }

    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);
	//将后台缓存作为渲染目标，让Direct3D 11能够在后台缓存上进行渲染。


	//在Direct3D 11中，没有默认的视口，必须手动设置。
    D3D11_VIEWPORT vp;
	 vp.Width  = static_cast<float>(width);//用c++的static_cast转换类型是个好习惯
	vp.Height = static_cast<float>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    
    m_pImmediateContext->RSSetViewports(1, &vp);

    return LoadContent();
}