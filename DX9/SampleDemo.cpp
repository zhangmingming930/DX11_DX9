#include"SampleDemo.h"
#include<stdio.h>


struct CUSTOMVERTEX
{
	D3DXVECTOR3 position;	// The position
	D3DXVECTOR2 tex;		// The texture coordinates
};
//
SampleDemo::SampleDemo()
{
	m_pVB  = NULL; // 顶点缓冲区接口
	m_pColorTextures = NULL;
	m_pCoefTex = NULL;
	m_pMidTex = NULL;
	m_pMidsurface = NULL;
	m_pBackbuffer = NULL;
	m_pEffect = NULL;
	m_pVBDecl = NULL;//顶点格式声明
	D3DXMatrixIdentity( &m_world );
	D3DXMatrixIdentity( &m_view );
	D3DXMatrixIdentity( &m_proj);
	cout = 0;

}

SampleDemo::~SampleDemo()
{
}
//创建纹理资源
bool SampleDemo::CreateTexture(IDirect3DTexture9 ** pTexture,IDirect3DSurface9**	pSurf,UINT width,UINT height)
{
	HRESULT hr;

	if(FAILED(hr = m_pd3dDevice->CreateTexture(width,
		height,
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,                 //////??????也许这里有问题
		D3DPOOL_DEFAULT,
		pTexture,
		NULL)))
	{
		MessageBox(NULL, L"Create Texture failed!", L"Error", 0) ;
		return false ;
	}
	//得到 （0层 纹理）页面  及 描述 
	D3DSURFACE_DESC Desc;
	hr = (*pTexture)->GetSurfaceLevel(0,pSurf);
	(*pSurf)->GetDesc(&Desc);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Get surface on texture failed!", L"Error", 0) ;
		return false ;
	}


}
//
bool SampleDemo::CreateCoefRes()
{	
		if(FAILED(m_pd3dDevice->CreateTexture(
			16,
			2,
			1,
			D3DUSAGE_DYNAMIC,
			D3DFMT_A16B16G16R16F,
			D3DPOOL_DEFAULT,
			&m_pCoefTex,
			NULL)))
		{
			MessageBox(NULL, L"Create up Texture failed!", L"Error", 0) ;
			return false ;
		}

		//得到 （0层 纹理）  描述 
		D3DSURFACE_DESC Desc_up;
		m_pCoefTex->GetLevelDesc(0,&Desc_up);

		/*D3DLOCKED_RECT lockedRect;
		m_pCoefTex->LockRect(0, &lockedRect,0, 0);         
		m_pCoefTex->UnlockRect(0);*/

		//强制纹理分配内存(空),避免Upload出错
		if(Desc_up.Usage == D3DUSAGE_DYNAMIC)
		{
			LPDIRECT3DTEXTURE9 pTexture =m_pCoefTex;
			D3DLOCKED_RECT lr;
			pTexture->LockRect(0,&lr,NULL,0);
			pTexture->UnlockRect(0);
		}

		if(1)
		{
			D3DXSaveTextureToFile(L"upTEX_1.bmp",D3DXIFF_BMP,m_pCoefTex,NULL);
		}

}
//
bool
SampleDemo:: LoadContent()
{
	HRESULT hr;
	
	//创建coef纹理
	if(FAILED(CreateCoefRes()))
	{
		MessageBox(0, L"创建Coef资源失败", 0, 0);
		return false ;
	}
	//创建目标纹理
	if(FAILED(CreateTexture(&m_pMidTex,&m_pMidsurface,1280,720)))
	{
		MessageBox(0, L"创建纹理资源失败", 0, 0);
		return false ;
	}
	
	//设置顶点数据及声明
	D3DVERTEXELEMENT9 VertexElemShader[] = 
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,  0 },		
		{ 0, 12,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,  0 },		
		D3DDECL_END()
	};
	if(FAILED(hr = m_pd3dDevice->CreateVertexDeclaration( VertexElemShader,&m_pVBDecl)))
	{
		MessageBox(NULL, L"Create Vertex Declaration failed!", L"Error", 0) ;
		return false ;
	}

	CUSTOMVERTEX vertices[] =
	{
		{  D3DXVECTOR3(-0.5f ,  -0.5f , 0.0f ) , D3DXVECTOR2(0.0,1.0)  },// x, y, z, w,u,v 
		{  D3DXVECTOR3(-0.5f  , 0.5f , 0.0f ) ,  D3DXVECTOR2(0.0,0.0) }, // 绿色
		{ D3DXVECTOR3(0.5f ,  -0.5f , 0.0f ) , D3DXVECTOR2(1.0,1.0)  },// 蓝色
		{ D3DXVECTOR3(0.5f ,  0.5f , 0.0f ) , D3DXVECTOR2(1.0,0.0 ) } // 蓝色
	};

	//顶点信息
	if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( CUSTOMVERTEX ),
		0 ,0 , D3DPOOL_DEFAULT , &m_pVB, NULL ))) 
	{
		MessageBox(NULL, L"Create Vertex Buffer failed!", L"Error", 0) ;
		return false ;
	}
	VOID* pVertices;
	if( FAILED( hr = m_pVB->Lock( 0, sizeof( vertices ), ( void** )&pVertices, 0 )))    return false;
	memcpy( pVertices, vertices, sizeof( vertices ));
	m_pVB->Unlock( );


	// 设置目录
	TCHAR szAppPath[ MAX_PATH ];
	TCHAR szRcPath[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, szAppPath );
	// 
	lstrcpy( szRcPath , szAppPath );
	lstrcat( szRcPath ,L"\\ResFile_1_Model" );
	SetCurrentDirectory( szRcPath );

	// 装入效果文件
	LPD3DXBUFFER pCode = NULL;
	if( FAILED( hr = D3DXCreateEffectFromFile( m_pd3dDevice, L"../FX/EN_SonyScaleFilter.fx", /////////////////////////////////////////////////////////////////////////////////////
		NULL, NULL, 0, NULL, &m_pEffect, &pCode )))
	{
		LPVOID pBufErr = pCode->GetBufferPointer();
		char* ErrInfo = ( char* ) pBufErr;
		OutputDebugStringA(ErrInfo); //OK
		pCode->Release( );

		MessageBox(NULL, L"Create Effect From File failed!", L"Error", 0) ;
		return false ;
	}


	//得到纹理信息  D3DFMT_X8R8G8B8 
	D3DXIMAGE_INFO imageInfo;
	D3DXGetImageInfoFromFile(L"1.jpg",&imageInfo);
	// 装入纹理，注意只装入一个纹理
	if( FAILED( hr = D3DXCreateTextureFromFileEx( m_pd3dDevice, L"1.jpg" , 
		1920, 1080, 
		D3DX_DEFAULT , 0 , D3DFMT_A8R8G8B8 , D3DPOOL_MANAGED , 
		D3DX_DEFAULT , D3DX_DEFAULT , 0 , NULL, NULL, &m_pColorTextures )))		// 更改D3DFMT_UNKNOWN  
	{
		MessageBox(NULL, L"Create Texture From File failed!", L"Error", 0) ;
		return false ;
	}



	//effect中全局变量与C++关联
