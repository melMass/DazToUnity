#ifndef MTB_NODES
#define MTB_NODES

void MTB_NormalFromTexture_float(Texture2D Texture, SamplerState Sampler, float2 UV, float Offset, float Strength, out float3 Out)
 {
     Offset = pow(Offset, 3) * 0.1;
     const float2 offset_u = float2(UV.x + Offset, UV.y);
     const float2 offset_v = float2(UV.x, UV.y + Offset);
     const float4 n_vec = Texture.Sample(Sampler, UV);
     const float normal_sample = (n_vec.x + n_vec.y + n_vec.z) / 3;
     const float4 u_vec =  Texture.Sample(Sampler, offset_u);
     const float4 v_vec =  Texture.Sample(Sampler, offset_v);
     const float u_sample = (u_vec.x + u_vec.y + u_vec.z) / 3;
     const float v_sample = (v_vec.x + v_vec.y + v_vec.z) / 3;
     const float3 va = float3(1, 0, (u_sample - normal_sample) * Strength);
     const float3 vb = float3(0, 1, (v_sample - normal_sample) * Strength);
     Out = normalize(cross(va, vb));
 }

#endif