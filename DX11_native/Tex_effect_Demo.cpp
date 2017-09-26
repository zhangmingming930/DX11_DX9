// Disable warnings generated by the Windows header files.
#pragma warning(disable : 4996)
#pragma warning(disable : 4244)
#pragma warning(disable : 4305)
#include "Tex_effect_Demo.h"

#include <stdio.h>
#include<assert.h>
#include <math.h>

#include<D3DX10math.h>
#define RGBA2VEC4(c) D3DXVECTOR4((c & 0x0ff) / 255.0f,((c >> 8) & 0x0ff) / 255.0f,((c >> 16) & 0x0ff) / 255.0f,((c >> 24) & 0x0ff) / 255.0f)

XMMATRIX g_world;
XMMATRIX g_view;
XMMATRIX g_proj;

struct VertexPos
{
	XMFLOAT3 pos;
	XMFLOAT2 tex0;
};

//
void GenerateTexMatrix( RECT *rcSrcImage,UINT uSrcWdith,UINT uSrcHeight,XMMATRIX * matTex)
{
	*matTex = XMMatrixIdentity();
	matTex->_11 = (rcSrcImage->right - rcSrcImage->left) / (float)uSrcWdith;
	matTex->_22 = (rcSrcImage->bottom - rcSrcImage->top) / (float)uSrcHeight;
	matTex->_31 = rcSrcImage->left / (float)uSrcWdith;
	matTex->_32 = rcSrcImage->top / (float)uSrcHeight;
}
void GenerateTexMatrix( TexResBufinfo *pSrcDef,XMMATRIX* matTex)
{	
	GenerateTexMatrix(&pSrcDef->rcImage,pSrcDef->BaseWidth,pSrcDef->BaseHeight,matTex);
}
//
texDemo::texDemo() 
: m_pInputLayout(NULL), /*m_pIndexBuffer(NULL),*/m_pVertexBuffer(NULL),m_pEffect(NULL),
m_pTechnique(NULL),m_pMatWorldViewProjVariable(NULL),m_pMatTexVariable(NULL),
m_pVecOffsetVariable(NULL),m_pVecSizeVariable(NULL),
m_pTexColorVariable(NULL),m_pTexCoefVariable(NULL),m_pTecP4(NULL),m_pTecP8(NULL),
m_pTex(NULL),m_pTexSRV(NULL),
m_pMidTex(NULL),m_pMidRTV(NULL),m_pRTtexSRV(NULL)
{

	m_pCoefTex = NULL;
	m_pCoefTexSRV = NULL;

	m_ptempTex = NULL;
	m_ptempRTV = NULL;
	m_ptempSRV = NULL;

	m_pVecHeightVariable = NULL;
	m_pVecWidthVariable = NULL;

	g_world = XMMatrixIdentity();
	g_view = XMMatrixIdentity();
	g_proj = XMMatrixIdentity();

	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&m_world, I);
	XMStoreFloat4x4(&m_view, I);
	XMStoreFloat4x4(&m_proj, I);
}
//
texDemo::~texDemo()
{
}
//
void texDemo::UnLoadContent()
{
	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();
	if (m_pInputLayout)
		m_pInputLayout->Release();
	if (m_pEffect)
		m_pEffect->Release();
	m_pVertexBuffer = 0;
	m_pInputLayout = 0;
	m_pEffect = 0;
//SAFE_RELEASE(m_pIndexBuffer);
	
	SAFE_RELEASE(m_pTex);  //ԭsrc
	SAFE_RELEASE(m_pTexSRV); 

	SAFE_RELEASE(m_pCoefTex);//coef
	SAFE_RELEASE(m_pCoefTexSRV); 

	SAFE_RELEASE(m_pMidTex);//dst
	SAFE_RELEASE(m_pMidRTV);  
	SAFE_RELEASE(m_pRTtexSRV);
	
}


