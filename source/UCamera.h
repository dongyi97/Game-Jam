#pragma once
#include "UObject.h"
#include "DirectXMath.h"
#include "FMath.h"

struct FViewportInfo {
	float topLeftX; // 0~1 시작 x 좌표
	float topLeftY; // 0~1 시작 y 좌표
	float width; // 0~1 가로 크기
	float height; // 0~1 세로 크기
};

enum class ECameraProjectionType {
	Perspective,
	Orthographic
};

class URenderer;

class UCamera : public UObject
{
public:

	UCamera(UObject* _parent) : UObject(_parent) {}

	static UCamera* MainCamera;

	static UCamera* TranslucentCamera;
	static UCamera* SubCamera;

	void Start() override;

	float CameraHorizontalAngle() { return cameraHorizontalAngle; }
	float CameraVerticalAngle() { return cameraVerticalAngle; }

	void SetProjectionType(ECameraProjectionType _projectionType) { projectionType = _projectionType; }
	ECameraProjectionType ProjectionType() const { return projectionType; }

	void SetViewPort(float topLeftX, float topLeftY, float width, float height) { 
		viewportInfo.topLeftX = topLeftX;
		viewportInfo.topLeftY = topLeftY;
		viewportInfo.width = width;
		viewportInfo.height = height;
	}

	void SetTarget(FVector3 _Target) { Target = _Target; }
	void SetTarget(float x, float y, float z) { Target = FVector3(x, y, z); }

	void UpdateCamera();

	void SetCameraHorizontalAngle(float _cameraHorizontalAngle) { cameraHorizontalAngle = _cameraHorizontalAngle; }
	void AddCameraHorizontalAngle(float _cameraHorizontalAngle) { cameraHorizontalAngle += _cameraHorizontalAngle; }
	void SetCameraVerticalAngle(float _cameraVerticalAngle) { cameraVerticalAngle = _cameraVerticalAngle; }
	void AddCameraVerticalAngle(float _cameraVerticalAngle) { cameraVerticalAngle += _cameraVerticalAngle; }

private:
	URenderer* renderer;

	ECameraProjectionType projectionType = ECameraProjectionType::Orthographic;

	float Far = 100.0f;
	float Near = 0.1f;

	// 원근 투영 관련 속성들
	float FOV = 60.0f;
	float ZoomValue = 0.0f;

	// 직교 투영 관련 속성들
	float OrthoWidth = 5.0f;  // 직교 투영 너비
	float OrthoHeight = 5.0f;  // 직교 투영 높이

	FVector3 Target;
	FVector3 UpVector;

	// 뷰 행렬과 투영 행렬
	DirectX::XMMATRIX ViewMatrix;
	DirectX::XMMATRIX ProjectionMatrix;

	FViewportInfo viewportInfo;

	float cameraHorizontalAngle = 0.0f;
	float cameraVerticalAngle = 0.0f;
};

