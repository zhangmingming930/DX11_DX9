#include"SampleDemo.h"
#include<stdio.h>


struct CUSTOMVERTEX
{
	D3DXVECTOR3 position;	// The position
	D3DXVECTOR2 tex;		// The texture coordinates
};

SampleDemo::SampleDemo()
{
	m_pVB  = NULL; // 顶点缓冲区接口
	m_pColorTextures = NULL;
	m_pUptexture = NULL;
	m_pRTtexture = NULL;
	m_pRtsurface = NULL;
	m_pBackbuffer = NULL;
	m_pEffect = NULL;
	m_pVBDecl = NULL;//顶点格式声明
	D3DXMatrixIdentity( &m_world );
	D3DXMatrixIdentity( &m_view );
	D3DXMatrixIdentity( &m_proj);

}

SampleDemo::~SampleDemo()
{

}

HRESULT
SampleDemo:: LoadContent()
{
	HRESULT hr;

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
			return E_FAIL ;
		}


	CUSTOMVERTEX vertices[] =
	{
		{  D3DXVECTOR3(-0.5f ,  -0.5f , 0.0f ), D3DXVECTOR2(0.0,1.0)  },// x, y, z, w,u,v 
		{  D3DXVECTOR3(-0.5f  , 0.5f , 0.0f ) ,  D3DXVECTOR2(0.0,0.0) }, // 绿色
		{ D3DXVECTOR3(0.5f ,  -0.5f , 0.0f ) , D3DXVECTOR2(1.0,1.0)  },// 蓝色
		{ D3DXVECTOR3(0.5f ,  0.5f , 0.0f ), D3DXVECTOR2(1.0,0.0 ) } // 蓝色
	};


	if( FAILED( hr = m_pd3dDevice->CreateVertexBuffer( 4 * sizeof( CUSTOMVERTEX ),
		0 ,0 , D3DPOOL_DEFAULT , &m_pVB, NULL ))) 
		{
			MessageBox(NULL, L"Create Vertex Buffer failed!", L"Error", 0) ;
			return E_FAIL ;
		}


	VOID* pVertices;
	if( FAILED( hr = m_pVB->Lock( 0, sizeof( vertices ), ( void** )&pVertices, 0 )))    return hr;
	memcpy( pVertices, vertices, sizeof( vertices ));
	m_pVB->Unlock( );


	///////////////////////////////////////
	//
	//创建一些资源（纹理）
	//
	///////////////////////

	if(FAILED(hr = m_pd3dDevice->CreateTexture(DST_WIDTH,
																DST_HEIGHT,
																1,
																D3DUSAGE_RENDERTARGET,
																/*D3DFMT_A8R8G8B8,*/
																D3DFMT_A32B32G32R32F,  //为了打印RT float型纹理坐标
																D3DPOOL_DEFAULT,
																&m_pRTtexture,
																NULL)))
	{
		MessageBox(NULL, L"Create Texture failed!", L"Error", 0) ;
		return E_FAIL ;
	}

	//得到 （0层 纹理）页面  及 页面描述 
	D3DSURFACE_DESC Desc;
	hr = m_pRTtexture->GetSurfaceLevel(0,&m_pRtsurface);
	m_pRtsurface->GetDesc(&Desc);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Get surface on texture failed!", L"Error", 0) ;
		return E_FAIL ;
	}
	