//Coefϵ������
bool texDemo::CreateResouce()
{
	D3D11_TEXTURE2D_DESC descTex2D;
	ZeroMemory(&descTex2D, sizeof (descTex2D));
	descTex2D.Width = 2;																										//RT���� ����
	descTex2D.Height = 16;
	descTex2D.MipLevels = 1;
	descTex2D.ArraySize = 1;
	descTex2D.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	descTex2D.SampleDesc.Count = 1;
	descTex2D.SampleDesc.Quality = 0;
	descTex2D.BindFlags = D3D11_BIND_SHADER_RESOURCE;  //�󶨹��߽׶α�־
	descTex2D.Usage = D3D11_USAGE_DEFAULT;
	descTex2D.MiscFlags = 0;
	descTex2D.CPUAccessFlags = 0;

	//ID3D11Texture2D* m_pMidTex = NULL;


	if (FAILED(m_pd3dDevice->CreateTexture2D
		(&descTex2D, NULL, (ID3D11Texture2D**)&m_pCoefTex)))
	{
		DXTRACE_MSG( L"����coef����ʧ��!" ); return false; 
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
	memset(&descSRV, 0, sizeof(descSRV) );
	descSRV.Format = descTex2D.Format;
	descSRV.ViewDimension =D3D11_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2D.MipLevels = descTex2D.MipLevels;

	if (FAILED(m_pd3dDevice->CreateShaderResourceView
		(m_pCoefTex, NULL, &m_pCoefTexSRV)))
	{
		SAFE_RELEASE(m_pCoefTex);
		DXTRACE_MSG( L"����coef������ɫ����Դ��ͼʧ��!" ); return false; 
	}

	if(0)
	{
		D3DX11SaveTextureToFile(m_pImmediateContext,m_pCoefTex,D3DX11_IFF_BMP,L"up_coef.bmp");
	}
	return true;
}
//
 bool texDemo::RT(ID3D11Resource**tempTex,ID3D11ShaderResourceView**temptexSRV,ID3D11RenderTargetView**temptexRTV,UINT width,UINT height)
 {
	 D3D11_TEXTURE2D_DESC descTex2D;
	 ZeroMemory(&descTex2D, sizeof (descTex2D));
	 descTex2D.Width = width;																										//RT���� ����
	 descTex2D.Height = height;
	 descTex2D.MipLevels = 1;
	 descTex2D.ArraySize = 1;
	 descTex2D.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	 /*descTex2D.Format = DXGI_FORMAT_R32G32B32A32_FLOAT ;*/
	 descTex2D.SampleDesc.Count = 1;
	 descTex2D.SampleDesc.Quality = 0;
	 descTex2D.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;  //�󶨹��߽׶α�־
	 descTex2D.Usage = D3D11_USAGE_DEFAULT;
	 descTex2D.MiscFlags = 0;
	 descTex2D.CPUAccessFlags = 0;

	 //ID3D11Texture2D* m_pMidTex = NULL;


	 if (FAILED(m_pd3dDevice->CreateTexture2D
		 (&descTex2D, NULL, (ID3D11Texture2D**)tempTex)))
	 {
		 DXTRACE_MSG( L"��������ʧ��!" ); return false; 
	 }

	 D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
	 memset(&descSRV, 0, sizeof(descSRV) );
	 descSRV.Format = descTex2D.Format;
	 descSRV.ViewDimension =D3D11_SRV_DIMENSION_TEXTURE2D;
	 descSRV.Texture2D.MipLevels = descTex2D.MipLevels;

	 if (FAILED(m_pd3dDevice->CreateShaderResourceView
		 (*tempTex, NULL, temptexSRV)))
	 {
		 SAFE_RELEASE(*tempTex);
		 DXTRACE_MSG( L"�����м�������ɫ����Դ��ͼʧ��!" ); return false; 
	 }


	 D3D11_RENDER_TARGET_VIEW_DESC descRT;
	 memset(&descRT, 0, sizeof(descRT) );
	 descRT.Format = descTex2D.Format;
	 descRT.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	 descRT.Texture2D.MipSlice = 0;                                                  //The index of the mipmap level to use mip slice

	 if (FAILED( m_pd3dDevice->CreateRenderTargetView
		 (*tempTex, NULL, temptexRTV)))
	 {
		 SAFE_RELEASE(*tempTex);
		 DXTRACE_MSG( L"�����м�������ȾĿ����ͼʧ��!" ); return false; 
	 }

	if(1)
	{
	Dump_RTT(L"mid_RTT.BMP");
	}
	return true;
 }
 //����Դ����
 bool texDemo::CreateSrc(ID3D11Resource**tempTex,ID3D11ShaderResourceView**temptexSRV,UINT width,UINT height)
 {
	 D3D11_TEXTURE2D_DESC descTex2D;
	 ZeroMemory(&descTex2D, sizeof (descTex2D));
	 descTex2D.Width = width;																										//RT���� ����
	 descTex2D.Height = height;
	 descTex2D.MipLevels = 1;
	 descTex2D.ArraySize = 1;
	 descTex2D.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	 descTex2D.SampleDesc.Count = 1;
	 descTex2D.SampleDesc.Quality = 0;
	 descTex2D.Usage = D3D11_USAGE_DYNAMIC;
	 descTex2D.BindFlags = D3D11_BIND_SHADER_RESOURCE;  //�󶨹��߽׶α�־
	 descTex2D.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	 descTex2D.MiscFlags = 0;

	 if (FAILED(m_pd3dDevice->CreateTexture2D
		 (&descTex2D, NULL, (ID3D11Texture2D**)tempTex)))
	 {
		 DXTRACE_MSG( L"����Դ����ʧ��!" ); return false; 
	 }

	 //ǿ�����������ڴ�,����Upload����
	 if( D3D11_USAGE_DYNAMIC == descTex2D.Usage)
	 {
		 if (m_pImmediateContext == 0)
			 return false;
		 ID3D11Texture2D* pTexture = (ID3D11Texture2D*)*tempTex;
		 D3D11_MAPPED_SUBRESOURCE map;
		 m_pImmediateContext->Map(pTexture,D3D11CalcSubresource(0,0,1),D3D11_MAP_WRITE_DISCARD,0,&map);
		 m_pImmediateContext->Unmap(pTexture,D3D11CalcSubresource(0,0,1));
	 }

	 D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
	 memset(&descSRV, 0, sizeof(descSRV) );
	 descSRV.Format = descTex2D.Format;
	 descSRV.ViewDimension =D3D11_SRV_DIMENSION_TEXTURE2D;
	 descSRV.Texture2D.MipLevels = descTex2D.MipLevels;

	 if (FAILED(m_pd3dDevice->CreateShaderResourceView
		 (*tempTex, NULL, temptexSRV)))
	 {
		 SAFE_RELEASE(*tempTex);
		 DXTRACE_MSG( L"����Դ������ɫ����Դ��ͼʧ��!" ); return false; 
	 }

	 if(1)
	 {
		 D3DX11SaveTextureToFile(m_pImmediateContext,*tempTex,D3DX11_IFF_BMP,L"up_src.bmp");
	 }
	 return true;
 }

 //
bool texDemo::LoadContent()
{
	HRESULT result;

	if(FAILED(CreateResouce()))
	{
		MessageBox(0, L"����Coef��Դʧ��", 0, 0);
		return false;
	}

	if(FAILED(RT(&m_pMidTex,&m_pRTtexSRV,&m_pMidRTV,DST_WIDTH  ,DST_HEIGHT)))
	{
		MessageBox(0, L"����RTT��Դʧ��", 0, 0);
		return false;
	}

	if(FAILED(CreateSrc(&m_pTex,&m_pTexSRV,RT_WIDTH  ,RT_HEIGHT)))
	{
		MessageBox(0, L"����Դ��Դʧ��", 0, 0);
		return false;
	}
	
	//��������
	Up_src();


	//���㻺��
	VertexPos vertices[] =
	{
		{ XMFLOAT3(  0.5f,  0.5f, 0.0f ), XMFLOAT2( 1.0f, 0.0f ) },
		{ XMFLOAT3(  0.5f, -0.5f, 0.0f ), XMFLOAT2( 1.0f, 1.0f ) },
		{ XMFLOAT3( -0.5f, -0.5f, 0.0f ), XMFLOAT2( 0.0f, 1.0f ) },

		{ XMFLOAT3( -0.5f, -0.5f, 0.0f ), XMFLOAT2( 0.0f, 1.0f ) },
		{ XMFLOAT3( -0.5f,  0.5f, 0.0f ), XMFLOAT2( 0.0f, 0.0f ) },
		{ XMFLOAT3(  0.5f,  0.5f, 0.0f ), XMFLOAT2( 1.0f, 0.0f ) }
	};

	D3D11_BUFFER_DESC vertexDesc;
	ZeroMemory(&vertexDesc, sizeof(vertexDesc));
	vertexDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexDesc.CPUAccessFlags = 0;
	vertexDesc.ByteWidth = sizeof(VertexPos)* 6;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(resourceData));
	resourceData.pSysMem = vertices;

	result = m_pd3dDevice->CreateBuffer(&vertexDesc, &resourceData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		return false;
	}


	

	if(0)
	{
		//�������� һ����λ ֱ�Ӵ���������ͼ
		/* result = D3DX11CreateShaderResourceViewFromFile( m_pd3dDevice,
		L"ColorMap.jpg", 0, 0, &m_pTexSRV, 0 );
		if (FAILED(result))
		{
		MessageBox(NULL, L"create texture failed!", L"error",MB_OK);
		return false;
		}*/

	   //��Ӳ����������;
	   D3DX11_IMAGE_LOAD_INFO imageLoadInfo;
	   imageLoadInfo.MipLevels = 1;
	   D3DX11_IMAGE_INFO imageInfo;
	  // D3DX11GetImageInfoFromFile(L"1.jpg",NULL,&imageInfo,NULL);
	   imageLoadInfo.pSrcInfo = &imageInfo;

	   result = D3DX11CreateTextureFromFile(m_pd3dDevice,	L"1.jpg",&imageLoadInfo,NULL,(ID3D11Resource**)&m_pTex,NULL);

	   // view  Ϊ�������� ��ɫ����Դ��ͼ
	   D3D11_TEXTURE2D_DESC desc;
	   ID3D11Texture2D* pTex2D = (ID3D11Texture2D*)m_pTex;
	   pTex2D->GetDesc(&desc);
	  
	   // ����m_pTex��ͼ m_pTexSRV
	   D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
	   memset(&descSRV, 0, sizeof(descSRV) );
	   descSRV.Format = desc.Format;
	   descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	   descSRV.Texture2D.MipLevels = desc.MipLevels;
	   m_pd3dDevice->CreateShaderResourceView(m_pTex,&descSRV,&m_pTexSRV);

	   if( FAILED( result ) ) { DXTRACE_MSG( L"��ȡ����ͼ��ʧ��!" ); return false;  }

	   SAFE_RELEASE( pTex2D);
	}


	/*DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined _DEBUG || defined DEBUG
	shaderFlags = D3DCOMPILE_DEBUG;
#endif*/

	//Ч���ļ�����
	ID3D10Blob *compiledShader = 0;
	ID3D10Blob *compilationMsgs = 0;
	result = D3DX11CompileFromFile(L"FX/texture.fx", 0, 0, 0, "fx_5_0", D3D10_SHADER_DEBUG,
		0, 0, &compiledShader, &compilationMsgs, 0);
	
	if (FAILED(result))
	{

		if (compilationMsgs != 0)
		{
			MessageBoxA(0, (CHAR*)compilationMsgs->GetBufferPointer(), 0, 0);
			compilationMsgs->Release();
			compilationMsgs = 0;
		}

		MessageBox(0, L"������ɫ������", 0, 0);
		return false;
	}

	result = D3DX11CreateEffectFromMemory(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(),
		0, m_pd3dDevice, &m_pEffect);
	compiledShader->Release();
	if (FAILED(result))
	{
		MessageBox(0, L"������ɫ��ʧ��", 0, 0);
		return false;
	}

	//effect��ȫ�ֱ�����C++����
	m_pTechnique = m_pEffect->GetTechniqueByName("TextureTech");
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("g_worldViewProj")->AsMatrix();
	m_pMatTexVariable= m_pEffect->GetVariableByName("g_matTex")->AsMatrix();
	m_pTexColorVariable = m_pEffect->GetVariableByName("g_tex")->AsShaderResource();


	//for debug
	m_pVecHeightVariable = m_pEffect->GetVariableByName( "g_height" )->AsScalar();
	m_pVecWidthVariable = m_pEffect->GetVariableByName( "g_width" )->AsScalar();



	//��������
	D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numLayoutElements = ARRAYSIZE(solidColorLayout);
	D3DX11_PASS_DESC passDesc;
	m_pTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	result = m_pd3dDevice->CreateInputLayout(solidColorLayout, numLayoutElements, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_pInputLayout);

	return true;
}
//���³���buffer��������ͼ��post_process��
void texDemo::Update(float dt)
{
	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet( 0.0f, 0.0f, -0.50f/tan(D3DX_PI/8.0f), 0.0f );
	XMVECTOR At = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	 g_view  = XMMatrixLookAtLH( Eye, At, Up );

	// Initialize the projection matrix
	 g_proj  = XMMatrixPerspectiveFovLH( D3DX_PI/4, 1.0f, 0.50f, 100.0f );


}

