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
	void shutdown();//释放内存

	virtual HRESULT LoadContent();//载入具体的Demo自己的内容，如纹理，几何体，着色器
	virtual void UnLoadContent();//释放具体Demo中的内容

	virtual void Update(float dt) = 0;
	virtual void Render() = 0;

protected:

	HINSTANCE m_hInstance;  
	HWND m_hWnd;

	IDirect3D9 *							m_pD3D    ; // D3D 接口
	IDirect3DDevice9 *					m_pd3dDevice  ; // 设备接口


/*
	// 模型
	LPDIRECT3DVERTEXBUFFER9       m_pVB;                // 顶点缓冲区
	LPDIRECT3DINDEXBUFFER9        m_pIB;                // 顶点缓冲区指针
	DWORD                         m_dwNumIndices;       // 顶点的索引个数
	DWORD                         m_dwNumVertices;      // 顶点个数
	LPDIRECT3DVERTEXDECLARATION9  m_pDecl;              // 顶点声明接口

	// 纹理
	//IDirect3DTexture9*            m_pColorTexture  ;
	// 效果
	LPD3DXEFFECT                  m_pEffect;            // 效果接口
*/

};





#endif