if(0)
{

	if(FAILED(hr = m_pd3dDevice->CreateTexture(RT_WIDTH,
																	RT_HEIGHT,
																		1,
													D3DUSAGE_DYNAMIC,
														D3DFMT_A8R8G8B8,
														D3DPOOL_DEFAULT,
															&m_pUptexture,
																		NULL)))
	{
		MessageBox(NULL, L"Create up Texture failed!", L"Error", 0) ;
		return E_FAIL ;
	}

	//得到 （0层 纹理）  描述 
	D3DLOCKED_RECT lockedRect;
	D3DSURFACE_DESC Desc_up;
	m_pUptexture->GetLevelDesc(0,&Desc_up);

	m_pUptexture->LockRect(0, &lockedRect,0, 0);         
	//DWORD* pCr = (DWORD*)(BYTE*)lockedRect.pBits;
	//DWORD T = 0Xff000000;
	for(int i = 0; i <Desc_up.Height; i++)
	{
		DWORD* pCr = (DWORD*)((BYTE*)lockedRect.pBits+lockedRect.Pitch*i);
		for(int j = 0; j < Desc_up.Width; j++)
		{

			switch (j % 3)   //自写颜色
			{
			case 0:
				//pCr[x] = 0x80ef80ff;
				pCr[j] = 0x8ff0000;  //ARGB
				break;
			case 1:
				pCr[j] = 0x70af70df;
				break;
			case 2:
				pCr[j] = 0x6000601f;
				break;
			//pCr[i * lockedRect.Pitch / 4+ j] = T++; 

			}
		}
	}

	m_pUptexture->UnlockRect(0);

	DWORD* imageData1 = (DWORD*)lockedRect.pBits;

	// SAVE BITS TO TEXT FILE
	FILE* ofile = fopen("output_up.txt", "w");

	for (int i=0; i<8/*Desc_up.Height*/; i++)
	{
		for (int j=0; j<8/*Desc_up.Width*/; j++)   
			//fprintf(ofile, "(%2x) ", imageData1[i*Desc_up.Width+j]);  //原始Buffer数据 ARGB
			fprintf(ofile, "(%d,%d,%d,%d) ", (imageData1[i*Desc_up.Width+j]>>16)&0xFF, (imageData1[i*Desc_up.Width+j]>>8)&0xFF,(imageData1[i*Desc_up.Width+j]>>0)&0xFF,(imageData1[i*Desc_up.Width+j]>>24)&0xFF);//rgba
		fprintf(ofile, "\n");
	}

	fclose(ofile);

	if(1)
	{
		D3DXSaveTextureToFile(L"upTEX_1.bmp",D3DXIFF_BMP,m_pUptexture,NULL);
	}

}



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
	if( FAILED( hr = D3DXCreateEffectFromFile( m_pd3dDevice, L"FileFX3.fx", /////////////////////////////////////////////////////////////////////////////////////
		NULL, NULL, 0, NULL, &m_pEffect, &pCode )))
	{
		LPVOID pBufErr = pCode->GetBufferPointer();
		char* ErrInfo = ( char* ) pBufErr;
		OutputDebugStringA(ErrInfo); //OK
		pCode->Release( );

		MessageBox(NULL, L"Create Effect From File failed!", L"Error", 0) ;
		return E_FAIL ;
	}


	//得到纹理信息  D3DFMT_X8R8G8B8 
	D3DXIMAGE_INFO imageInfo;
	D3DXGetImageInfoFromFile(L"3.jpg",&imageInfo);


	// 装入纹理，注意只装入一个纹理
	if( FAILED( hr = D3DXCreateTextureFromFileEx( m_pd3dDevice, L"3.jpg" , 
		 (UINT)RT_WIDTH,  (UINT)RT_HEIGHT, 
		D3DX_DEFAULT , 0 , D3DFMT_A8R8G8B8 , D3DPOOL_MANAGED , 
		D3DX_DEFAULT , D3DX_DEFAULT , 0 , NULL, NULL, &m_pColorTextures )))		// 更改D3DFMT_UNKNOWN  

	{
		MessageBox(NULL, L"Create Texture From File failed!", L"Error", 0) ;
		return E_FAIL ;
	}


	Dump_native();


	D3DXHANDLE handletec = m_pEffect->GetTechniqueByName( "DefaultTech");
	if( FAILED( hr = m_pEffect->SetTechnique(handletec ))) return hr ;


	return S_OK;
}

void 
SampleDemo::UnLoadContent()
{
	SAFE_RELEASE( m_pVB );
	SAFE_RELEASE(m_pColorTextures );
	SAFE_RELEASE(m_pUptexture);
	SAFE_RELEASE(m_pRTtexture );
	SAFE_RELEASE(m_pRtsurface );
	SAFE_RELEASE(m_pBackbuffer);
	SAFE_RELEASE( m_pEffect );
	SAFE_RELEASE( m_pVBDecl );
}

void 
SampleDemo::Update(float dt)
{
//更新数据
}

void 
SampleDemo::SetupMatrices( )
{
	// 观察矩阵
	D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-0.50f/tan(D3DX_PI_/8.0f) );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMatrixLookAtLH( &m_view, &vEyePt, &vLookatPt, &vUpVec );
	// 投影矩阵
	D3DXMatrixPerspectiveFovLH( &m_proj , D3DX_PI_/4, 1.0f, 0.50f, 100.0f );
}


