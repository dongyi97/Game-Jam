#pragma once
#include <list>
#include <map>
#include <memory>
#include "FMath.h"

struct FTransform {
	FVector3 Pos;
	FVector3 Rotate;
	FVector3 Scale;
};

class UObject
{
public:
	virtual void Start() {}
	virtual void Update(float _Delta) {}
	virtual void Release() {}
	virtual void LevelStart(class ULevel* _NextLevel) {}
	virtual void LevelEnd(class ULevel* _NextLevel) {}


	virtual void AllUpdate(float _Delta); // �ڽ��� ������Ʈ�� ��� ������ �ڽĵ��� ������Ʈ�� ��� ����
	virtual void AllReleaseCheck();

	UObject(UObject* _parent) : Parent(_parent) {}
	virtual void AllRender();


	bool IsDeath() 
	{
		return Parent == nullptr ? IsDeathValue : Parent->IsDeath() || IsDeathValue;
	}

	bool IsUpdate() 
	{
		return Parent == nullptr ? true == IsUpdateValue && false == IsDeathValue : Parent->IsUpdate() && true == IsUpdateValue && false == IsDeath();
	}


	const FTransform& Gettransform()
	{
		return Transform;
	}

	void Settransform(const FTransform& _transform)
	{
		Transform = _transform;
	}

public:

	// Transform ���� getter/setter �Լ���
	FTransform GetTransform() const { return Transform; }
	void SetTransform(const FTransform& transform) { Transform = transform; }

	// Position ���� getter/setter
	FVector3 GetPosition() const { return Transform.Pos; }
	void SetPosition(const FVector3& pos) { Transform.Pos = pos; }
	void SetPosition(float x, float y, float z) { Transform.Pos = FVector3(x, y, z); }

	// Rotation ���� getter/setter
	FVector3 GetRotation() const { return Transform.Rotate; }
	void SetRotation(const FVector3& rotate) { Transform.Rotate = rotate; }
	void SetRotation(float x, float y, float z) { Transform.Rotate = FVector3(x, y, z); }

	// Scale ���� getter/setter
	FVector3 GetScale() const { return Transform.Scale; }
	void SetScale(const FVector3& scale) { Transform.Scale = scale; }
	void SetScale(float x, float y, float z) { Transform.Scale = FVector3(x, y, z); }
	void SetUniformScale(float scale) { Transform.Scale = FVector3(scale, scale, scale); }
	

	void Move(FVector3 dir) { Transform.Pos = Transform.Pos + dir; }

	virtual void Render() {}

protected:
	FTransform Transform;

	UObject* Parent = nullptr;
	std::map<int, std::list<UObject*>> Childs;


private:
	float LiveTime = 0.0f;
	bool IsUpdateValue = true;
	bool IsDeathValue = false;

};

