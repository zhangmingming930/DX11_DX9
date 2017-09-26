#pragma once
#include"DX9DemoBase.h"


#define RT_WIDTH  
#define RT_HEIGHT 

#define DST_WIDTH  1280
#define DST_HEIGHT  720


#define P8 1

#define Clamp(x, min, max)  x = (x<min  ? min : x<max ? x : max);
#define Step_Pixel (0.5f / 255.0f)
#define CEIL(x) (int(x) + ((float(x) - int(x)) > Step_Pixel ? 1 : 0))

//纹理资源Buffer基本信息
typedef struct tag_TexResBufinfo
{
	bool								bRenderToFullTarget;				// 是否全屏渲染
	int									BaseWidth;							// buffer宽度
	int									BaseHeight;							// buffer高度
	double								OffsetX;							// buffer左上角x位置
	double								OffsetY;							// buffer左上角y位置
	POINT								ptBase;								// 基准点坐标

	RECT								rcImage;							// 处理位置

	// 构造函数
	tag_TexResBufinfo()
	{
		bRenderToFullTarget = false;
		BaseWidth = 0;
		BaseHeight = 0;
		OffsetX = 0;
		OffsetY = 0;
		ptBase.x = 0;
		ptBase.y = 0;
		memset(&rcImage, 0, sizeof(rcImage));
	}

	tag_TexResBufinfo(UINT W,UINT H,double offsetX,double offsetY)
	{
		bRenderToFullTarget = false;
		BaseWidth = W;
		BaseHeight = H;
		OffsetX = offsetX;
		OffsetY = offsetY;
		ptBase.x = 0;
		ptBase.y = 0;
		memset(&rcImage, 0, sizeof(rcImage));
	}


	int GetImageWidth() const
	{
		return rcImage.right - rcImage.left;
	}
	int GetImageHeight() const
	{
		return rcImage.bottom - rcImage.top;
	}

	~tag_TexResBufinfo()
	{
	}


}TexResBufinfo;


class SampleDemo : public DX9DemoBase
{
public:
	SampleDemo();
	virtual ~SampleDemo();

	bool LoadContent();
	void UnLoadContent();

	void Update(float dt);
	void Render();

	bool CreateTexture(IDirect3DTexture9** pTexture,IDirect3DSurface9** pSurf,UINT width,UINT height);

	//	P4 P8
	bool CreateCoefRes();
	bool SonyFilter(TexResBufinfo *pSrcDef, TexResBufinfo *pDstDef,double dbSacleX,double dbSacleY,int nTaps,RECT * pRcBound);
	void CalcBox(const D3DXMATRIXA16 * matWorld,const D3DXMATRIXA16 * matView,const D3DXMATRIXA16 * matProj,RECT * rcImage);
	void CalcSonyFilterCoef(double dbScale,int nTaps,int nPattern/*0 other 1 PageRoll*/);

	void Dump_RT();
	void Dump();
	void Dump_native();


private:
	IDirect3DVertexBuffer9 *			m_pVB  ; // 顶点缓冲区接口

	IDirect3DTexture9 *                m_pColorTextures ;//纹理接口

	IDirect3DTexture9 *					m_pCoefTex;    //系数纹理
	IDirect3DTexture9 *					m_pMidTex;	//RTT 纹理

	IDirect3DSurface9*					m_pMidsurface;//RT纹理的页面 当前渲染页面
	IDirect3DSurface9*					m_pBackbuffer;//  原渲染页面


	ID3DXEffect *							m_pEffect  ; // 效果接口
	IDirect3DVertexDeclaration9 * m_pVBDecl ;//顶点格式声明

	D3DXMATRIXA16 m_world;
	D3DXMATRIXA16 m_view;
	D3DXMATRIXA16 m_proj;

	UINT cout;

};