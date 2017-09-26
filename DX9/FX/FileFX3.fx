
texture g_ColorTexture:register(s0); ;             // 纹理


float4x4 g_matTex;					// 纹理矩阵

float4x4 g_matWorld;                // 世界矩阵
float4x4 g_matView;                 // 观察矩阵
float4x4 g_matProject;              // 投影矩阵



sampler MeshTextureSampler_Linear = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = Linear;
    MinFilter = Linear;
    MagFilter = Linear;
    AddressU = Clamp;
	AddressV = Clamp;
  };
  
  sampler g_samPoint =  sampler_state
{	
	Texture = <g_ColorTexture>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter =  Point;
	AddressU =Border;
	AddressV =Border;
};


//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position  :POSITION;  
    float2 Tex        : TEXCOORD0;
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
    float3 Tex        : TEXCOORD0;
  };


VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;

	//Out.Position = In.Position;
	
	float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    
	Out.Position = mul(In.Position , matWorldViewProject );
	
	//Out.Position = In.Position;
	
	Out.Tex = mul(float4(In.Tex,1.0f,0.0f),g_matTex).xyz;
   
    return Out;
  }
  
  
  VS_OUTPUT YUV_VS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;

   //Out.Position = In.Position;
   
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
	Out.Position = mul(In.Position , matWorldViewProject );

   Out.Tex = mul(float4(In.Tex,1.0f,0.0f),g_matTex).xyz;
   
    return Out;
  }
  
  

float4 RenderScenePS( VS_OUTPUT In ) : COLOR0
  {
   return tex2D( MeshTextureSampler_Linear, In.Tex);
    
	float2 tex;
	tex.x =  (In.Tex).x*1920.0/128.0f;
	tex.y =  (In.Tex).y*1080.0/128.0f;
	 float4 fColor = float4(tex,0.0f,1.0f);
	return  fColor;
  }
  
  
  
//------------------------------------------------------------------------------
// 实现技术方式
//------------------------------------------------------------------------------
technique DefaultTech
  {
  
     
     PASS P0//quad
     {
     
     VertexShader = compile vs_2_0 YUV_VS( );
	 PixelShader  = compile ps_2_0 RenderScenePS( );
     
     }
     
     PASS P1//quad
     {
     
     VertexShader = compile vs_2_0 RenderSceneVS( );
	 PixelShader  = compile ps_2_0 RenderScenePS( );
     
     }
     
      
      
 }
