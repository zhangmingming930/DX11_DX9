#include"./native_color.INC"


cbuffer cbPerObject
{
	float4x4 g_worldViewProj; 
	float4x4 g_matTex;
	float g_height;
	float g_width;
};


DepthStencilState DisableDepth
{	
    DepthEnable = False;    
};
BlendState NoAlphaBlend
{	    
};
RasterizerState DisableCulling
{
    CullMode = NONE;   
};



//纹理  Texture2D的定义不能放在cbuffer中
Texture2D g_tex;

//设置过滤方式
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

struct VertexIn
{
	float3 pos : POSITION;
	float2 tex : TEXCOORD;
};

struct VertexOut
{
	float4 posH  : SV_POSITION;
	float2 tex : TEXCOORD;	   	
};



VertexOut VS(VertexIn vin)
{
	VertexOut vout;	
	vout.posH = mul(float4(vin.pos, 1.0f), g_worldViewProj);	
	
    	//vout.tex = vin.tex;  
    	
    	vout.tex = mul(float4(vin.tex,1.0,1.0),g_matTex).xy;	
    		
    	//vout.tex.x = vout.tex.x - 0.5/g_width;
    	//vout.tex.y = vout.tex.y - 0.5/g_height;
    	
    	return vout;
}




float4 PS(VertexOut pin,uniform SamplerState samColor) : SV_Target
{
	
   
	// float2 Tex = pin.posH.xy;//这是视口的大小 g_ScreenResolution 窗口解析度
	 
	//Tex.x /= g_width*(200.f/256.f);
	//Tex.y /= g_height*(200.f/256.f);
   
	//Tex.x = 1.0f/ (g_width/2.f);  //采一点
	//Tex.y = 1.0f/ (g_height/2.f);
	
	//纹理颜色数据
	//return g_tex.Sample(samColor, Tex);
	
	//pin.tex.x =1.0f/g_width*(256.f/256.f);
	//pin.tex.y =5.0f/g_height*(256.f/256.f);
	
	//pin.tex = 1.0f/g_height;
	
	return g_tex.Sample(samColor,pin.tex);
	
	
	
	//纹理坐标数据
	//float2 tex;
	//tex.x = pin.tex.x*1920.0/256.0f;
	//tex.y =  pin.tex.y*1080.0/256.0f;
	
      float4 fColor = float4(pin.tex,0.0f,1.0f);

	return  fColor ; 
}


technique11 TextureTech
{
    pass P0 //derect
    {
        SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(samPoint) ) );
        
        
         SetBlendState( NoAlphaBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }
    pass P1	//native
    {
		SetVertexShader( CompileShader( vs_5_0, VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_5_0, PS(samLinear) ) );
        
         SetBlendState( NoAlphaBlend, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
        SetDepthStencilState( DisableDepth, 0 );
        SetRasterizerState( DisableCulling );
    }
}