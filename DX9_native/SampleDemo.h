#pragma once
#include"DX9DemoBase.h"


#define RT_WIDTH 256
#define RT_HEIGHT 256

#define DST_WIDTH  128
#define DST_HEIGHT 128

#define D3DX_PI_   (3.14159265358979323846)

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

	HRESULT LoadContent();
	void UnLoadContent();

	void Update(float dt);
	void Render();
	void SetupMatrices( );

	void Dump_RT();
	void Dump();
	void Dump_native();


private:
	IDirect3DVertexBuffer9 *			m_pVB  ; // 顶点缓冲区接口

	IDirect3DTexture9 *                m_pColorTextures ;//纹理接口
	IDirect3DTexture9 *					m_pUptexture;    //上行纹理
	IDirect3DTexture9 *					m_pRTtexture;	//RTT 纹理

	IDirect3DSurface9*					m_pRtsurface;//RT纹理的页面 当前渲染页面
	IDirect3DSurface9*					m_pBackbuffer;//  原渲染页面


	ID3DXEffect *							m_pEffect  ; // 效果接口
	IDirect3DVertexDeclaration9 * m_pVBDecl ;//顶点格式声明

	D3DXMATRIXA16 m_world;
	D3DXMATRIXA16 m_view;
	D3DXMATRIXA16 m_proj;

};