#ifdef		P8
	if( FAILED( hr = m_pEffect->SetTechnique("P8"))) 
	{
		MessageBox(NULL, L"SetTexchnique(P8)  failed!", L"Error", 0);
		return false ;
	}
#else
	if( FAILED( hr = m_pEffect->SetTechnique("P4"))) return false ;
#endif


	return true;
}

//
void 
SampleDemo::UnLoadContent()
{
	SAFE_RELEASE( m_pVB );

	SAFE_RELEASE(m_pColorTextures );
	SAFE_RELEASE(m_pCoefTex);
	SAFE_RELEASE(m_pMidTex );

	SAFE_RELEASE(m_pBackbuffer);
	SAFE_RELEASE(m_pMidsurface);

	SAFE_RELEASE( m_pEffect );
	SAFE_RELEASE( m_pVBDecl );
}

//
void 
SampleDemo::Update(float dt)
{
	//更新数据
	/*D3DXMATRIXA16 m_matView,m_matPrj;
	D3DXMatrixIdentity( &m_matView  );
	D3DXMatrixIdentity( &m_matPrj);

	D3DXVECTOR3 vEyePt1( 0.0f, 0.0f,-0.50f/tan(D3DX_PI/8.0f) );
	D3DXVECTOR3 vLookatPt1( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec1( 0.0f, 1.0f, 0.0f );

	D3DXMatrixLookAtLH( &m_matView, &vEyePt1, &vLookatPt1, &vUpVec1 );
	D3DXMatrixPerspectiveFovLH( &m_matPrj, D3DX_PI/4, 1.0f, 0.50f, 100.0f );*/

	// 观察矩阵
	D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-0.50f/tan(D3DX_PI/8.0f) );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &m_view, &vEyePt, &vLookatPt, &vUpVec );
	// 投影矩阵
	D3DXMatrixPerspectiveFovLH( &m_proj , D3DX_PI/4, 1.0f, 0.50f, 100.0f );

	 //世界矩阵
	/*m_pEffect->SetMatrix( "g_matWorld" , &m_world );
	m_pEffect->SetMatrix( "g_matView" , &m_view );
	m_pEffect->SetMatrix( "g_matProject" , &m_proj );*/
}

