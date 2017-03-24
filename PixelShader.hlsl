struct VS_INPUT
{
	float4 Position   : POSITION;
	float4 Color      : COLOR;
};

struct VS_OUTPUT
{
	float4 Position   : SV_POSITION;
	float4 Color      : COLOR;
};

float4 main(VS_OUTPUT In) : SV_TARGET
{
	return In.Color;
}