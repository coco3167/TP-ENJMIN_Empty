Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

struct Input {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float4 main(Input input) : SV_TARGET {
    float4 color = tex.Sample(samplerState, (input.uv+float2(12,8))/16);
    //color = float4(input.uv, 0, 1);

    clip(color.w - 0.01f);
    return color;
}