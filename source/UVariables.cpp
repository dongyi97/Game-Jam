#include "UVariables.h"

UINT numVerticesSphere = sizeof(sphere_vertices) / sizeof(FVertexSimple);
ID3D11Buffer* vertexBufferSphere;
ID3D11Buffer* vertexBufferSphereTrans;
ID3D11Buffer* indexBufferSphere; // 인덱스 버퍼

const FVector3 GRAVITY = FVector3(0, -9.8f, 0);

const float LEFT_BORDER = -1.0f;
const float RIGHT_BORDER = 1.0f;
const float TOP_BORDER = 1.0f;
const float BOTTOM_BORDER = -1.0f;

bool bIsGravity = false;
bool bIsMagnetic = false;
bool bIsRotate = false;