//
void 
SampleDemo::Render()
{
	//顶点信息
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( CUSTOMVERTEX ));
	m_pd3dDevice->SetVertexDeclaration(m_pVBDecl);

	//////////////////////////////////
	//
	//RTT(经过 sonyFilter 处理)
	//
	/////////////////////////////////
	m_pd3dDevice->GetRenderTarget(0, &m_pBackbuffer) ;   //保存原来渲染页面

	TexResBufinfo SrcDef(1920,1080,0.5f,0.5f);
	SetRect(&SrcDef.rcImage,0,0,1920,1080);
	TexResBufinfo DstDef(1280,720,0,0);
	SetRect(&DstDef.rcImage,0,0,1280,720);

	TexResBufinfo*	pSrcDef = &SrcDef;
	TexResBufinfo*  pDstDef = &DstDef;

	RECT rcBound;
	SetRect(&rcBound,0,0,1280,720);

	//POS_STRETCH_TO_FILL
	double dbSacleX, dbSacleY;
	dbSacleX = (double)pDstDef->GetImageWidth()/pSrcDef->GetImageWidth();
	dbSacleY = (double)pDstDef->GetImageHeight()/pSrcDef->GetImageHeight();

#ifdef P8
	SonyFilter( pSrcDef, pDstDef, dbSacleX, dbSacleY,8,& rcBound);
#else
	SonyFilter( pSrcDef, pDstDef, dbSacleX, dbSacleY,4,& rcBound);
#endif

	if(1)
	{	
		Dump_RT();
		Dump_native();
	}
	//////////////////////////////////////////////////////
	//
	//显示 RT  
	//
	/////////////////////////////////////////////////////
	m_pd3dDevice->SetRenderTarget(0, m_pBackbuffer) ;//恢复原来的渲染页面
	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_XRGB( 0, 0, 255 ), 1.0f, 0 );

	//VP
	D3DVIEWPORT9 vPort;
	m_pd3dDevice->GetViewport(&vPort);
	vPort.Width = static_cast<float>(1280);
	vPort.Height = static_cast<float>(720);
	vPort.MinZ = 0.0f;
	vPort.MaxZ = 1.0f;
	vPort.X = 0.0f;
	vPort.Y = 0.0f;
	m_pd3dDevice->SetViewport(&vPort);

	D3DXMATRIXA16 matworld,worldViewProj,matTex;
	TexResBufinfo quadDef(1280,720,0,0);
	SetRect(&quadDef.rcImage,0,0,1280,720);

	//定位矩阵
	D3DXMatrixIdentity(&matworld);
	matworld._11 = quadDef.GetImageWidth()  / (float)vPort.Width;   //这个竟然是单位矩阵
	matworld._22 = quadDef.GetImageHeight() / (float)vPort.Height;
	worldViewProj = matworld * m_view* m_proj;		

	//纹理矩阵
	D3DXMatrixIdentity(&matTex);
	RECT rcResult = quadDef.rcImage;
	matTex._11 = (rcResult.right - rcResult.left) / (float)quadDef.BaseWidth;
	matTex._22 = (rcResult.bottom - rcResult.top) / (float)quadDef.BaseHeight;
	matTex._31 = rcResult.left / (float)quadDef.BaseWidth;
	matTex._32 = rcResult.top / (float)quadDef.BaseHeight;

	m_pEffect->SetMatrix( "g_matWVP" , &worldViewProj );
	m_pEffect->SetMatrix( "g_matTex" , &matTex);
	m_pEffect->SetTexture("g_txColor",m_pMidTex);//使用渲染的纹理（RT）

	if( SUCCEEDED( m_pd3dDevice->BeginScene( )))
	{
		
		UINT iPass, cPasses;
		m_pEffect->Begin( &cPasses, 0 );
		iPass = 3;
		m_pEffect->BeginPass(iPass);
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		m_pEffect->EndPass();
		m_pEffect->End();

		if(0)
		{	
			Dump();
		}
		m_pd3dDevice->EndScene();
	}

	m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}
