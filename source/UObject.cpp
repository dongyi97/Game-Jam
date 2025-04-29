#include "UObject.h"
#include <list>
#include <memory>

void UObject::AllReleaseCheck()
{
}

void UObject::AllRender()
{

}

void UObject::AllUpdate(float _Delta)
{
	Update(_Delta);
	for (std::pair<const int, std::list<UObject*>>& _Pair : Childs)
	{
		std::list<UObject*>& Group = _Pair.second;
		for (UObject* _Child : Group)
		{
			if (false == _Child->IsUpdate())
			{
				continue;
			}
			_Child->Update(_Delta);
		}
	}
}
