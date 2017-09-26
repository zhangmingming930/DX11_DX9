#pragma once

#include "Dx11DemoBase.h"
#include "d3dx11effect.h"

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif 
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)       { if (p) { delete [](p);     (p)=NULL; } }
#endif 
//#define D3DX_PI    ((FLOAT)  3.141592654f)


#define RT_WIDTH 256
#define RT_HEIGHT 256

#define DST_WIDTH  128
#define DST_HEIGHT  128

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
//
class texDemo : public Dx11DemoBase
{
public:
	texDemo();
	~texDemo();

	bool LoadContent() override; // 表示派生类重写基类虚函数f
	void UnLoadContent() override;

	void Update(float dt) override;
	void Render() override;
	
	//创造纹理资源
	bool CreateResouce();
	bool RT(ID3D11Resource**,ID3D11ShaderResourceView**,ID3D11RenderTargetView**,UINT,UINT);
	bool CreateSrc(ID3D11Resource**,ID3D11ShaderResourceView**,UINT w =1920 ,UINT h=1080);
	
	//抓取图像
	void Dump_RTT(LPCTSTR szPath);
	void Dump_native(LPCTSTR) ;
	void Dump(LPCTSTR);

	//读取图像
	bool Map( );
	bool Map_RTT( );
	bool Map_native( );
	//加载源数据
	bool Up_src();

	//P4 P8
	//void CalcSonyFilterCoef(ID3D11DeviceContext* pDeviceContext,double dbScale,int nTaps,int nPattern/*0 PageRoll 1 other*/);
	//void SonyFilter(TexResBufinfo* pSrcDef, TexResBufinfo* pDstDef,double dbSacleX,double dbSacleY,int nTaps,RECT * pRcBound);

private:
	ID3D11Buffer *m_pVertexBuffer;
	//ID3D11Buffer *m_pIndexBuffer;
	ID3D11InputLayout *m_pInputLayout;


	//Effect 
	ID3DX11Effect *m_pEffect;




	//effect 中指向全局变量的接口
	ID3DX11EffectTechnique *m_pTechnique;
	//ID3DX11EffectMatrixVariable *m_pFxWorldViewProj;
	//ID3DX11EffectMatrixVariable *m_pFxmatTex;
	//ID3DX11EffectShaderResourceVariable*	m_pFxTex;


	ID3DX11EffectMatrixVariable*         m_pMatWorldViewProjVariable;  //ok
	ID3DX11EffectMatrixVariable*         m_pMatTexVariable;	//ok

	ID3DX11EffectScalarVariable*         m_pVecOffsetVariable;

	ID3DX11EffectScalarVariable*         m_pVecHeightVariable;
	ID3DX11EffectScalarVariable*         m_pVecWidthVariable;

	ID3DX11EffectVectorVariable*         m_pVecSizeVariable;

	ID3DX11EffectShaderResourceVariable* m_pTexColorVariable;  //ok
	ID3DX11EffectShaderResourceVariable* m_pTexCoefVariable;

	ID3DX11EffectTechnique*              m_pTecP4;
	ID3DX11EffectTechnique*              m_pTecP8;	

	//                                            src
	ID3D11Resource*						m_pTex;//原始纹理
	ID3D11ShaderResourceView*      m_pTexSRV;//纹理视图
	//其RTV 是后缓冲的RTV


	//P4 P8                                     coef
	ID3D11Resource*						m_pCoefTex;
	ID3D11ShaderResourceView*      m_pCoefTexSRV;//纹理视图






	//中间RT                                  dst
	ID3D11Resource*						m_pMidTex;//中间纹理
	ID3D11RenderTargetView*			m_pMidRTV;
	//ID3D11DepthStencilView*			m_pMidDSV; //中间深度模板视图
	ID3D11ShaderResourceView*		m_pRTtexSRV;		//RT纹理视图	//OutputTexture

	//												temp
	ID3D11Resource*						m_ptempTex;//中间纹理
	ID3D11RenderTargetView*			m_ptempRTV;
	//ID3D11DepthStencilView*			m_pMidDSV; //中间深度模板视图
	ID3D11ShaderResourceView*		m_ptempSRV;		//RT纹理视图	//OutputTexture



	/*XMMATRIX m_world;   //作为类成员 进行m_world = XMMatrixIdentity(); 初始化有问题
	XMMATRIX m_view;
	XMMATRIX m_proj;*/

	XMFLOAT4X4 m_world;
	XMFLOAT4X4 m_view;
	XMFLOAT4X4 m_proj;
};