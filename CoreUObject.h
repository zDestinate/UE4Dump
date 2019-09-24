#pragma once

#include <string>
#include "global.h"

class UClass {};

template<typename Fn>
inline Fn GetVFunction(const void* instance, std::size_t index)
{
	auto vtable = *reinterpret_cast<const void***>(const_cast<void*>(instance));
	return reinterpret_cast<Fn>(vtable[index]);
};

class UObject;

/*
class UObject
{
public:
	void* Vtable;
	int32_t ObjectFlags;
	int32_t InternalIndex;
	UClass* Class;
	FName Name;
	UObject* Outer;

	std::string GetName() const
	{
		std::string name(Name.GetName());
		if (Name.Number > 0)
		{
			name += '_' + std::to_string(Name.Number);
		}

		auto pos = name.rfind('/');
		if (pos == std::string::npos)
		{
			return name;
		}

		return name.substr(pos + 1);
	}

	std::string GetFullName() const
	{
		std::string name;

		if (this != nullptr)
		{
			std::string temp;
			for (auto p = Outer; p; p = p->Outer)
			{
				temp = p->GetName() + "." + temp;
			}

			name = this->GetName();
			name += " ";
			name += temp;
			name += GetName();
		}

		return name;
	}

	template<typename T>

	static T* FindObject(const std::string& name)
	{
		for (int i = 0; i < GlobalObjects->ObjObjects.Num(); ++i) {
			auto object = GlobalObjects->ObjObjects.GetByIndex(i);

			if (object == nullptr)
			{
				continue;
			}

			if (object->GetFullName().c_str() == name)
			{
				return reinterpret_cast<T*>(object);
			}
		}
		return nullptr;
	}

	static UClass* FindClass(const std::string& name)
	{
		return FindObject<UClass>(name);
	}

	template<typename T>
	static T* GetObjectCasted(std::size_t index)
	{
		return static_cast<T*>(GlobalObjects->ObjObjects.GetByIndex(index));
	}

	static UClass* StaticClass()
	{
		static auto ptr = UObject::FindClass("Class CoreUObject.Object");
		return ptr;
	}

	inline void ProcessEvent(class UFunction* function, void* parms)
	{
		return GetVFunction<void(__thiscall*)(UObject*, class UFunction*, void*, void*)>(this, 76)(this, function, parms, nullptr);
	}
};
*/