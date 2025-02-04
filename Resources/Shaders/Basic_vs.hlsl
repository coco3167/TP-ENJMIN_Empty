struct Input {
	float3 pos : POSITION0;
};

struct Output {
	float4 pos : SV_POSITION;
};

cbuffer ModelData : register(b0)
{
	float4x4 Model;
}

cbuffer CameraData : register(b1)
{
	float4x4 View;
	float4x4 Projection;
}

Output main(Input input) {
	Output output = (Output)0;

	float4 pos = float4(input.pos, 1.0);
	pos = mul(pos, Model);
	pos = mul(pos, View);
	pos = mul(pos, Projection);

    output.pos = pos;
	

	return output;
}