//
//P4_P8采样
//
bool SampleDemo::SonyFilter(TexResBufinfo *pSrcDef, TexResBufinfo *pDstDef,double dbSacleX,double dbSacleY,int nTaps,RECT * pRcBound)
{

	RECT rcBorder;
		memcpy(&rcBorder,pRcBound,sizeof(RECT));

	LPD3DXEFFECT pEffect = m_pEffect;
	pEffect->SetTexture("g_txCoef",m_pCoefTex);

	RECT rcResult = pSrcDef->rcImage;

	BOOL bRenderX = int((dbSacleX - 1.0) * 1e4) != 0;
	BOOL bRenderY = int((dbSacleY - 1.0) * 1e4) != 0;

	DWORD crClear = 0xFF101010;
	D3DXMATRIXA16 matWorld,matCombine,matTex;	
	//X
	IDirect3DTexture9 *	 pYUVATexture = NULL;

	TexResBufinfo* pTempDef = pSrcDef;

	if(bRenderX)
	{
		LPDIRECT3DSURFACE9 pSurf = NULL;
		if(bRenderY)		
			CreateTexture(&pYUVATexture,&pSurf,pTempDef ->BaseWidth,pTempDef ->BaseHeight);
		else
		{
			//pYUVATexture = (CBaseTexture*)pDstDef->pContainer;
			 pTempDef = pDstDef;
			CreateTexture(&pYUVATexture,&pSurf,pTempDef ->BaseWidth,pTempDef ->BaseHeight);
		}

		//LPDIRECT3DSURFACE9 pSurf = NULL;
		//pYUVATexture->GetSurfaceLevel(0,&pSurf);

		//
		//设置渲染目标 temp纹理  pYUVATexture
		//
		m_pd3dDevice->SetRenderTarget(0,pSurf);
		SAFE_RELEASE(pSurf);
		m_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET,crClear,0,1);
		D3DVIEWPORT9 vPort;
		m_pd3dDevice->GetViewport(&vPort);
		vPort.X = rcBorder.left;		
		vPort.Width = rcBorder.right - rcBorder.left;
		if(!bRenderY)
		{
			vPort.Y = rcBorder.top;
			vPort.Height = rcBorder.bottom - rcBorder.top;
		}
		m_pd3dDevice->SetViewport(&vPort);


		CalcSonyFilterCoef(dbSacleX,nTaps,0);	

		//世界矩阵
		D3DXMatrixIdentity(&matWorld);

		matWorld._11 = pSrcDef->GetImageWidth() * dbSacleX / (float)vPort.Width;
		matWorld._22 = pSrcDef->GetImageHeight() / (float)vPort.Height;
	
		matWorld._41 = pSrcDef->OffsetX - 0.5f;
		if(!bRenderY)
		matWorld._42 = 0.5f - pSrcDef->OffsetY;

		matCombine = matWorld * m_view * m_proj;

		pEffect->SetTechnique(nTaps == 4 ? "P4" : "P8");
		pEffect->SetMatrix("g_matWVP",&matCombine);

		//纹理矩阵
		D3DXMatrixIdentity(&matTex);	

		matTex._11 = pSrcDef->GetImageWidth() / (float)pSrcDef->BaseWidth;
		matTex._22 = pSrcDef->GetImageHeight() / (float)pSrcDef->BaseHeight;
		matTex._31 = pSrcDef->rcImage.left / (float)pSrcDef->BaseWidth;
		matTex._32 = pSrcDef->rcImage.top / (float)pSrcDef->BaseHeight;

		matTex._41 = (0.5f + pSrcDef->rcImage.left) / pSrcDef->BaseWidth;
		matTex._42 = (0.5f + pSrcDef->rcImage.top) / pSrcDef->BaseHeight;
		matTex._43 = (pSrcDef->rcImage.right - 0.5f) / pSrcDef->BaseWidth;
		matTex._44 = (pSrcDef->rcImage.bottom - 0.5f) / pSrcDef->BaseHeight;

		pEffect->SetMatrix("g_matTex",&matTex);

		D3DXVECTOR4 vTexSize(pSrcDef->BaseWidth,pSrcDef->BaseHeight,0.0f,0.0f);
		vTexSize.z = 1.0f / vTexSize.x;
		vTexSize.w = 1.0f / vTexSize.y;
		pEffect->SetVector("g_txSize",&vTexSize); 

		pEffect->SetTexture("g_txColor",m_pColorTextures);

		if(SUCCEEDED(m_pd3dDevice->BeginScene()))
		{
			UINT cPass;
			pEffect->Begin(&cPass,0);
			pEffect->BeginPass(0);  //  PIX_FMT_YUVA
			m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
			pEffect->EndPass();
			pEffect->End();
			m_pd3dDevice->EndScene();
		}

		CalcBox(&matWorld,&m_view,&m_proj,&rcResult);
	}
	D3DXSaveTextureToFile(L"../P4_P8/dst_mid.bmp",D3DXIFF_BMP,pYUVATexture ,NULL);
	D3DXSaveTextureToFile(L"../P4_P8/src.bmp",D3DXIFF_BMP,m_pColorTextures,NULL);	
	//Y	
	if(bRenderY)
	{
		//
		//设置中间渲染目标 m_pMidTex   dst
		//
		LPDIRECT3DSURFACE9 pSurf = NULL;
		m_pMidTex->GetSurfaceLevel(0,&pSurf);
		m_pd3dDevice->SetRenderTarget(0,pSurf);
		SAFE_RELEASE(pSurf);

		m_pd3dDevice->Clear(0,NULL,D3DCLEAR_TARGET,crClear,0,1);
		D3DVIEWPORT9 vPort;
		m_pd3dDevice->GetViewport(&vPort);
		vPort.X = rcBorder.left;		
		vPort.Width = rcBorder.right - rcBorder.left;		
		vPort.Y = rcBorder.top;
		vPort.Height = rcBorder.bottom - rcBorder.top;

		m_pd3dDevice->SetViewport(&vPort);
		
		//计算系数纹理
		if(nTaps == 4)
			CalcSonyFilterCoef(dbSacleY,nTaps,0);	
		else
		{
			if(dbSacleY > 1.0f / 1.5f)
				CalcSonyFilterCoef(1.0,nTaps,0);
			else
				CalcSonyFilterCoef(dbSacleY * 1.5,nTaps,0);
		}
	
		//世界矩阵
		D3DXMatrixIdentity(&matWorld);	

		float fOffset = (0.25f / dbSacleY - 0.25f);
			fOffset = 0.0f;  //源 bFirstField =1 节目 m_pProfile->bInterval = 0

		matWorld._11 = (rcResult.right - rcResult.left) / (float)vPort.Width;
		matWorld._22 = (rcResult.bottom - rcResult.top) * dbSacleY / (float)vPort.Height;	
		
		if(bRenderX)
			matWorld._41 = (matWorld._11 - 1.0f) / 2.0f + rcResult.left / (float)vPort.Width;	
		else
			matWorld._41 = pSrcDef->OffsetX - 0.5f;			

		matWorld._42 = 0.5f - pSrcDef->OffsetY;

		matCombine = matWorld * m_view * m_proj;

		pEffect->SetFloat("g_fOffset",fOffset);

		pEffect->SetMatrix("g_matWVP",&matCombine);
		
		//纹理矩阵
		D3DXMatrixIdentity(&matTex);
		matTex._11 = (rcResult.right - rcResult.left) / (float)pTempDef->BaseWidth;
		matTex._22 = (rcResult.bottom - rcResult.top) / (float)pTempDef->BaseHeight;
		matTex._31 = rcResult.left / (float)pTempDef->BaseWidth;
		matTex._32 = rcResult.top / (float)pTempDef->BaseHeight;

		matTex._41 = (0.5f + rcResult.left) / pTempDef->BaseWidth;
		matTex._42 = (0.5f + rcResult.top) / pTempDef->BaseHeight;
		matTex._43 = (rcResult.right - 0.5f) / pTempDef->BaseWidth;
		matTex._44 = (rcResult.bottom - 0.5f) / pTempDef->BaseHeight;

		pEffect->SetTechnique(nTaps == 4 ? "P4" : "P8");
		pEffect->SetMatrix("g_matTex",&matTex);
		pEffect->SetTexture("g_txColor",pYUVATexture);//X轴处理的纹理 进入shader

		D3DXVECTOR4 vTexSize = D3DXVECTOR4(pTempDef->BaseWidth,pTempDef->BaseHeight,0.0f,0.0f);
		vTexSize.z = 1.0f / vTexSize.x;
		vTexSize.w = 1.0f / vTexSize.y;
		pEffect->SetVector("g_txSize",&vTexSize);

		if(SUCCEEDED(m_pd3dDevice->BeginScene()))
		{
			UINT cPass;
			pEffect->Begin(&cPass,0);
			pEffect->BeginPass(2);
			m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
			pEffect->EndPass();
			pEffect->End();
			m_pd3dDevice->EndScene();
		}
		CalcBox(&matWorld,&m_view,&m_proj,&rcResult);
	}
	pDstDef->rcImage = rcResult;
	pDstDef->OffsetX = pDstDef->rcImage.left;
	pDstDef->OffsetY = pDstDef->rcImage.top;

	D3DXSaveTextureToFile(L"../P4_P8/last_dst.bmp",D3DXIFF_BMP,m_pMidTex ,NULL);	
	return true;
}
void SampleDemo::CalcBox(const D3DXMATRIXA16 * matWorld,const D3DXMATRIXA16 * matView,const D3DXMATRIXA16 * matProj,RECT * rcImage)
{
	D3DXVECTOR3 vMesh[4]=
	{
		D3DXVECTOR3(-0.5f,-0.5f,0.0f),
		D3DXVECTOR3(-0.5f,0.5f,0.0f),
		D3DXVECTOR3(0.5f,0.5f,0.0f),
		D3DXVECTOR3(0.5f,-0.5f,0.0f),
	};
	D3DVIEWPORT9 vPort;
	m_pd3dDevice->GetViewport(&vPort);
	D3DXVec3ProjectArray(vMesh,sizeof(D3DXVECTOR3),vMesh,sizeof(D3DXVECTOR3),&vPort,matProj,matView,matWorld,4);
	D3DXVECTOR3 vMin,vMax;
	D3DXComputeBoundingBox(vMesh,4,sizeof(D3DXVECTOR3),&vMin,&vMax);
	//RECT rcImage;
	SetRect(rcImage,CEIL(vMin.x),CEIL(vMin.y),CEIL(vMax.x),CEIL(vMax.y));
	Clamp(rcImage->left,0,vPort.Width);
	Clamp(rcImage->top,0,vPort.Height);
	Clamp(rcImage->right,0,vPort.Width);
	Clamp(rcImage->bottom,0,vPort.Height);
}
void SampleDemo::CalcSonyFilterCoef(double dbScale,int nTaps,int nPattern/*0 other 1 PageRoll*/)
{
	float		coef[16][8];
	double		kaiser[8];	

	if(nPattern == 0)
	{
		int i,j,k;
		double d,w,o;
		d = 3.14159 * dbScale;
		if(d > 3.14159) d = 3.14159;

		if(nTaps== 4)
		{
			kaiser[0] = 0.60997 * 2048.0;
			kaiser[1] = 0.95074 * 2048.0;
			kaiser[2] = 0.95074 * 2048.0;
			kaiser[3] = 0.60997 * 2048.0;
			//
			for(j = 0;j < 16;j++)
			{
				o = double(j) / 16.0;
				for(i = 0;i < 4;i++){
					w = (double(i - 1) - o) * d;
					if(w == 0.0) coef[j][i] = 2048;
					else coef[j][i] = int(sin(w) / w * kaiser[i]);
				}
				k = 0;
				for(i = 0;i < 4;i++){
					k += coef[j][i];
				}
				for(i = 0;i < 4;i++){
					coef[j][i] = coef[j][i] * 2048 / k;	// normalize
				}
				coef[j][1] = 2048 - coef[j][0] - coef[j][2] - coef[j][3];
			}
			for(int j = 0;j < 16 ;j ++)
			{
				for(int i = 0; i < 4; i ++)
				{
					coef[j][i] /= 2048.0f;
				}
			}
		}
		else if(nTaps== 8)
		{

			kaiser[0] = 0.251731 * 2048.0;
			kaiser[1] = 0.534026 * 2048.0;
			kaiser[2] = 0.808136 * 2048.0;
			kaiser[3] = 0.977194 * 2048.0;
			kaiser[4] = 0.977194 * 2048.0;
			kaiser[5] = 0.808136 * 2048.0;
			kaiser[6] = 0.534026 * 2048.0;
			kaiser[7] = 0.251731 * 2048.0;

			//
			for(j = 0;j < 16;j++)
			{
				o = double(j) / 16.0;
				for(i = 0;i < 8;i++)
				{
					w = (double(i - 3) - o) * d;
					if(w == 0.0) coef[j][i] = 2048;
					else coef[j][i] = int(sin(w) / w * kaiser[i]);
				}
				k = 0;
				for(i = 0;i < 8;i++)
				{
					k += coef[j][i];
				}
				for(i = 0;i < 8;i++)
				{
					coef[j][i] = coef[j][i] * 2048 / k;	// normalize
				}
				coef[j][3] = 2048 - coef[j][0] - coef[j][1] - coef[j][2] - coef[j][4] - coef[j][5] - coef[j][6] - coef[j][7];				
			}
		}	

	}
	else //Pattern == 1
	{
		if(nTaps == 4)
		{
			float fixcoef[16][8] = 
			{  
				{0.267043f,		0.465914f,	0.267043f,	0},
				{0.244556f,		0.460979f,	0.284701f,	0.00976424f},
				{0.222628f,		0.454824f,	0.302024f,	0.020524f},
				{0.201314f,		0.447476f,	0.318928f,	0.0322827f},
				{0.180667f,		0.438968f,	0.335327f,	0.0450376f},
				{0.160741f,		0.429341f,	0.351139f,	0.05878f},
				{0.141583f,		0.418641f,	0.366281f,	0.0734951f},
				{0.123241f,		0.406923f,	0.380675f,	0.0891622f},
				{0.105755f,		0.394245f,	0.394245f,	0.105755f},
				{0.0891622f,	0.380675f,	0.406923f,	0.123241f},
				{0.0734951f,	0.366281f,	0.418641f,	0.141583f},
				{0.05878f,		0.351139f,	0.429341f,	0.160741f},
				{0.0450376f,	0.335327f,	0.438968f,	0.180667f},
				{0.0322827f,	0.318928f,	0.447476f,	0.201314f},
				{0.020524f,		0.302024f,	0.454824f,	0.222628f},
				{0.00976424f,	0.284701f,	0.460979f,	0.244556f},
			};
			memcpy(coef,fixcoef,sizeof(float) * 16 * 8);
		}
		else if(nTaps == 8)
		{
			float fixcoef[16][8] = 
			{		// Cutoff = 1/2  K = 3.5
				-55,0,534,1039,646,0,-117,0,
				-54,-16,504,1021,691,27,-120,-4,
				-52,-32,467,1012,726,55,-120,-8,
				-49,-46,430,1000,761,85,-120,-12,
				-47,-59,393,986,793,116,-118,-16,
				-44,-71,357,969,824,148,-115,-21,
				-40,-82,320,950,853,181,-111,-25,
				-37,-91,285,929,881,215,-105,-29,
				-33,-99,249,906,906,249,-99,-33,
				-29,-105,215,881,929,285,-91,-37,
				-25,-111,181,853,950,320,-82,-40,
				-21,-115,148,824,969,357,-71,-44,
				-16,-118,116,793,986,393,-59,-47,
				-12,-120,85,761,1000,430,-46,-49,
				-8,-120,55,726,1012,467,-32,-52,
				-4,-120,27,691,1021,504,-16,-54
			};			
			memcpy(coef,fixcoef,sizeof(float) * 16 * 8);
		}		
	}
	LPDIRECT3DTEXTURE9 pCoefTex = m_pCoefTex;

	D3DLOCKED_RECT lr;
	pCoefTex->LockRect(0,&lr,NULL,0);


	//FILE* ofile_ = fopen("coef_true.txt", "w");

	if(nTaps== 4)
	{
		D3DXVECTOR4_16F * pData = (D3DXVECTOR4_16F*)((byte*)lr.pBits);
		for(int i = 0;i < 16;i ++)
		{
			pData[i] = (float*)&coef[i][0];		
		//	fprintf(ofile_, "(%5d) ", pData[i]);  ///
		}
	}
	else if(nTaps == 8)
	{
		for(int j = 0;j < 16;j ++)
		{
			for(int i = 0; i < 8; i ++)
			{
				coef[j][i] /= 2048.0f;
			}
		}

		for(int j = 0;j < 2;j ++)
		{
			D3DXVECTOR4_16F * pData = (D3DXVECTOR4_16F*)((byte*)lr.pBits + lr.Pitch * j);
			for(int i = 0;i < 16;i ++)
			{
				pData[i] = (float*)&coef[i][j * 4];
			//	fprintf(ofile_, "(%d, %d,%d,%d )", pData[i].x,pData[i].y,pData[i].z,pData[i].w);  ///
			}
		//	fprintf(ofile_, "\n");								//

		}		
	}

	//fclose(ofile_);
	pCoefTex->UnlockRect(0);
	if(1)
	{
		D3DXSaveTextureToFile(L"upTEX_2.bmp",D3DXIFF_BMP,m_pCoefTex,NULL);

		char filename[20];
		sprintf(filename,"%s%d%s","coef_up",cout++,".txt");

		WORD * imageData1 =  (WORD*)lr.pBits;

		// SAVE BITS TO TEXT FILE
		FILE* ofile = fopen(filename, "w");

		for (int i=0; i<2; i++)
		{
			for (int j=0; j<16; j++)   
				fprintf(ofile, "(%d,%d,%d,%d) ", imageData1[i*16*4+j*4], imageData1[i*16*4+j*4+1], imageData1[i*16*4+j*4+2],imageData1[i*16*4+j*4+3]);
			fprintf(ofile, "\n");
		}

		fclose(ofile);


	}
}
//
//抓取图像得到数据
void  SampleDemo::Dump_RT()
{

	D3DXSaveTextureToFile(L"RTT_tex.bmp",D3DXIFF_BMP,m_pMidTex,NULL);

	//D3DXSaveSurfaceToFile(L"RTT_surf.bmp", D3DXIFF_BMP,m_pMidsurface, NULL, NULL);

	HRESULT hr ;
	//读数据
	LPDIRECT3DTEXTURE9 text= NULL;
	LPDIRECT3DSURFACE9 surf=NULL;
	D3DLOCKED_RECT lockbits;

	hr = m_pd3dDevice->CreateTexture(
		DST_WIDTH,
		DST_HEIGHT,
		1, 
		0, 
		D3DFMT_A8R8G8B8,
		D3DPOOL_SYSTEMMEM, 
		&text, 
		NULL) ;
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create texture failed!", L"Error", 0) ;
		//return E_FAIL ;
	}


	text->GetSurfaceLevel(0,&surf);

	if (m_pd3dDevice->GetRenderTargetData(m_pMidsurface, surf) == D3D_OK)             // 从m_pMidsurface 中读取数据 
		if (surf->LockRect(&lockbits, NULL, D3DLOCK_READONLY) == D3D_OK)
		{

			surf->UnlockRect();

			DWORD* imageData1 = (DWORD*)lockbits.pBits;//原始Buffer数据 ARGB

			// SAVE BITS TO TEXT FILE
			FILE* ofile = fopen("DX9_output_RT.txt", "w");

			for (int i=0; i<8; i++)
			{
				for (int j=0; j<8; j++)   
					//fprintf(ofile, "(%d,%d) ", (imageData1[i*1920+j]>>16)&0xFF, (imageData1[i*1920+j]>>8)&0xFF);// R G通道对应X Y
					fprintf(ofile, "(%d,%d,%d,%d) ", (imageData1[i*1280+j]>>16)&0xFF, (imageData1[i*1280+j]>>8)&0xFF,(imageData1[i*1280+j]>>0)&0xFF,(imageData1[i*1280+j]>>24)&0xFF);//rgba
				fprintf(ofile, "\n");
			} 

			fclose(ofile);
		}

		if(0)
		{
			LPDIRECT3DSURFACE9 pOldSurf = NULL;
			m_pd3dDevice->GetRenderTarget(0, &pOldSurf);
			D3DXSaveSurfaceToFile(L"1.bmp", D3DXIFF_BMP,pOldSurf, NULL, NULL);
			SAFE_RELEASE(pOldSurf);
		}

		SAFE_RELEASE(text);
		SAFE_RELEASE(surf);
}