//
void texDemo::Render()
{

	if (m_pImmediateContext == 0)
		return;

	UINT stride = sizeof(VertexPos);
	UINT offset = 0;
	//����������Ϣ��ʽ������Ϣ
	m_pImmediateContext->IASetInputLayout(m_pInputLayout);
	//����Ҫ���Ƶļ�������Ϣ
	m_pImmediateContext->IASetVertexBuffers(0,1,&m_pVertexBuffer,&stride,&offset);
	
	//ָ����λ���������
	m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	///////////////////////////////////////////////////////////////
	//RTT 
	//��Ŀ��󶨵�OM������ϲ��׶Σ�����������ͼ��Ⱦ
	///////////////////////////////////////////////////////////////
	m_pImmediateContext->OMSetRenderTargets(1, &m_pMidRTV, NULL);

	//�����ȾĿ����ͼ
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };//������ɫ
	m_pImmediateContext->ClearRenderTargetView(m_pMidRTV, clearColor);
	//option..............................�����ɫ��Ϣ���⣬�����Լ��������Ⱥ�ģ����Ϣ....................


	TexResBufinfo SrcDef(RT_WIDTH,RT_HEIGHT,0.5f,0.5f);
	SetRect(&SrcDef.rcImage,0,0,RT_WIDTH,RT_HEIGHT);

	TexResBufinfo DstDef(DST_WIDTH ,DST_HEIGHT,0,0);
	SetRect(&DstDef.rcImage,0,0,DST_WIDTH ,DST_HEIGHT);

	TexResBufinfo*	pSrcDef = &SrcDef;
	TexResBufinfo*  pDstDef = &DstDef;

	RECT  rcBorder;
	//SetRect(& rcBorder,0,0,RT_WIDTH,RT_HEIGHT);
	SetRect(& rcBorder,0,0,DST_WIDTH,DST_HEIGHT);

	//VP
	D3D11_VIEWPORT viewport;
	UINT uNum = 1;
	m_pImmediateContext->RSGetViewports(&uNum,&viewport);
	//viewport.Width = static_cast<float>(1920);
	//viewport.Height = static_cast<float>(1080);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = rcBorder.left;		
	viewport.Width = rcBorder.right - rcBorder.left;
	viewport.TopLeftX= rcBorder.top;
	viewport.Height = rcBorder.bottom - rcBorder.top;
	m_pImmediateContext->RSSetViewports( 1, &viewport);
	

	XMMATRIX worldViewProj,matWorld,matTex;

	//��������
	matTex = XMMatrixIdentity();
	GenerateTexMatrix(pSrcDef,&matTex);

	//�������
	matWorld = XMMatrixIdentity();
	matWorld._11 =1.0f; /*pDstDef->GetImageWidth() / (float)viewport.Width;*/
	matWorld._22 =1.0f;/* pDstDef->GetImageHeight() / (float)viewport.Height;*/

	matWorld._41 = pSrcDef->OffsetX - 0.5f;
	matWorld._42 = 0.5f - pSrcDef->OffsetY;

	worldViewProj = matWorld * g_view* g_proj;		


	//effect�б�����ֵ
	m_pVecHeightVariable->SetFloat(pSrcDef->BaseHeight);
	m_pVecWidthVariable->SetFloat(pSrcDef->BaseWidth);

	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<float*>(&worldViewProj));
	m_pMatTexVariable->SetMatrix(reinterpret_cast<float*>(&matTex));
	m_pTexColorVariable->SetResource(m_pTexSRV);//�����ԭʼ������ͼ

	D3DX11_TECHNIQUE_DESC techDesc;
	m_pTechnique->GetDesc(&techDesc);
	m_pTechnique->GetPassByIndex(0)->Apply(0, m_pImmediateContext);
	m_pImmediateContext->Draw(6, 0);
	

	if(1)
	{
	Dump_RTT(L"RTT.BMP");
	Map_RTT();
	}

	///////////////////////////////////////////////////////////////
	//
	//������ʾRT
	///////////////////////////////////////////////////////////////

	//�ָ�Ĭ��������ͼ
	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);
	//�����ȾĿ����ͼ
	float ClearsceneColor[4] = { 0.0f, 0.5f, 0.5f, 1.0f };	//������ɫ
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, ClearsceneColor);

	D3D11_VIEWPORT viewport1;
	viewport1.Width = static_cast<float>(DST_WIDTH);
	viewport1.Height = static_cast<float>(DST_HEIGHT);
	viewport1.MinDepth = 0.0f;
	viewport1.MaxDepth = 1.0f;
	viewport1.TopLeftX = 0.0f;
	viewport1.TopLeftY = 0.0f;
	m_pImmediateContext->RSSetViewports( 1, &viewport1 );


	XMMATRIX matworld_,matCombine,matTex_;
	TexResBufinfo quadDef(DST_WIDTH  ,DST_HEIGHT,0,0);
	SetRect(&quadDef.rcImage,0,0,DST_WIDTH  ,DST_HEIGHT);

	//��λ����
	matworld_ = XMMatrixIdentity();
	matworld_._11 = quadDef.GetImageWidth()  / (float)viewport1.Width;   //�����Ȼ�ǵ�λ����
	matworld_._22 = quadDef.GetImageHeight() / (float)viewport1.Height;
	//matworld_._41 = -0.5f;
	//matworld_._42 = 0.5f;

	matCombine = matworld_ *g_view* g_proj;		

	//��������
	matTex_ = XMMatrixIdentity();
	GenerateTexMatrix(&quadDef,&matTex);


	m_pMatWorldViewProjVariable->SetMatrix(reinterpret_cast<float*>(&matCombine ));
	m_pMatTexVariable->SetMatrix(reinterpret_cast<float*>(&matTex_));
	m_pTexColorVariable->SetResource(m_pRTtexSRV);//RT��Ⱦ��������ͼ   ��update with per frame��

	m_pTechnique->GetPassByIndex(0)->Apply(0, m_pImmediateContext);
	m_pImmediateContext->Draw(6, 0);


	if(1)
	{
	Dump(L"back_line.bmp");
	Dump_native(L"native_DX11.bmp");
	
	//Map();
	Map_native();
	}

	//�����ʾ
	m_pSwapChain->Present(0, 0);
}

