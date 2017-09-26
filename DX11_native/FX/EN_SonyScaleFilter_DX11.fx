#include ".\ColorSpaceFunc.INC"

cbuffer cbChangeEveryFrame
{
	float4x4 g_matWVP;
	float4x4 g_matTex;

	float  g_fOffset;
	float4 g_txSize;
};

Texture2D g_txColor;
Texture2D g_txCoef;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;    
};
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Clamp;
    AddressV = Clamp;    
};

DepthStencilState DisableDepth
{	
    DepthEnable = False;    
};

BlendState NoAlphaBlend
{	    
};

BlendState AlphaBlending
{	
    BlendEnable[0] = True;
    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = ADD;
    
    SrcBlendAlpha = Src_Alpha;
    DestBlendAlpha = Inv_Src_Alpha;
    BlendOpAlpha = ADD;    
};

RasterizerState DisableCulling
{
    CullMode = NONE;   
};

struct VS_INPUT
{
	float4 vPos:POSITION;				
	float2 vTex:TEXCOORD0;
};
struct PS_INPUT
{	
	float4 oPos:SV_POSITION;
	float4 vTex:TEXCOORD0;
};
PS_INPUT VS(VS_INPUT input)
{	
	PS_INPUT output;
	
	output.oPos = mul(input.vPos,g_matWVP);
	output.vTex.xy = mul(float3(input.vTex,1.0f),(float3x3)g_matTex);		
	output.vTex.zw = input.vTex;	
	
	return output;
}	

