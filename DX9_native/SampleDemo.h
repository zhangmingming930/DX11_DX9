#pragma once
#include"DX9DemoBase.h"


#define RT_WIDTH 256
#define RT_HEIGHT 256

#define DST_WIDTH  128
#define DST_HEIGHT 128

#define D3DX_PI_   (3.14159265358979323846)

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
	IDirect3DVertexBuffer9 *			m_pVB  ; // ���㻺�����ӿ�

	IDirect3DTexture9 *                m_pColorTextures ;//����ӿ�
	IDirect3DTexture9 *					m_pUptexture;    //��������
	IDirect3DTexture9 *					m_pRTtexture;	//RTT ����

	IDirect3DSurface9*					m_pRtsurface;//RT�����ҳ�� ��ǰ��Ⱦҳ��
	IDirect3DSurface9*					m_pBackbuffer;//  ԭ��Ⱦҳ��


	ID3DXEffect *							m_pEffect  ; // Ч���ӿ�
	IDirect3DVertexDeclaration9 * m_pVBDecl ;//�����ʽ����

	D3DXMATRIXA16 m_world;
	D3DXMATRIXA16 m_view;
	D3DXMATRIXA16 m_proj;

};