void 
SampleDemo::Render()
{
	SetupMatrices( );
	m_pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof( CUSTOMVERTEX ));
	m_pd3dDevice->SetVertexDeclaration(m_pVBDecl);

	D3DXMATRIXA16 matCombine,matTex,matWorld,matPosOffset;


	//////////////////////////////////
	//RTT
	/////////////////////////////////

	m_pd3dDevice->GetRenderTarget(0, &m_pBackbuffer) ;   //保存原来渲染页面
	m_pd3dDevice->SetRenderTarget(0,m_pRtsurface);         //设置当前渲染页面 (纹理表面)
	m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_ARGB(255, 0, 0, 0 ), 1.0f, 0 );

	TexResBufinfo SrcDef(RT_WIDTH,RT_HEIGHT,0.5f,0.5f);
	SetRect(&SrcDef.rcImage,0,0,RT_WIDTH,RT_HEIGHT);

	TexResBufinfo DstDef(DST_WIDTH,DST_HEIGHT,0,0);
	SetRect(&DstDef.rcImage,0,0,DST_WIDTH,DST_HEIGHT);

	TexResBufinfo*	pSrcDef = &SrcDef;
	TexResBufinfo*  pDstDef = &DstDef;

	RECT  rcBorder;
	//SetRect(& rcBorder,0,0,RT_WIDTH,RT_HEIGHT);
	SetRect(& rcBorder,0,0,DST_WIDTH,DST_HEIGHT);


	//VP
	D3DVIEWPORT9 vPort1;
	m_pd3dDevice->GetViewport(&vPort1);
	//memset(&vPort1, 0, sizeof(D3DVIEWPORT9));
	vPort1.MaxZ = 1;
	//vPort1.Width = DST_WIDTH;
	//vPort1.Height = DST_HEIGHT;
	vPort1.X = rcBorder.left;		
	vPort1.Width = rcBorder.right - rcBorder.left;
	vPort1.Y = rcBorder.top;
	vPort1.Height = rcBorder.bottom - rcBorder.top;
	m_pd3dDevice->SetViewport(&vPort1);

	//世界矩阵
	D3DXMatrixIdentity(&matWorld);

	 double dbSacleX,dbSacleY;
	 dbSacleX = DST_WIDTH/(double)RT_WIDTH;
	 dbSacleY = DST_HEIGHT/(double)RT_HEIGHT;

	//matWorld._11 = pSrcDef->GetImageWidth() / (float)vPort1.Width;
	//matWorld._22 = pSrcDef->GetImageHeight() / (float)vPort1.Height;
	//matWorld._11 = pDstDef->GetImageWidth() / (float)vPort1.Width;
	//matWorld._22 = pDstDef->GetImageHeight() / (float)vPort1.Height;
	matWorld._11 = 1.0f;/*pDstDef->GetImageWidth() / (float)pSrcDef->GetImageWidth();*/
	matWorld._22 =1.0f;/*pDstDef->GetImageHeight() /(float)pSrcDef->GetImageHeight() ;*/
	matWorld._41 = pSrcDef->OffsetX - 0.5f;
	matWorld._42 = 0.5f - pSrcDef->OffsetY;

	matCombine = matWorld*m_view * m_proj;
	m_pEffect->SetMatrix("g_matWorldViewProj",&matCombine);

	


	//纹理矩阵
	D3DXMatrixIdentity( &matTex);

	matTex._11 = pSrcDef->GetImageWidth()/ (float)pSrcDef->BaseWidth;
	matTex._22 = pSrcDef->GetImageHeight()/ (float)pSrcDef->BaseHeight;
	//matTex._31 = /*pSrcDef->rcImage.left*//*0.25f*/0.5f/*0.5f*(DST_WIDTH/(float)RT_WIDTH)*/ / ((float)pSrcDef->BaseWidth);     //////////////////////////////// matTex._31 = 0.5f / pYUVA->GetImagesInfo()->Base_Width;     /*/*/ Tex += 0.5f * g_txSize.zw;/*/*/
	//matTex._32 =/* pSrcDef->rcImage.top*//*0.25f*/0.5f/*0.5f*(DST_HEIGHT/(float)RT_HEIGHT)*// ((float)pSrcDef->BaseHeight);		///////////////////////////////	matTex._32 = 0.5f / pYUVA->GetImagesInfo()->Base_Height;
	m_pEffect->SetMatrix("g_matTex",&matTex);

	//顶点偏移矩阵  0.5f
	D3DXMatrixIdentity( &matPosOffset);	
	matPosOffset._41 = -1.0f/pDstDef->BaseWidth;
	matPosOffset._42 = 1.0f/pDstDef->BaseHeight;
	m_pEffect->SetMatrix("g_matPosoffset",&matPosOffset);

	

	//纹理采样坐标偏移
	D3DXVECTOR4 vTexSize(pDstDef->BaseWidth,pDstDef->BaseHeight,0.0f,0.0f);
	vTexSize.z = 1.0f / vTexSize.x;
	vTexSize.w = 1.0f / vTexSize.y;
	m_pEffect->SetVector("g_txSize",&vTexSize); 

	//m_pEffect->SetTexture( "g_ColorTexture",m_pUptexture)  ;//加载上行的的纹理
	m_pEffect->SetTexture( "g_ColorTexture",m_pColorTextures)  ;//加载载入JPG的纹理

	if( SUCCEEDED( m_pd3dDevice->BeginScene( )))
	{
		UINT iPass, cPasses;
		m_pEffect->Begin( &cPasses, 0 );
		iPass = 0;

		m_pEffect->BeginPass(iPass);
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);
		m_pEffect->EndPass();

		m_pEffect->End();
		m_pd3dDevice->EndScene();
	}


	if(1)
		{	Dump_RT();	
			D3DXSaveTextureToFile(L"RTT_tex.bmp",D3DXIFF_BMP,m_pRTtexture,NULL);

	}
		m_pd3dDevice->SetRenderTarget(0, m_pBackbuffer) ;//恢复原来的渲染页面
		m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_ARGB(255, 0, 0, 0 ), 1.0f, 0 );

	//////////////////////////////////////////////////////
	//显示 RT  
	/////////////////////////////////////////////////////

		D3DVIEWPORT9 vPort;
		m_pd3dDevice->GetViewport(&vPort);
		vPort.Width = DST_WIDTH;
		vPort.Height = DST_HEIGHT;
		m_pd3dDevice->SetViewport(&vPort);

		//纹理变换矩阵
	
		D3DXMATRIXA16 matworld_,worldViewProj,matTex_;
		TexResBufinfo quadDef(DST_WIDTH,DST_HEIGHT,0,0);
		SetRect(&quadDef.rcImage,0,0,DST_WIDTH,DST_HEIGHT);

		//定位矩阵
		D3DXMatrixIdentity(&matworld_);
		matworld_._11 = quadDef.GetImageWidth()  / (float)vPort.Width;   //这个竟然是单位矩阵
		matworld_._22 = quadDef.GetImageHeight() / (float)vPort.Height;
		worldViewProj = matworld_ * m_view* m_proj;		

		//纹理矩阵
		D3DXMatrixIdentity(&matTex_);
		RECT rcResult = quadDef.rcImage;
		matTex_._11 = (rcResult.right - rcResult.left) / (float)quadDef.BaseWidth;
		matTex_._22 = (rcResult.bottom - rcResult.top) / (float)quadDef.BaseHeight;
		matTex_._31 = rcResult.left/ (float)quadDef.BaseWidth;
		matTex_._32 =rcResult.top/ (float)quadDef.BaseHeight;

		m_pEffect->SetMatrix( "g_matTex" , &matTex_);
		m_pEffect->SetMatrix("g_matWorldViewProj",&worldViewProj);

		m_pEffect->SetTexture("g_ColorTexture",m_pRTtexture);//使用渲染的纹理（RT）

	if( SUCCEEDED( m_pd3dDevice->BeginScene( )))
	{
		UINT iPass, cPasses;
		m_pEffect->Begin( &cPasses, 0 );
		iPass = 0;
		m_pEffect->BeginPass(iPass);
		m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
		m_pEffect->EndPass();

		m_pEffect->End();

		if(1)
		{	
			Dump();
		}
		m_pd3dDevice->EndScene();
	}

	 m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}
