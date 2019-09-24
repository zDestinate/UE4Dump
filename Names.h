#pragma once

#include <set>
#include <string>
#include <cctype>
#include <locale>

template<class T>
struct TArray
{
	friend struct FString;

public:
	inline TArray()
	{
		Data = nullptr;
		Count = Max = 0;
	};

	inline int Num() const
	{
		return Count;
	};

	inline T& operator[](int i)
	{
		return Data[i];
	};

	inline const T& operator[](int i) const
	{
		return Data[i];
	};

	inline bool IsValidIndex(int i) const
	{
		return i < Num();
	}

private:
	T* Data;
	__int32 Count;
	__int32 Max;
};

#define NAME_WIDE_MASK 0x1
#define NAME_INDEX_SHIFT 1

class FNameEntry
{
public:
	FNameEntry* HashNext;
	__int32 Index;

	union
	{
		char AnsiName[1024];
		wchar_t WideName[1024];
	};

	inline const __int32 GetIndex() const
	{
		return Index >> NAME_INDEX_SHIFT;
	}

	inline bool IsWide() const
	{
		return Index & NAME_WIDE_MASK;
	}

	inline const char* GetAnsiName() const
	{
		return AnsiName;
	}

	inline const wchar_t* GetWideName() const
	{
		return WideName;
	}
};

template<typename ElementType, __int32 MaxTotalElements, __int32 ElementsPerChunk>
class TStaticIndirectArrayThreadSafeRead
{
public:
	inline size_t Num() const
	{
		return NumElements;
	}

	inline bool IsValidIndex(__int32 index) const
	{
		return index < Num() && index >= 0 && GetById(index) != nullptr;
	}

	inline ElementType const* const& GetById(int index) const
	{
		ElementType* ItemPtr = GetItemPtr(index);
		return ItemPtr;
	}

	inline ElementType const* const& operator[](__int32 index) const
	{
		ElementType* ItemPtr = GetItemPtr(index);
		return ItemPtr;
	}

private:
	inline ElementType* GetItemPtr(__int32 Index) const
	{
		__int32 ChunkIndex = Index / ElementsPerChunk;
		__int32 WithinChunkIndex = Index % ElementsPerChunk;
		ElementType* Chunk = (*Chunks)[ChunkIndex];
		if (ChunkIndex > NumChunks) return nullptr;
		if (Index > MaxTotalElements) return nullptr;
		if (!Chunk) return nullptr;
		int offset = WithinChunkIndex * 0x8;
		uintptr_t ptrAddress = *reinterpret_cast<uintptr_t*>(reinterpret_cast<unsigned char*>(Chunk) + offset);
		ElementType* ItemPtr = reinterpret_cast<ElementType*>(ptrAddress);
		return ItemPtr;
	}

	enum
	{
		ChunkTableSize = (MaxTotalElements + ElementsPerChunk - 1) / ElementsPerChunk
	};

	ElementType** Chunks[ChunkTableSize];
	__int32 NumElements;
	__int32 NumChunks;
};

using TNameEntryArray = TStaticIndirectArrayThreadSafeRead<FNameEntry, 2 * 1024 * 1024, 16384>;

struct FName
{
	union
	{
		struct
		{
			__int32 ComparisonIndex;
			__int32 Number;
		};

		__int64 CompositeComparisonValue;
	};

	inline FName()
		: ComparisonIndex(0),
		Number(0)
	{
	};

	inline FName(__int32 i)
		: ComparisonIndex(i),
		Number(0)
	{
	};

	FName(const char* nameToFind)
		: ComparisonIndex(0),
		Number(0)
	{
		static std::set<int> cache;

		for (auto i : cache)
		{
			if (!std::strcmp(GetGlobalNames()[i]->GetAnsiName(), nameToFind))
			{
				ComparisonIndex = i;

				return;
			}
		}

		for (auto i = 0; i < GetGlobalNames().Num(); ++i)
		{
			if (GetGlobalNames()[i] != nullptr)
			{
				if (!std::strcmp(GetGlobalNames()[i]->GetAnsiName(), nameToFind))
				{
					cache.insert(i);

					ComparisonIndex = i;

					return;
				}
			}
		}
	};

	static TNameEntryArray* GNames;
	static inline TNameEntryArray& GetGlobalNames()
	{
		return *GNames;
	};

	inline const char* GetName() const
	{
		return GetGlobalNames()[ComparisonIndex]->GetAnsiName();
	};

	inline bool operator==(const FName& other) const
	{
		return ComparisonIndex == other.ComparisonIndex;
	};
};

struct FString : private TArray<wchar_t>
{
	inline FString()
	{
	};

	FString(const wchar_t* other)
	{
		Max = Count = *other ? std::wcslen(other) + 1 : 0;

		if (Count)
		{
			Data = const_cast<wchar_t*>(other);
		}
	};

	inline bool IsValid() const
	{
		return Data != nullptr;
	}

	inline const wchar_t* c_str() const
	{
		return Data;
	}

	std::string ToString() const
	{
		auto length = std::wcslen(Data);

		std::string str(length, '\0');

		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

		return str;
	}
};