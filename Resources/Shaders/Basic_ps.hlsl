struct Input {
    float4 pos : SV_POSITION;
};

float4 main(Input input) : SV_TARGET {
    return float4(0.94, 0.9, 0.04, 1);
}