//ץȡͼƬ
void texDemo::Dump(LPCTSTR szPath)
{

	ID3D11DeviceContext *pDeviceContext = NULL;
	m_pd3dDevice->GetImmediateContext(&pDeviceContext);

	ID3D11Texture2D* backBufferTexture;
	m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&backBufferTexture );

	D3DX11_IMAGE_FILE_FORMAT eFmt = D3DX11_IFF_BMP;
	HRESULT hr = D3DX11SaveTextureToFile(
		pDeviceContext,
		/*(ID3D11Resource *)m_pTex*/backBufferTexture,
		eFmt,
		szPath
		);

	SAFE_RELEASE(backBufferTexture);
	pDeviceContext->Release();

}




void texDemo::Dump_RTT(LPCTSTR szPath)
{
	
	ID3D11DeviceContext *pDeviceContext = NULL;
	m_pd3dDevice->GetImmediateContext(&pDeviceContext);

	//ID3D11Texture2D* backBufferTexture;
	//swapChain_->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&backBufferTexture );

	D3DX11_IMAGE_FILE_FORMAT eFmt = D3DX11_IFF_BMP;
	HRESULT hr = D3DX11SaveTextureToFile(
		pDeviceContext,
		(ID3D11Resource *)m_pMidTex,
		eFmt,
		szPath
		);

	//SAFE_RELEASE(backBufferTexture);
	pDeviceContext->Release();

}

