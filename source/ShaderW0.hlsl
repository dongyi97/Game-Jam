// ShaderW0.hlsl
cbuffer vertexConstants : register(b0)
{
    float3 Offset;
    float OffsetPad;
    float3 Scale;
    float ScalePad;
    float3 Color;
    float ColorPad;
    float3 Rotate;
    float RotatePad;
    int Trans;
}

cbuffer transformConstants : register(b1)
{
    matrix worldViewProj;
}

cbuffer lightBuffer : register(b2)
{
    float3 LightDirection;
    float Padding; // 정렬을 맞추기 위한 패딩
    float3 LightColor;
}

struct VS_INPUT
{
    float4 position : POSITION; // Input position from vertex buffer
    float4 color : COLOR; // Input color from vertex buffer
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // Transformed position to pass to the pixel shader
    float4 color : COLOR; // Color to pass to the pixel shader
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;

    // X축 회전 행렬
    float4x4 RotationMatrixX =
    {
        1, 0, 0, 0,
        0, cos(Rotate.x), -sin(Rotate.x), 0,
        0, sin(Rotate.x), cos(Rotate.x), 0,
        0, 0, 0, 1
    };

    // Y축 회전 행렬
    float4x4 RotationMatrixY =
    {
        cos(Rotate.y), 0, sin(Rotate.y), 0,
        0, 1, 0, 0,
        -sin(Rotate.y), 0, cos(Rotate.y), 0,
        0, 0, 0, 1
    };

    // Z축 회전 행렬
    float4x4 RotationMatrixZ =
    {
        cos(Rotate.z), -sin(Rotate.z), 0, 0,
        sin(Rotate.z), cos(Rotate.z), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

    float4 scaledPosition = input.position * float4(Scale, 1.0);
    // 전체 회전 행렬
    float4x4 RotationMatrix = mul(mul(RotationMatrixX, RotationMatrixY), RotationMatrixZ);
    float4 rotatedPosition = mul(scaledPosition, RotationMatrix); // Apply rotation
    float4 offsetPosition = rotatedPosition + float4(Offset, 0.0f);
    output.position = mul(offsetPosition, worldViewProj);
    
    
    
    if (Color.r == -1)
    {
        output.color = input.color;
    }
    else
    {
        output.color = float4(Color, 1.0f);
    }
    
    if (Trans > 0)
    {
        output.color = input.color;
        output.color.w = 0.2f;
    }
   
    
    

    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // Output the color directly
    return input.color;
}