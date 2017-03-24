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

VS_OUTPUT main( VS_INPUT In )
{
	VS_OUTPUT Out;
	Out.Position = In.Position;
	Out.Color = In.Color;
	return Out;
}