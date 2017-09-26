
texture g_ColorTexture:register(s0); ;             // 纹理


float4x4 g_matTex;					// 纹理矩阵
float4x4 g_matWorldViewProj;
float4x4 g_matPosoffset;
float4 g_txSize;



sampler g_samLinear= sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter =	Linear;
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
	MipFilter = None;
	AddressU =Clamp;
	AddressV =Clamp;
};


struct VS_INPUT
  {
    float3 Position  :POSITION;  
    float2 Tex        : TEXCOORD0;
  };
struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
    float3 Tex        : TEXCOORD0;
  };

  
  float4 final_position(float4 screen_pos)
{
	float2 texel_offset = float2(-g_txSize.z,g_txSize.w);
	
    float4 result = screen_pos;
    
    result.xy += texel_offset*result.ww;
    return result;
}

  VS_OUTPUT YUV_VS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0; 
	
	Out.Position =mul(float4(In.Position ,1.0f), g_matWorldViewProj); 
	
	//Out.Position =mul(Out.Position,g_matPosoffset);  //顶点坐标朝左上方偏移0.5个像素。  A
	//Out.Position = final_position(Out.Position);                                //改顶点坐标偏移量   ok   纹理不用偏移  同上				B
  
  
  ///////////////// Out.Tex.xy = float2(0.5f, -0.5f)*In.Position.xy + 0.5f.xx;				  //改顶点坐标偏移量   不用
   
   
   
   
   Out.Tex = mul(float4(In.Tex,1.0f,0.0f),g_matTex).xyz;
   
    return Out;
  }
  
  

  float4 RenderScenePS( VS_OUTPUT In, uniform sampler samColor) : COLOR0
  {
	float2 Tex = In.Tex.xy;
	
	//Tex += 0.5f*(256.f/200.f) * g_txSize.zw; //缩放倍数偏移
	Tex += 0.5f* g_txSize.zw ;                        //正常大小偏移  (0.5f/viewport)
	
	//Tex.x = 1.0f/ (g_txSize.x/2.f);  //采一点
	//Tex.y = 1.0f/ (g_txSize.y/2.f);
	
	return tex2D( samColor, Tex);  //偏移颜色
  
	
	//return tex2D( samColor, In.Tex); //未偏移颜色
    
    
	//float2 tex;
	//tex.x =  (Tex).x*1920.0/256.0f;
	//tex.y =  (Tex).y*1080.0/256.0f;
	
	float4 fColor = float4(Tex,0.0f,1.0f); // 输出纹理坐标

	
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
	 PixelShader  = compile ps_2_0 RenderScenePS(g_samPoint);
	 
	CullMode = None;
	ZEnable = FALSE;
	AlphaBlendEnable = FALSE;
     
     }
     
     PASS P1//RT
     {
     
     VertexShader = compile vs_2_0 YUV_VS( );
	 PixelShader  = compile ps_2_0 RenderScenePS(g_samLinear);
	 
		CullMode = None;
		ZEnable = FALSE;
		AlphaBlendEnable = FALSE;
     }
     
      
      
 }
