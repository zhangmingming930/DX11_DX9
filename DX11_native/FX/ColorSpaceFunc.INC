cbuffer IMMUTABLE
{
	float4x4 matRGBA2YUVA;
	float4x4 matYUVA2RGBA;
	float4 g_vUVOffset;
	float4 g_vSafeColor;
	float4 g_vExtendInfo;
};
///////////////////////////////////////////////////////////////////////////////
// UV 偏移定义

//#define CS_UV_OFFSET		128.f / 255.f
//#define CS_UV_OFFSET_2		256.f / 255.f

#define CS_UV_OFFSET		g_vUVOffset.x
#define CS_UV_OFFSET_2		g_vUVOffset.y

#define CS_YUVA_OFFSET		float4(0.0f, CS_UV_OFFSET, CS_UV_OFFSET, 0.0f)

//#define LUM_LIMIT_LOW		(16.0 / 255.0f)
//#define LUM_LIMIT_HEIGHT	(235.0 / 255.0f)

//#define LUM_LIMIT_16_235(y) ( (y) * (235.0f - 16.0f) / 255.0f + 16 / 255.0f)
//#define LUM_LIMIT_16_235_INV(y)	( ((y)-16/255.f) * 255.f / (235.f-16.f) )

#define LUM_LIMIT_LOW		(g_vSafeColor.x / g_vExtendInfo.y)
#define LUM_LIMIT_HEIGHT	(g_vSafeColor.y / g_vExtendInfo.y)
#define UV_LIMIT_LOW		(g_vSafeColor.z / g_vExtendInfo.y)
#define UV_LIMIT_HEIGHT		(g_vSafeColor.w / g_vExtendInfo.y)

#define LUM_LIMIT_16_235(y) ( (y) * (LUM_LIMIT_HEIGHT - LUM_LIMIT_LOW) + LUM_LIMIT_LOW)
#define LUM_LIMIT_16_235_INV(y)	( ((y) - LUM_LIMIT_LOW) / (LUM_LIMIT_HEIGHT - LUM_LIMIT_LOW) )
#define UV_LIMIT_16_240(y) ( (y) * (UV_LIMIT_HEIGHT - UV_LIMIT_LOW) + UV_LIMIT_LOW)
#define UV_LIMIT_16_240_INV(y)	( ((y) - UV_LIMIT_LOW) / (UV_LIMIT_HEIGHT - UV_LIMIT_LOW) )

// bitCount
#define CS_BITCOUNT		g_vExtendInfo.x

///////////////////////////////////////////////////////////////////////////////
// 接口定义

// YUVA 与 RGBA 之间的变换

float CS_RGB2Y(in float3 rgb)
{
	return dot(matRGBA2YUVA[0].xyz, rgb);
}

float CS_RGB2U(in float3 rgb)
{
	return dot(matRGBA2YUVA[1].xyz, rgb);
}

float CS_RGB2V(in float3 rgb)
{
	return dot(matRGBA2YUVA[2].xyz, rgb);
}

float3 CS_RGB2YUV(in float3 rgb)
{
	return mul((float3x3)matRGBA2YUVA, rgb);
}

float3 CS_YUV2RGB(in float3 yuv)
{
	return saturate(mul((float3x3)matYUVA2RGBA, yuv));
}