void texDemo::Dump_native(LPCTSTR szPath)
{

	ID3D11DeviceContext *pDeviceContext = NULL;
	m_pd3dDevice->GetImmediateContext(&pDeviceContext);

	//ID3D11Texture2D* backBufferTexture;
	//swapChain_->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&backBufferTexture );

	D3DX11_IMAGE_FILE_FORMAT eFmt = D3DX11_IFF_BMP;
	HRESULT hr = D3DX11SaveTextureToFile(
		pDeviceContext,
		(ID3D11Resource *)m_pTex,
		eFmt,
		szPath
		);

	//SAFE_RELEASE(backBufferTexture);
	pDeviceContext->Release();
}


//��������
bool texDemo::Map( )
{
	//�õ������� ��֮�����

	ID3D11DeviceContext *pDeviceContext = NULL;
	m_pd3dDevice->GetImmediateContext(&pDeviceContext);

	// get info
	D3D11_RESOURCE_DIMENSION eDim;
	m_pTex->GetType(&eDim);
	assert(eDim == D3D11_RESOURCE_DIMENSION_TEXTURE2D);

	ID3D11Texture2D* backBufferTexture;
	m_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&backBufferTexture );
	D3D11_TEXTURE2D_DESC descOldTex;
	backBufferTexture->GetDesc(&descOldTex);


	ID3D11Resource	*m_pTex_formap = NULL;	// only for map
	// create
	if (!m_pTex_formap)
	{
		assert(descOldTex.ArraySize == 1);
		if (descOldTex.MipLevels != 1)	// miplevels must be 1
			return false;
		descOldTex.BindFlags = NULL;	// bind to nothing
		descOldTex.Usage = D3D11_USAGE_STAGING;
		descOldTex.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		m_pd3dDevice->CreateTexture2D(
			&descOldTex,
			NULL,
			(ID3D11Texture2D**)&m_pTex_formap
			) ;
	}
	D3D11_BOX box = {0, 0, 0, 
		descOldTex.Width, descOldTex.Height, 1};
	pDeviceContext->CopySubresourceRegion(m_pTex_formap, 0, 
		0, 0, 0,
		/*m_pTex*/backBufferTexture, 0, &box); 
	pDeviceContext->Flush();
	// map
	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	pDeviceContext->Map(m_pTex_formap, 0, D3D11_MAP_READ, 0, &mapped_subresource) ;


	DWORD* imageData1 = (DWORD*) mapped_subresource.pData;

	const char* filename ="D:\\Projects_new\\DX9\\DX11_native\\data\\render_back.txt";
	FILE* ofile = fopen(filename , "w");

	for (int i=0; i<8; i++)
	{
		for (int j=0; j<8; j++)   
			//fprintf(ofile, "(%d, %d) ", (imageData1[i*1920+j])&0xFF,(imageData1[i*1920+j]>>8)&0xFF);//   rg ͨ����xy
			fprintf(ofile, "(%d,%d,%d,%d) ", (imageData1[i*1920+j])&0xFF,(imageData1[i*1920+j]>>8)&0xFF,
			(imageData1[i*1920+j]>>16)&0xFF,(imageData1[i*1920+j]>>24)&0xFF);//rgba    ԭʼ����BUFFER ��ABGR
		fprintf(ofile, "\n");
	}

	fclose(ofile);

	pDeviceContext->Unmap(m_pTex_formap, 0);


	//pDeviceContext->Release();

	SAFE_RELEASE(pDeviceContext);
	SAFE_RELEASE(m_pTex_formap);
	SAFE_RELEASE(backBufferTexture);

	return true;

}