void SampleDemo::Dump_native()
{
	//D3DSURFACE_DESC Desc_src;
	//m_pColorTextures->GetLevelDesc(0,&Desc_src);

	//得到 （0层 纹理）  描述 
	D3DLOCKED_RECT lockedRect;
	D3DSURFACE_DESC Desc_src;
	m_pColorTextures->GetLevelDesc(0,&Desc_src);

	m_pColorTextures->LockRect(0, &lockedRect,0, 0); 


	DWORD* imageData1 = (DWORD*)lockedRect.pBits;

	// SAVE BITS TO TEXT FILE
	FILE* ofile = fopen("DX9_output_native.txt", "w");

	for (int i=0; i<8; i++)
	{
		for (int j=0; j<8; j++)   
			//fprintf(ofile, "(%d,%d) ", (imageData1[i*1920+j]>>16)&0xFF, (imageData1[i*1920+j]>>8)&0xFF);// R G通道对应X Y
			fprintf(ofile, "(%d,%d,%d,%d) ", (imageData1[i*1920+j]>>16)&0xFF, (imageData1[i*1920+j]>>8)&0xFF,(imageData1[i*1920+j]>>0)&0xFF,(imageData1[i*1920+j]>>24)&0xFF);//rgba
		//fprintf(ofile, "(%2x) ", imageData1[i*1920+j]);  //原始Buffer数据 ARGB
		fprintf(ofile, "\n");
	} 

	fclose(ofile);

	m_pColorTextures->UnlockRect(0);

	D3DXSaveTextureToFile(L"native_tex.bmp", D3DXIFF_BMP, m_pColorTextures, NULL);
}