//
void  SampleDemo::Dump_RT()
{

	//D3DXSaveTextureToFile(L"RTT_tex.bmp",D3DXIFF_BMP,m_pRTtexture,NULL);

	D3DXSaveSurfaceToFile(L"RTT_surf.bmp", D3DXIFF_BMP,m_pRtsurface, NULL, NULL);

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
		/*D3DFMT_A8R8G8B8,*/
		D3DFMT_A32B32G32R32F,
		D3DPOOL_SYSTEMMEM, 
		&text, 
		NULL) ;
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Create texture failed!", L"Error", 0) ;
		//return E_FAIL ;
	}


	text->GetSurfaceLevel(0,&surf);

	if (m_pd3dDevice->GetRenderTargetData(m_pRtsurface, surf) == D3D_OK)             // 从m_pRtsurface 中读取数据 
		if (surf->LockRect(&lockbits, NULL, D3DLOCK_READONLY) == D3D_OK)
		{

			surf->UnlockRect();

			float* imageData1 = (float*)lockbits.pBits;//原始Buffer数据 A32B32G32R32F
			//DWORD* imageData1 = (DWORD*)lockbits.pBits;//原始Buffer数据 ARGB

			// SAVE BITS TO TEXT FILE
			FILE* ofile = fopen("D:/Bjiao/DX9_output_RT__.txt", "w");

			for (int i=0; i<20; i++)
			{
				for (int j=0; j<20; j++)   
					//fprintf(ofile, "(%d,%d) ", (imageData1[i*1920+j]>>16)&0xFF, (imageData1[i*1920+j]>>8)&0xFF);// R G通道对应X Y
					//fprintf(ofile, "(%d,%d,%d,%d) ", (imageData1[i*DST_WIDTH +j]>>16)&0xFF, (imageData1[i*DST_WIDTH +j]>>8)&0xFF,(imageData1[i*DST_WIDTH +j]>>0)&0xFF,(imageData1[i*DST_WIDTH +j]>>24)&0xFF);//rgba
					//fprintf(ofile, "(%.9lf,%.9lf,%.9lf) ", imageData1[i*DST_WIDTH*4+j*4], imageData1[i*DST_WIDTH*4+j*4+1], imageData1[i*DST_WIDTH*4+j*4+2]/*, imageData1[i*DST_WIDTH*4+j*4+3]*/); //R32G32B32A32
					fprintf(ofile, "(%.9lf,%.9lf) ", imageData1[i*DST_WIDTH*4+j*4]*256.f, imageData1[i*DST_WIDTH*4+j*4+1]*256.f/*,imageData1[i*DST_WIDTH*4+j*4+2], imageData1[i*DST_WIDTH*4+j*4+3]*/); //R32G32B32A32
				fprintf(ofile, "\n");											//width =  DST_WIDTH   size of one element in bytes = 16 
			} 

			fclose(ofile);
		}

		text->Release();

		surf->Release(); 

		if(0)
		{
			LPDIRECT3DSURFACE9 pOldSurf = NULL;
			m_pd3dDevice->GetRenderTarget(0, &pOldSurf);
			D3DXSaveSurfaceToFile(L"1.bmp", D3DXIFF_BMP,pOldSurf, NULL, NULL);
			SAFE_RELEASE(pOldSurf);
		}

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
	FILE* ofile = fopen("D:/Bjiao/DX9_output_native.txt", "w");

	for (int i=0; i<8; i++)
	{
		for (int j=0; j<8; j++)   
			//fprintf(ofile, "(%d,%d) ", (imageData1[i*1920+j]>>16)&0xFF, (imageData1[i*1920+j]>>8)&0xFF);// R G通道对应X Y
			fprintf(ofile, "(%d,%d,%d,%d) ", (imageData1[i*Desc_src.Width+j]>>16)&0xFF, (imageData1[i*Desc_src.Width+j]>>8)&0xFF,(imageData1[i*Desc_src.Width+j]>>0)&0xFF,(imageData1[i*Desc_src.Width+j]>>24)&0xFF);//rgba
			//fprintf(ofile, "(%2x) ", imageData1[i*1920+j]);  //原始Buffer数据 ARGB
		fprintf(ofile, "\n");
	} 

	fclose(ofile);

if(0)  //读取图像像素值rgb
{
	FILE* bf,*gf,*rf;
	bf=fopen("D:/SRC/b.txt","w");
	gf=fopen("D:/SRC/g.txt","w");
	rf=fopen("D:/SRC/r.txt","w");

	for (int i=0; i<Desc_src.Height; i++)
	{
		for (int j=0; j<Desc_src.Width; j++)  
		{
			fprintf(rf,"%d ",(imageData1[i*Desc_src.Width+j]>>16)&0xFF);
			fprintf(gf,"%d ",(imageData1[i*Desc_src.Width+j]>>8)&0xFF);
			fprintf(bf,"%d ",(imageData1[i*Desc_src.Width+j]>>0)&0xFF);
		}
	} 
	fclose(bf);
	fclose(gf);
	fclose(rf);
}

	m_pColorTextures->UnlockRect(0);

	D3DXSaveTextureToFile(L"native_DX9.bmp", D3DXIFF_BMP, m_pColorTextures, NULL);
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
}