#pragma once
#include <math.h>
#include <stdlib.h>
#include <cmath>
#include <vector>
#include <ctime>

#define PI 3.14159265358979323846

struct FVertexSimple
{
    float x, y, z;    // Position
    float r, g, b, a; // Color
};
// Structure for a 3D vector
struct FVector3
{
    float x, y, z;
    FVector3(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}

    static const FVector3 One;
    static const FVector3 Zero;

    void Normalize()
    {
        float magnitude = sqrt(x * x + y * y + z * z);
        if (magnitude > 0)
        {
            x /= magnitude;
            y /= magnitude;
            z /= magnitude;
        }
    }

    float Length() {
        return sqrt(x * x + y * y + z * z);
    }

    static float DotProduct(const FVector3& a, const FVector3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static FVector3 CrossProduct(const FVector3& a, const FVector3& b)
    {
        return FVector3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    static float Distance(const FVector3& a, const FVector3& b)
    {
        return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) + (a.z - b.z) * (a.z - b.z));
    }

    static FVector3 RandomVector(float minValue, float maxValue) {
        float randX = minValue + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxValue - minValue);
        float randY = minValue + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxValue - minValue);
        float randZ = minValue + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxValue - minValue);

        return FVector3(randX, randY, randZ);
    }

    // Operator overloading for +
    FVector3 operator+(const FVector3& other) const {
        return FVector3(x + other.x, y + other.y, z + other.z);
    }

    // Operator overloading for -
    FVector3 operator-(const FVector3& other) const {
        return FVector3(x - other.x, y - other.y, z - other.z);
    }

    // Operator overloading for *
    FVector3 operator*(float scalar) const {
        return FVector3(x * scalar, y * scalar, z * scalar);
    }

    // Operator overloading for /
    FVector3 operator/(float scalar) const {
        return FVector3(x / scalar, y / scalar, z / scalar);
    }
};


class FMath
{
public:
    static std::vector<FVertexSimple> CreateSphereVertices(float radius, int segments, const FVector3& color)
    {
        std::vector<FVertexSimple> vertices;
        for (int y = 0; y <= segments; ++y)
        {
            for (int x = 0; x <= segments; ++x)
            {
                float xSegment = (float)x / (float)segments;
                float ySegment = (float)y / (float)segments;
                float xPos = radius * cos(xSegment * 2.0f * PI) * sin(ySegment * PI);
                float yPos = radius * cos(ySegment * PI);
                float zPos = radius * sin(xSegment * 2.0f * PI) * sin(ySegment * PI);

                FVertexSimple vertex;
                vertex.x = xPos;
                vertex.y = yPos;
                vertex.z = zPos;
                vertex.r = color.x;
                vertex.g = color.y;
                vertex.b = color.z;
                vertex.a = 1.0f;

                vertices.push_back(vertex);
            }
        }
        return vertices;
    }

    // 인덱스 버퍼를 함께 생성하는 새 함수
    static std::pair<std::vector<FVertexSimple>, std::vector<unsigned int>> CreateSphereWithIndices(
        float radius, int segments, const FVector3 color, bool _rainbow = false)
    {
        std::vector<FVertexSimple> vertices;
        std::vector<unsigned int> indices;

        // 랜덤 시드 초기화
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        // 정점 생성
        for (int y = 0; y <= segments; ++y)
        {
            for (int x = 0; x <= segments; ++x)
            {
                float xSegment = (float)x / (float)segments;
                float ySegment = (float)y / (float)segments;
                float xPos = radius * cos(xSegment * 2.0f * PI) * sin(ySegment * PI);
                float yPos = radius * cos(ySegment * PI);
                float zPos = radius * sin(xSegment * 2.0f * PI) * sin(ySegment * PI);

                FVertexSimple vertex;
                vertex.x = xPos;
                vertex.y = yPos;
                vertex.z = zPos;

                if (_rainbow)
                {
                    // 무작위로 색상 생성
                    float hue = static_cast<float>(std::rand()) / RAND_MAX; // [0, 1] 범위의 무작위 값
                    float saturation = 0.7f; // 채도 조절
                    float value = 1.0f;      // 명도 조절

                    // HSV에서 RGB로 변환
                    int h_i = static_cast<int>(hue * 6);
                    float f = hue * 6 - h_i;
                    float p = value * (1 - saturation);
                    float q = value * (1 - f * saturation);
                    float t = value * (1 - (1 - f) * saturation);

                    switch (h_i % 6)
                    {
                    case 0: vertex.r = value; vertex.g = t; vertex.b = p; break;
                    case 1: vertex.r = q; vertex.g = value; vertex.b = p; break;
                    case 2: vertex.r = p; vertex.g = value; vertex.b = t; break;
                    case 3: vertex.r = p; vertex.g = q; vertex.b = value; break;
                    case 4: vertex.r = t; vertex.g = p; vertex.b = value; break;
                    case 5: vertex.r = value; vertex.g = p; vertex.b = q; break;
                    }
                }
                else
                {
                    // 기본 단색 사용
                    vertex.r = color.x;
                    vertex.g = color.y;
                    vertex.b = color.z;
                }

                vertex.a = 1.0f;

                vertices.push_back(vertex);
            }
        }

        // 인덱스 생성
        for (int y = 0; y < segments; ++y)
        {
            for (int x = 0; x < segments; ++x)
            {
                // 현재 사각형의 4개 정점 인덱스
                unsigned int topLeft = y * (segments + 1) + x;
                unsigned int topRight = topLeft + 1;
                unsigned int bottomLeft = (y + 1) * (segments + 1) + x;
                unsigned int bottomRight = bottomLeft + 1;

                // 첫 번째 삼각형
                indices.push_back(topLeft);
                indices.push_back(bottomLeft);
                indices.push_back(topRight);

                // 두 번째 삼각형
                indices.push_back(topRight);
                indices.push_back(bottomLeft);
                indices.push_back(bottomRight);
            }
        }

        return std::make_pair(vertices, indices);
    }

};