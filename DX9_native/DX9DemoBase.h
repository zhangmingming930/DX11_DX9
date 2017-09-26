#ifndef _DX9DEMOBASE_H_
#define _DX9DEMOBASE_H_
#include <d3dx9.h>
#include<d3dx9.h>
#include<DxErr.h>


#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

class DX9DemoBase
{
public:
	DX9DemoBase();
	virtual ~DX9DemoBase();

	HRESULT intialize(HINSTANCE hInstance, HWND hwnd );
	void shutdown();//�ͷ��ڴ�

	virtual HRESULT LoadContent();//��������Demo�Լ������ݣ������������壬��ɫ��
	virtual void UnLoadContent();//�ͷž���Demo�е�����

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;

protected:

	HINSTANCE m_hInstance;  
	HWND m_hWnd;

	IDirect3D9 *							m_pD3D    ; // D3D �ӿ�
	IDirect3DDevice9 *					m_pd3dDevice  ; // �豸�ӿ�


/*
	// ģ��
	LPDIRECT3DVERTEXBUFFER9       m_pVB;                // ���㻺����
	LPDIRECT3DINDEXBUFFER9        m_pIB;                // ���㻺����ָ��
	DWORD                         m_dwNumIndices;       // �������������
	DWORD                         m_dwNumVertices;      // �������
	LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // ���������ӿ�

	// ����
	//IDirect3DTexture9*            m_pColorTexture  ;
	// Ч��
	LPD3DXEFFECT                  m_pEffect;            // Ч���ӿ�
*/

};





#endif