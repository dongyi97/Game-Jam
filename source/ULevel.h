#pragma once
#include "UObject.h"
#include <list>
#include <map>
#include <set>
#include <memory>
#include <vector>
#include "FEnum.h"

class ULevel : public UObject
{
public:
	ULevel() : UObject(nullptr) {}


protected:

public:

	class UCamera* CreateCamera(int _Order, ECAMERAORDER _CameraOrder)
	{
		return CreateCamera(_Order, static_cast<int>(_CameraOrder));
	}

	class UCamera* CreateCamera(int _Order, int _CameraOrder);

	void Start() override;

	////void AllUpdate(float _Delta) override; 

	//void Release() override;

	//void AllReleaseCheck() override;

	void AllRender(float _Delta);

	template<typename ObjectType>
	ObjectType* CreateObject(int _Order = 0)
	{
		ObjectType* NewObject = new ObjectType();
		NewObject->Start();
		Childs[_Order].push_back(NewObject);
		return NewObject;
	}


	void  Update(float _Delta) override;

private:


	std::map<int,class UCamera*> Cameras;

};