bool texDemo::Map_RTT( )
{
	//�õ������� ��֮�����

	ID3D11DeviceContext *pDeviceContext = NULL;
	m_pd3dDevice->GetImmediateContext(&pDeviceContext);


	// get info
	D3D11_RESOURCE_DIMENSION eDim;
	m_pMidTex->GetType(&eDim);
	assert(eDim == D3D11_RESOURCE_DIMENSION_TEXTURE2D);


	ID3D11Texture2D* pTex2D = reinterpret_cast<ID3D11Texture2D*>(m_pMidTex);
	D3D11_TEXTURE2D_DESC descOldTex;
	pTex2D->GetDesc(&descOldTex);

	ID3D11Resource	*m_pTex_formap = NULL;	// only for map
	// create
	if (!m_pTex_formap)
	{
		assert(descOldTex.ArraySize == 1);
		if (descOldTex.MipLevels != 1)	// miplevels must be 1
			return false;
		descOldTex.BindFlags = NULL;	// bind to nothing
		descOldTex.Usage = D3D11_USAGE_STAGING;
		descOldTex.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		m_pd3dDevice->CreateTexture2D(
			&descOldTex,
			NULL,
			(ID3D11Texture2D**)&m_pTex_formap
			) ;
	}
	D3D11_BOX box = {0, 0, 0, 
		descOldTex.Width, descOldTex.Height, 1};
	pDeviceContext->CopySubresourceRegion(m_pTex_formap, 0, 
		0, 0, 0,
		m_pMidTex, 0, &box); 
	pDeviceContext->Flush();
	// map
	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	pDeviceContext->Map(m_pTex_formap, 0, D3D11_MAP_READ, 0, &mapped_subresource) ;

	//DWORD* imageData1 = (DWORD*) mapped_subresource.pData;
	float* imageData1 = (float*) mapped_subresource.pData;


	const char* filename ="D:/Bjiao/DX11_RT.txt";
	FILE* ofile = fopen( filename, "w");

	for (int i=0; i<20; i++)
	{
		for (int j=0; j<20; j++)   
			// fprintf(ofile, "(%2x) ", imageData1[i*1920+j]);//ԭʼBuffer ���� ABGR
			//fprintf(ofile, "(%d,%d,%d,%d) ", (imageData1[i*DST_WIDTH+j])&0xFF,(imageData1[i*DST_WIDTH+j]>>8)&0xFF,
			//(imageData1[i*DST_WIDTH+j]>>16)&0xFF,(imageData1[i*DST_WIDTH+j]>>24)&0xFF); // abgr -> rgba 
			//fprintf(ofile, "(%.9lf,%.9lf,%.9lf) ", imageData1[i*DST_WIDTH*4+j*4], imageData1[i*DST_WIDTH*4+j*4+1], imageData1[i*DST_WIDTH*4+j*4+2]/*, imageData1[i*DST_WIDTH*4+j*4+3]*/); //R32G32B32A32 
			fprintf(ofile, "(%.9lf,%.9lf) ", imageData1[i*DST_WIDTH*4+j*4]*256.f, imageData1[i*DST_WIDTH*4+j*4+1]*256.f/*, imageData1[i*DST_WIDTH*4+j*4+2], imageData1[i*DST_WIDTH*4+j*4+3]*/); //R32G32B32A32 
		fprintf(ofile, "\n");
	}

	fclose(ofile);

	pDeviceContext->Unmap(m_pTex_formap, 0);


	//pDeviceContext->Release();


	SAFE_RELEASE(pDeviceContext);
	SAFE_RELEASE(m_pTex_formap);

	return true;

}

