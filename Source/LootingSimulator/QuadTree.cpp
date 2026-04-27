


#include "QuadTree.h"

FQuadTree::FQuadTree(int8 MaxDepth, FVector2d LeftBottom, FVector2d RightTop)
	: MaxDepth(MaxDepth)
	, Depth(1)
	, Bounds{ LeftBottom , RightTop }
	, SubTrees{}
{
	const int8 NextDepth = Depth + 1;
	if (NextDepth < MaxDepth)
	{
		const FVector2d HalfPoint = (LeftBottom + RightTop) * 0.5;
		SubTrees[EQuadDirection::LeftBottom] = new FQuadTree(MaxDepth, NextDepth, LeftBottom, HalfPoint);

		SubTrees[EQuadDirection::LeftTop] = new FQuadTree(MaxDepth, NextDepth, FVector2d(LeftBottom.X, HalfPoint.Y), FVector2d(HalfPoint.X, RightTop.Y));
		SubTrees[EQuadDirection::RightBottom] = new FQuadTree(MaxDepth, NextDepth, FVector2d(HalfPoint.X, LeftBottom.Y), FVector2d(RightTop.X, HalfPoint.Y));
		SubTrees[EQuadDirection::RightTop] = new FQuadTree(MaxDepth, NextDepth, HalfPoint, RightTop);
	}

	List.Reserve(64);
}

FQuadTree::~FQuadTree()
{
	delete SubTrees[EQuadDirection::LeftBottom];
	SubTrees[EQuadDirection::LeftBottom] = nullptr;

	delete SubTrees[EQuadDirection::LeftTop];
	SubTrees[EQuadDirection::LeftTop] = nullptr;

	delete SubTrees[EQuadDirection::RightTop];
	SubTrees[EQuadDirection::RightTop] = nullptr;

	delete SubTrees[EQuadDirection::RightBottom];
	SubTrees[EQuadDirection::RightBottom] = nullptr;
}

void FQuadTree::InsertItem(int32 IndexInStorage, FVector3d Position)
{
	// TODO: (고민)브랜치리스로 해볼 수 있을지도. (eDirection을 L/R 로, B/T를 offset으로하면 단순한 삼항체크로 인덱스 계산 가능할 것 같다)
	// TODO: 매개변수 부분도 최적화 할만한 부분이 있을지도
	if (SubTrees[EQuadDirection::LeftBottom] && SubTrees[EQuadDirection::LeftTop] && SubTrees[EQuadDirection::RightBottom] && SubTrees[EQuadDirection::RightTop])
	{
		const FVector2d HalfPoint = (Bounds[0] + Bounds[1]) * 0.5;
		if (Position.X < HalfPoint.X)
		{
			// left
			if (Position.Y < HalfPoint.Y)
			{
				//bottom
				SubTrees[EQuadDirection::LeftBottom]->InsertItem(IndexInStorage, Position);
			}
			else
			{
				// top
				SubTrees[EQuadDirection::LeftTop]->InsertItem(IndexInStorage, Position);
			}
		}
		else
		{
			// right
			if (Position.Y < HalfPoint.Y)
			{
				//bottom
				SubTrees[EQuadDirection::RightBottom]->InsertItem(IndexInStorage, Position);
			}
			else
			{
				// top
				SubTrees[EQuadDirection::RightTop]->InsertItem(IndexInStorage, Position);
			}
		}
	}
	else
	{
		List.Emplace(FQuadTreeNode(IndexInStorage, Position));
	}
}

int32 FQuadTree::GetIndexByPosition(FVector3d& Point)
{
	int32_t IndexInStorage = -1;
	if (SubTrees[EQuadDirection::LeftBottom] && SubTrees[EQuadDirection::LeftTop] && SubTrees[EQuadDirection::RightBottom] && SubTrees[EQuadDirection::RightTop])
	{
		const FVector2d HalfPoint = (Bounds[0] + Bounds[1]) * 0.5;
		if (Point.X < HalfPoint.X)
		{
			// left
			if (Point.Y < HalfPoint.Y)
			{
				//bottom
				SubTrees[EQuadDirection::LeftBottom]->GetIndexByPosition(Point);
			}
			else
			{
				// top
				SubTrees[EQuadDirection::LeftTop]->GetIndexByPosition(Point);
			}
		}
		else
		{
			// right
			if (Point.Y < HalfPoint.Y)
			{
				//bottom
				SubTrees[EQuadDirection::RightBottom]->GetIndexByPosition(Point);
			}
			else
			{
				// top
				SubTrees[EQuadDirection::RightTop]->GetIndexByPosition(Point);
			}
		}
	}
	else
	{
		double MinDist = TNumericLimits<double>::Max();
		FVector3d Position = Point;
		for(FQuadTreeNode& Node : List)
		{
			const double Dist = FVector3d::Distance(Position, Node.Position);
			const bool bMinDist = Dist < MinDist;
			MinDist = bMinDist ? Dist : MinDist;
			IndexInStorage = bMinDist ? Node.IndexInStorage : IndexInStorage;
		}
	}
	return IndexInStorage;
}

FQuadTree::FQuadTree(int8 MaxDepth, int8 Depth, FVector2d LeftBottom, FVector2d RightTop)
	: MaxDepth(MaxDepth)
	, Depth(Depth)
	, Bounds{ LeftBottom, RightTop }
	, SubTrees{}
{
	if(Depth < MaxDepth)
	{
		const int8 NextDepth = Depth + 1;
		const FVector2d HalfPoint = (LeftBottom + RightTop) * 0.5;
		SubTrees[EQuadDirection::LeftBottom] = new FQuadTree(MaxDepth, NextDepth, LeftBottom, HalfPoint);
		SubTrees[EQuadDirection::LeftTop] = new FQuadTree(MaxDepth, NextDepth, FVector2d(LeftBottom.X, HalfPoint.Y), FVector2d(HalfPoint.X, RightTop.Y));
		SubTrees[EQuadDirection::RightBottom] = new FQuadTree(MaxDepth, NextDepth, FVector2d(HalfPoint.X, LeftBottom.Y), FVector2d(RightTop.X, HalfPoint.Y));
		SubTrees[EQuadDirection::RightTop] = new FQuadTree(MaxDepth, NextDepth, HalfPoint, RightTop);
	}
}
