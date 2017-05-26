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

matrix ModelMatrix;

VS_OUTPUT vsmain(VS_INPUT In)
{
	VS_OUTPUT Out;
	Out.Position = mul(In.Position, ModelMatrix);
	//Out.Position = In.Position;
	Out.Color = In.Color;
	return Out;
}

float4 psmain(VS_OUTPUT In) : SV_TARGET
{
	return In.Color;
}