void SampleDemo::Dump()
{
	LPDIRECT3DSURFACE9 pOldSurf = NULL;
	m_pd3dDevice->GetRenderTarget(0, &pOldSurf);
	D3DXSaveSurfaceToFile(L"quad_surf.bmp", D3DXIFF_BMP,pOldSurf, NULL, NULL);
	SAFE_RELEASE(pOldSurf);


	HRESULT hr = NULL;
	IDirect3DSurface9 *pSurface;
	hr = m_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pSurface);
	if (FAILED(hr)) return ;

	D3DSURFACE_DESC surface_Desc;
	hr = pSurface->GetDesc(&surface_Desc);
	if (FAILED(hr)) return ;

	IDirect3DTexture9 *pTex = NULL;
	IDirect3DSurface9 *pTexSurface = NULL;
	m_pd3dDevice->CreateTexture(surface_Desc.Width,
		surface_Desc.Height,
		1,
		0,
		surface_Desc.Format,
		D3DPOOL_SYSTEMMEM, //必须为这个
		&pTex, NULL);

	if (pTex)
		hr = pTex->GetSurfaceLevel(0, &pTexSurface);
	if (pTexSurface)
		hr = m_pd3dDevice->GetRenderTargetData(pSurface, pTexSurface);    //从pSurface （backbuffer）中读取数据

	D3DLOCKED_RECT lockedRect;
	pTex->LockRect(0, &lockedRect, NULL, D3DLOCK_READONLY);

	DWORD* imageData1 = (DWORD*)lockedRect.pBits;

	// SAVE BITS TO TEXT FILE
	FILE* ofile = fopen("DX9_output_back.txt", "w");

	for (int i=0; i<8; i++)
	{
		for (int j=0; j<8; j++)   
			//fprintf(ofile, "%2x ", imageData1[i*1920+j]);//原始Buffer数据 ARGB
			//fprintf(ofile, "(%d,%d) ", (imageData1[i*1920+j]>>16)&0xFF, (imageData1[i*1920+j]>>8)&0xFF);//xy
			fprintf(ofile, "(%d,%d,%d,%d) ", (imageData1[i*1920+j]>>16)&0xFF, (imageData1[i*1920+j]>>8)&0xFF,(imageData1[i*1920+j]>>0)&0xFF,(imageData1[i*1920+j]>>24)&0xFF);//rgba
		fprintf(ofile, "\n");
	}

	fclose(ofile);

	pTex->UnlockRect(0);
	D3DXSaveTextureToFile(L"quad_tex.bmp", D3DXIFF_BMP, pTex, NULL);

	SAFE_RELEASE(pTex);
	SAFE_RELEASE(pTexSurface);
	SAFE_RELEASE(pSurface);
}