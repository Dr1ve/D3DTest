cbuffer cbPerObject
{
	float4x4 WVP;
};

struct VertexInputType
{
	float4 Pos : POSITION;
	float4 Norm : NORMAL;
	float2 Tex : TEXCOORD;
};

struct PixelInputType
{
	float4 Pos :SV_POSITION;
	float4 Norm : NORMAL;
	float2 Tex : TEXCOORD;
};

PixelInputType VS(VertexInputType input)
{
	PixelInputType output;

	output.Pos = mul(input.Pos, WVP);
	output.Tex = input.Tex;

	return output;
}