float4 PS_P4_H(PS_INPUT input):SV_Target
{
	float4 fColor = (float4)0;	
	
	float2 Tex = input.vTex.xy;
	float fX = Tex.x * g_txSize.x + 1e-3;
	float4 vCoef = g_txCoef.Sample(samPoint,float2(0.1f / 2.0f,frac(fX)  + 0.1f / 16.0f));		
	
	Tex.x = uint(fX) * g_txSize.z;
//	Tex += 0.5f * g_txSize.zw;
	
	for(int i = -1 ;i < 3;i ++)		
		fColor += g_txColor.Sample(samPoint,clamp(Tex + float2(i,0.0f) * g_txSize.zw,float2(g_matTex._41,g_matTex._42),float2(g_matTex._43,g_matTex._44))) * vCoef[i + 1];
		
	return fColor;
}
float4 PS_P4_V(PS_INPUT input):SV_Target
{
	float4 fColor = (float4)0;	
	float2 Tex = input.vTex.xy;
	float fY = Tex.y * g_txSize.y + g_fOffset + 1e-3;
	float4 vCoef = g_txCoef.Sample(samPoint,float2(0.1f / 2.0f,frac(fY) + 0.1f / 16.0f));	
		
	Tex.y = uint(fY) * g_txSize.w;
	
//	Tex += 0.5f * g_txSize.zw;
	
	for(int i = -1 ;i < 3;i ++)		
		fColor += g_txColor.Sample(samPoint,clamp(Tex + float2(0.0f,i) * g_txSize.zw,float2(g_matTex._41,g_matTex._42),float2(g_matTex._43,g_matTex._44))) * vCoef[i + 1];
		
	return fColor;
}
float4 PS_P8_H_422(PS_INPUT input):SV_Target
{	
	float4 fColor = (float4)0;	
	float2 Tex = input.vTex.xy;	
	float fX = Tex.x * g_txSize.x + 0.3f / 255.0f;
		
	float4 vCoef0 = g_txCoef.Sample(samPoint,float2(0.1f / 2.0f,frac(fX) + 0.1f / 16.0f));	
	float4 vCoef1 = g_txCoef.Sample(samPoint,float2(1.1f / 2.0f,frac(fX) + 0.1f / 16.0f));	
	float  vCoefY[8] = {vCoef0,vCoef1};
		
	
 	float fXuv = Tex.x * g_txSize.x / 2.0f + 0.3f / 255.0f;	
 	
 	vCoef0 = g_txCoef.Sample(samPoint,float2(0.1f / 2.0f,frac(fXuv) + 0.1f / 16.0f));	
 	vCoef1 = g_txCoef.Sample(samPoint,float2(1.1f / 2.0f,frac(fXuv) + 0.1f / 16.0f));	
 	float  vCoefUV[8] = {vCoef0,vCoef1};		

	Tex.x = int(fX) * g_txSize.z;
	Tex.xy += 0.5f * g_txSize.zw;
		
	int nPixel[12] = {-6,-4,-3,-2,-1,0,1,2,3,4,6,8};	
	int nUVPixel[8] = {0,1,3,5,7,9,10,11};	
	
	int nPixel1[12] = {-7,-5,-3,-2,-1,0,1,2,3,4,5,7};
	int nUVPixel1[8] = {0,1,2,4,6,8,10,11};
	
	if(frac(fXuv) >= 0.5f)
	{
		for(int i = 0; i < 8;i ++)
		   nUVPixel[i] = nUVPixel1[i];	
		
		for(int i = 0; i < 12;i ++)		
			nPixel[i] = nPixel1[i];	
	}
	
	int nYPixel[8] = {2,3,4,5,6,7,8,9};
	float4 vPixel[12];
	for(int i = 0;i < 12;i ++)
	{
		vPixel[i] = g_txColor.Sample(samPoint,clamp(Tex + float2(nPixel[i],0.0f) * g_txSize.zw,float2(g_matTex._41,g_matTex._42),float2(g_matTex._43,g_matTex._44)));
	}
	
	float2 YA = 0;
	float2 UV = 0;
	for(int i = 0;i < 8 ;i ++)
	{
		YA  += vPixel[nYPixel[i]].xw   * vCoefY[i];
		UV += vPixel[nUVPixel[i]].yz * vCoefUV[i];
	}
	fColor = float4(YA.x,UV,YA.y);
	
	return fColor;
}
float4 PS_P8_H_444(PS_INPUT input):SV_Target
{
	float4 fColor = (float4)0;	
	float2 Tex = input.vTex.xy;
	float fX = Tex.x * g_txSize.x + 1e-3;
	float4 vCoef0 = g_txCoef.Sample(samPoint,float2(0.1f / 2.0f,frac(fX) + 0.1f / 16.0f));	
	float4 vCoef1 = g_txCoef.Sample(samPoint,float2(1.1f / 2.0f,frac(fX) + 0.1f / 16.0f));	
		
	Tex.x = int(fX) * g_txSize.z;
	Tex += 0.5f * g_txSize.zw;
	
	int i = 0;
	for(i = -3 ;i < 1;i ++)		
		fColor += g_txColor.Sample(samPoint,clamp(Tex + float2(i,0.0f) * g_txSize.zw,float2(g_matTex._41,g_matTex._42),float2(g_matTex._43,g_matTex._44))) * vCoef0[i + 3];
	
	for(i = 1 ;i < 5;i ++)		
		fColor += g_txColor.Sample(samPoint,clamp(Tex + float2(i,0.0f) * g_txSize.zw,float2(g_matTex._41,g_matTex._42),float2(g_matTex._43,g_matTex._44))) * vCoef1[i - 1];
		
	return fColor;
}
float4 PS_P8_V(PS_INPUT input):SV_Target
{
	float2 Tex = input.vTex.xy;
		
	float fEdge = (0.5f - abs(input.vTex.w - 0.5f)) * g_txSize.y;
	fEdge = smoothstep(0.0f,1.0f,fEdge);	
		
	float fY = 	Tex.y * g_txSize.y + g_fOffset * fEdge + 1e-3;
	float4 vCoef0 = g_txCoef.Sample(samPoint,float2(0.1f / 2.0f,frac(fY) + 0.1f / 16.0f));	
	float4 vCoef1 = g_txCoef.Sample(samPoint,float2(1.1f / 2.0f,frac(fY) + 0.1f / 16.0f));		
		
	Tex.y = int(fY) * g_txSize.w;
	
	Tex += 0.5f * g_txSize.zw;	
		
	float4 fColor = (float4)0;	
	int i = 0;
	for(i = -3 ;i < 1;i ++)		
		fColor += g_txColor.Sample(samPoint,clamp(Tex + float2(0.0f,i) * g_txSize.zw,float2(g_matTex._41,g_matTex._42),float2(g_matTex._43,g_matTex._44))) * vCoef0[i + 3];
	
	for(i = 1 ;i < 5;i ++)		
		fColor += g_txColor.Sample(samPoint,clamp(Tex + float2(0.0f,i) * g_txSize.zw,float2(g_matTex._41,g_matTex._42),float2(g_matTex._43,g_matTex._44))) * vCoef1[i - 1];		
	
	return fColor;
}

technique11 P4
{
	PASS P0//Hor
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, PS_P4_H() ) );
        
        SetBlendState( NoAlphaBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );	
	}
	PASS P1//Hor
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, PS_P4_H() ) );
        
        SetBlendState( NoAlphaBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );	
	}
	PASS P2//Vert
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, PS_P4_V() ) );
        
        SetBlendState( NoAlphaBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );	
	}
}
technique11 P8
{
	PASS P0//Hor
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, PS_P8_H_444() ) );
        
        SetBlendState( NoAlphaBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );	
	}
	PASS P1//Hor
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, PS_P8_H_422() ) );
        
        SetBlendState( NoAlphaBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
	}
	PASS P2//vert
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL);
        SetPixelShader( CompileShader( ps_4_0, PS_P8_V() ) );
        
        SetBlendState( NoAlphaBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );	
	}
}