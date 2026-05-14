

#pragma once

#include "CoreMinimal.h"
#include "QuadTree.generated.h"

USTRUCT()
struct FQuadTreeNode
{
	GENERATED_BODY()

	int32 IndexInStorage;
	FVector3d Position;
};

/**
 *
 */
class LOOTINGSIMULATOR_API FQuadTree
{
public:
	enum EQuadDirection
	{
		LeftBottom,
		LeftTop,
		RightBottom,
		RightTop,
		DirectionCount
	};
public:
	FQuadTree(int8 MaxDepth, FVector2d LeftBottom, FVector2d RightTop);
	~FQuadTree();

	void InsertItem(int32 IndexInStorage, FVector3d Position);

	int32 GetIndexByPosition(FVector3d& Point);

private:
	FQuadTree(int8 MaxDepth, int8 Depth, FVector2d LeftBottom, FVector2d RightTop);

private:
	int8 MaxDepth = 1;
	int8 Depth = 1;
	// MEMO: 0-LeftBottom / 1-RightTop
	FVector2d Bounds[2] = {};
	FQuadTree* SubTrees[EQuadDirection::DirectionCount] = {};
	TArray<FQuadTreeNode> List;
};
