#pragma once
#include "Sphere.h"
#include <d3d11.h>

// ----- Variables
//URenderer* renderer;
//UINT numVerticesSphere = sizeof(sphere_vertices) / sizeof(FVertexSimple);
//ID3D11Buffer* vertexBufferSphere;
//
//const FVector3 GRAVITY = FVector3(0, -9.8f, 0);
//
//const float LEFT_BORDER = -1.0f;
//const float RIGHT_BORDER = 1.0f;
//const float TOP_BORDER = 1.0f;
//const float BOTTOM_BORDER = -1.0f;
//
//bool bIsGravity = false;
//bool bIsMagnetic = false;
//bool bIsRotate = false;
 
extern UINT numVerticesSphere;
extern ID3D11Buffer* vertexBufferSphere;
extern ID3D11Buffer* vertexBufferSphereTrans;
extern ID3D11Buffer* indexBufferSphere; // ÀÎµ¦½º ¹öÆÛ

extern const FVector3 GRAVITY;

extern const float LEFT_BORDER;
extern const float RIGHT_BORDER;
extern const float TOP_BORDER;
extern const float BOTTOM_BORDER;

extern bool bIsGravity;
extern bool bIsMagnetic;
extern bool bIsRotate;