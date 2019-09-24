#pragma once

class UObject;

class FUObjectItem
{
public:
	UObject* Object;
	__int32 Flags;
	__int32 ClusterIndex;
	__int32 SerialNumber;
	char Unknown[0x4];

	enum class ObjectFlags : __int32
	{
		None = 0,
		Native = 1 << 25,
		Async = 1 << 26,
		AsyncLoading = 1 << 27,
		Unreachable = 1 << 28,
		PendingKill = 1 << 29,
		RootSet = 1 << 30,
		NoStrongReference = 1 << 31
	};

	inline bool IsUnreachable() const
	{
		return !!(Flags & static_cast<std::underlying_type_t<ObjectFlags>>(ObjectFlags::Unreachable));
	}
	inline bool IsPendingKill() const
	{
		return !!(Flags & static_cast<std::underlying_type_t<ObjectFlags>>(ObjectFlags::PendingKill));
	}
};

class TUObjectArray
{
	enum
	{
		NumElementsPerChunk = 64 * 1024,
	};
public:
	inline int32_t Num() const
	{
		return NumElements;
	}
	inline int32_t Max() const
	{
		return MaxElements;
	}
	inline bool IsValidIndex(int32_t Index) const
	{
		return Index < Num() && Index >= 0;
	}
	inline FUObjectItem* GetObjectPtr(int32_t Index) const
	{
		const int32_t ChunkIndex = Index / NumElementsPerChunk;
		const int32_t WithinChunkIndex = Index % NumElementsPerChunk;
		if (!IsValidIndex(Index)) return nullptr;
		if (ChunkIndex > NumChunks) return nullptr;
		if (Index > MaxElements) return nullptr;
		FUObjectItem* Chunk = Objects[ChunkIndex];
		if (!Chunk) return nullptr;
		return Chunk + WithinChunkIndex;
	}
	inline UObject* GetByIndex(int32_t index) const
	{
		FUObjectItem* ItemPtr = GetObjectPtr(index);
		if (!ItemPtr) return nullptr;
		return (*ItemPtr).Object;
	}
	inline FUObjectItem* GetItemByIndex(int32_t index) const
	{
		FUObjectItem* ItemPtr = GetObjectPtr(index);
		if (!ItemPtr) return nullptr;
		return ItemPtr;
	}
private:
	FUObjectItem** Objects;
	FUObjectItem* PreAllocatedObjects;
	int32_t MaxElements;
	int32_t NumElements;
	int32_t MaxChunks;
	int32_t NumChunks;
};

class FUObjectArray
{
public:
	__int32 ObjFirstGCIndex;
	__int32 ObjLastNonGCIndex;
	__int32 MaxObjectsNotConsideredByGC;
	__int32 OpenForDisregardForGC;
	TUObjectArray ObjObjects;
};