bool texDemo::Map_native( )
{
	//�õ������� ��֮�����

	ID3D11DeviceContext *pDeviceContext = NULL;
	m_pd3dDevice->GetImmediateContext(&pDeviceContext);


	// get info
	D3D11_RESOURCE_DIMENSION eDim;
	m_pTex->GetType(&eDim);
	assert(eDim == D3D11_RESOURCE_DIMENSION_TEXTURE2D);


	ID3D11Texture2D* pTex2D = reinterpret_cast<ID3D11Texture2D*>(m_pTex);
	D3D11_TEXTURE2D_DESC descOldTex;
	pTex2D->GetDesc(&descOldTex);

	ID3D11Resource	*m_pTex_formap = NULL;	// only for map
	// create
	if (!m_pTex_formap)
	{
		assert(descOldTex.ArraySize == 1);
		if (descOldTex.MipLevels != 1)	// miplevels must be 1
			return false;
		descOldTex.BindFlags = NULL;	// bind to nothing
		descOldTex.Usage = D3D11_USAGE_STAGING;
		descOldTex.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		m_pd3dDevice->CreateTexture2D(
			&descOldTex,
			NULL,
			(ID3D11Texture2D**)&m_pTex_formap
			) ;
	}
	D3D11_BOX box = {0, 0, 0, 
		descOldTex.Width, descOldTex.Height, 1};
	pDeviceContext->CopySubresourceRegion(m_pTex_formap, 0, 
		0, 0, 0,
		m_pTex, 0, &box); 
	pDeviceContext->Flush();
	// map
	D3D11_MAPPED_SUBRESOURCE mapped_subresource;
	pDeviceContext->Map(m_pTex_formap, 0, D3D11_MAP_READ, 0, &mapped_subresource) ;

	DWORD* imageData1 = (DWORD*) mapped_subresource.pData;


	const char* filename ="D:/Bjiao/DX11native.txt";
	FILE* ofile = fopen( filename, "w");

	for (int i=0; i<8; i++)
	{
		for (int j=0; j<8; j++)   
		{
			//fprintf(ofile, "(%2x) ", imageData1[i*1920+j]);//Buffer ���� ABGR	jpg ��ʱ ���  �Լ�д�� ����һ��
			fprintf(ofile, "(%d,%d,%d,%d) ", (imageData1[i*descOldTex.Width+j])&0xFF,(imageData1[i*descOldTex.Width+j]>>8)&0xFF,
				(imageData1[i*descOldTex.Width+j]>>16)&0xFF,(imageData1[i*descOldTex.Width+j]>>24)&0xFF); //  abgr ->  rgba   
			//fprintf(ofile, "(%d,%d,%d,%d) ", (imageData1[i*descOldTex.Width+j]>>16)&0xFF,(imageData1[i*descOldTex.Width+j]>>8)&0xFF,
			//	(imageData1[i*descOldTex.Width+j])&0xFF,(imageData1[i*descOldTex.Width+j]>>24)&0xFF); // argb - > rgba   
		}
		fprintf(ofile, "\n");
	}

	fclose(ofile);

	pDeviceContext->Unmap(m_pTex_formap, 0);


	SAFE_RELEASE(pDeviceContext);
	SAFE_RELEASE(m_pTex_formap);

	return true;

}

