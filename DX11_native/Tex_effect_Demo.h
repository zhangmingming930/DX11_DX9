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

//������ԴBuffer������Ϣ
typedef struct tag_TexResBufinfo
{
	bool								bRenderToFullTarget;				// �Ƿ�ȫ����Ⱦ
	int									BaseWidth;							// buffer���
	int									BaseHeight;							// buffer�߶�
	double								OffsetX;							// buffer���Ͻ�xλ��
	double								OffsetY;							// buffer���Ͻ�yλ��
	POINT								ptBase;								// ��׼������

	RECT								rcImage;							// ����λ��

	// ���캯��
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

	bool LoadContent() override; // ��ʾ��������д�����麯��f
	void UnLoadContent() override;

	void Update(float dt) override;
	void Render() override;
	
	//����������Դ
	bool CreateResouce();
	bool RT(ID3D11Resource**,ID3D11ShaderResourceView**,ID3D11RenderTargetView**,UINT,UINT);
	bool CreateSrc(ID3D11Resource**,ID3D11ShaderResourceView**,UINT w =1920 ,UINT h=1080);
	
	//ץȡͼ��
	void Dump_RTT(LPCTSTR szPath);
	void Dump_native(LPCTSTR) ;
	void Dump(LPCTSTR);

	//��ȡͼ��
	bool Map( );
	bool Map_RTT( );
	bool Map_native( );
	//����Դ����
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




	//effect ��ָ��ȫ�ֱ����Ľӿ�
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
	ID3D11Resource*						m_pTex;//ԭʼ����
	ID3D11ShaderResourceView*      m_pTexSRV;//������ͼ
	//��RTV �Ǻ󻺳��RTV


	//P4 P8                                     coef
	ID3D11Resource*						m_pCoefTex;
	ID3D11ShaderResourceView*      m_pCoefTexSRV;//������ͼ






	//�м�RT                                  dst
	ID3D11Resource*						m_pMidTex;//�м�����
	ID3D11RenderTargetView*			m_pMidRTV;
	//ID3D11DepthStencilView*			m_pMidDSV; //�м����ģ����ͼ
	ID3D11ShaderResourceView*		m_pRTtexSRV;		//RT������ͼ	//OutputTexture

	//												temp
	ID3D11Resource*						m_ptempTex;//�м�����
	ID3D11RenderTargetView*			m_ptempRTV;
	//ID3D11DepthStencilView*			m_pMidDSV; //�м����ģ����ͼ
	ID3D11ShaderResourceView*		m_ptempSRV;		//RT������ͼ	//OutputTexture



	/*XMMATRIX m_world;   //��Ϊ���Ա ����m_world = XMMatrixIdentity(); ��ʼ��������
	XMMATRIX m_view;
	XMMATRIX m_proj;*/

	XMFLOAT4X4 m_world;
	XMFLOAT4X4 m_view;
	XMFLOAT4X4 m_proj;
};