//Դ������������
bool texDemo::Up_src()
{

	//�ļ���SRC����
	FILE* bf,*gf,*rf;
	bf=fopen("D:/SRC/b.txt","r");
	gf=fopen("D:/SRC/g.txt","r");
	rf=fopen("D:/SRC/r.txt","r");

	//������
	ID3D11DeviceContext *pDeviceContext = NULL;
	m_pd3dDevice->GetImmediateContext(&pDeviceContext);

	/*ID3D11Texture2D* pTex2D = reinterpret_cast<ID3D11Texture2D*>(m_pTex);
	D3D11_TEXTURE2D_DESC desc;
	pTex2D->GetDesc(&desc);*/

	ID3D11Texture2D * pTex2D = NULL;
	m_pTexSRV->GetResource((ID3D11Resource**)&pTex2D); //Get the resource that is accessed through this view.

	D3D11_TEXTURE2D_DESC desc;
	pTex2D->GetDesc(&desc);

	D3D11_MAPPED_SUBRESOURCE map;
	m_pImmediateContext->Map(pTex2D,D3D11CalcSubresource(0,0,1),D3D11_MAP_WRITE_DISCARD,0,&map);


	int* b = new int(0);
	int* g = new int(0);
	int* r = new int(0);

	for(UINT y = 0; y < desc.Height; y ++)
	{
		DWORD *pCr = (DWORD*)((BYTE*)map.pData + map.RowPitch  * y);
		for(UINT x = 0; x < desc.Width ;x ++)
		{
			//switch (x % 3)   //��д��ɫ
			//{
			//case 0:
			//	//pCr[x] = 0x80ef80ff;
			//	pCr[x] = 0x80000ff;  //ABGR
			//	break;
			//case 1:
			//	pCr[x] = 0x70df70af;
			//	break;
			//case 2:
			//	pCr[x] = 0x601f6000;
			//	break;
			//}

			fscanf(bf,"%d",b);
			fscanf(gf,"%d",g);
			fscanf(rf,"%d",r);
			
			pCr[x] = (0xff<<24)+(b[0]<<16)+(g[0]<<8)+r[0];   //ABGR

			}			
		}

	m_pImmediateContext->Unmap(pTex2D,D3D11CalcSubresource(0,0,1));


	SAFE_DELETE(r);
	SAFE_DELETE(g); 
	SAFE_DELETE(b);  

	fclose(bf);
	fclose(gf);
	fclose(rf);

	SAFE_RELEASE(pDeviceContext);

	if(1)
	{
		D3DX11SaveTextureToFile(m_pImmediateContext,m_pTex,D3DX11_IFF_BMP,L"up_src.bmp");
	}

	return true;
}