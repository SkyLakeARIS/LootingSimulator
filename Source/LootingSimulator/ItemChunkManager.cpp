// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemChunkManager.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "EngineUtils.h"
#include "Engine/StaticMeshActor.h"
#include "Kismet/GameplayStatics.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
// Sets default values
AItemChunkManager::AItemChunkManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	FloorImsc = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FloorISMC"));
}

#if WITH_EDITOR
void AItemChunkManager::BakeFloorsToIsmc()
{
	if(!FloorMesh.Get())
	{
		FNotificationInfo NotificationInfo(NSLOCTEXT("Dev.Preprocess.Chunk", "BakingChunk_Fail_NoMesh", "바닥 Mesh 최적화 실패"));
		NotificationInfo.ExpireDuration = 3.0f;
		NotificationInfo.bFireAndForget = true;
		NotificationInfo.Image = FAppStyle::GetBrush("Icons.Error");
		NotificationInfo.SubText = NSLOCTEXT("Dev.Preprocess.Chunk", "Fail_Reason_NoMesh", "바닥 Mesh를 설정하세요.");
		TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(NotificationInfo);

		GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileFailed_Cue.CompileFailed_Cue"));
		return;
	}

	UWorld* const World = GetWorld();
	if (!World)
	{
		FNotificationInfo NotificationInfo(NSLOCTEXT("Dev.Preprocess.Chunk", "BakingChunk_Fail_NoActor", "바닥 Mesh 최적화 실패"));
		NotificationInfo.ExpireDuration = 3.0f;
		NotificationInfo.bFireAndForget = true;
		NotificationInfo.Image = FAppStyle::GetBrush("Icons.Error");
		NotificationInfo.SubText = NSLOCTEXT("Dev.Preprocess.Chunk", "Fail_Reason_NoActor", "액터가 월드에 배치되었는지 확인하세요.");
		TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(NotificationInfo);

		GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileFailed_Cue.CompileFailed_Cue"));
		return;
	}

	if (GEditor)
	{
		GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileStart_Cue.CompileStart_Cue"));
	}

	Modify();

	if(FloorImsc.Get())
	{
		FloorImsc->ClearInstances();
	}
	else
	{
		FloorImsc = NewObject<UInstancedStaticMeshComponent>(this);
		FloorImsc->RegisterComponent();
		FloorImsc->SetStaticMesh(FloorMesh);
	}

	bool bSuccess = true;
	FText NotifyStateText;
	SNotificationItem::ECompletionState CompletionState = SNotificationItem::CS_Success;
	FString SoundFilePath;

	// TODO: 메인스레드 블로킹으로 단계별 팝업을 위해서는 비동기 방식 검토가 필요.
	FNotificationInfo NotificationInfo(NSLOCTEXT("Dev.Preprocess.Chunk", "BakingChunk_Process_Start", "바닥 Mesh 최적화 시작"));
	NotificationInfo.ExpireDuration = 3.0f;
	NotificationInfo.Image = FAppStyle::GetBrush("Icons.Info");
	NotificationInfo.SubText = NSLOCTEXT("Dev.Preprocess.Chunk", "BakingChunk_Message_Start", "수행 중...");
	TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(NotificationInfo);


	FSpawnAreaBounds Extent={ TNumericLimits<double>::Max(), TNumericLimits<double>::Lowest(), TNumericLimits<double>::Max(), TNumericLimits<double>::Lowest() };
	const FName FloorTag("Floor");
	FVector Origin {};
	FVector BoxExtent {};
	FVector RefBoxExtent {};
	FTransform Transform{};
	bool bReferenceSet = false;
	for (AStaticMeshActor* Actor : TActorRange<AStaticMeshActor>(World))
	{
		if(Actor->ActorHasTag(FloorTag))
		{
			Actor->GetActorBounds(false,Origin, BoxExtent);

			if(!bReferenceSet)
			{
				RefBoxExtent = BoxExtent;
				bReferenceSet = true;
			}

			if(abs(RefBoxExtent.X - BoxExtent.X) > DBL_EPSILON || abs(RefBoxExtent.Y - BoxExtent.Y) > DBL_EPSILON)
			{
				NotifyStateText = NSLOCTEXT("Dev.Preprocess.Chunk", "Fail_Reason_MeshExtentNotUniform", "바닥의 Scale들은 모두 같아야 합니다.");
				CompletionState = SNotificationItem::CS_Fail;
				SoundFilePath = TEXT("/Engine/EditorSounds/Notifications/CompileFailed_Cue.CompileFailed_Cue");
				bSuccess = false;
				break;
			}

			Extent.MinX = FMath::Min<double>(Extent.MinX, Origin.X - BoxExtent.X);
			Extent.MaxX = FMath::Max<double>(Extent.MaxX, Origin.X + BoxExtent.X);

			Extent.MinY = FMath::Min<double>(Extent.MinY, Origin.Y - BoxExtent.Y);
			Extent.MaxY = FMath::Max<double>(Extent.MaxY, Origin.Y + BoxExtent.Y);

			Transform = Actor->GetActorTransform();
			FloorImsc->AddInstance(Transform);
		}
	}

	Extent.MinX += RefBoxExtent.X;
	Extent.MaxX += RefBoxExtent.X;
	Extent.MinY += RefBoxExtent.Y;
	Extent.MaxY += RefBoxExtent.Y;
	constexpr double Tolerance = 0.001;
	if(Extent.MinX < -Tolerance || Extent.MinY < -Tolerance)
	{
		NotifyStateText = NSLOCTEXT("Dev.Preprocess.Chunk", "Fail_Reason_FloorExistOutOfMap", "바닥 Mesh는 +X축, +Y축에 존재해야 합니다. (x >= 0, y >= 0)");
		CompletionState = SNotificationItem::CS_Fail;
		SoundFilePath = TEXT("/Engine/EditorSounds/Notifications/CompileFailed_Cue.CompileFailed_Cue");
		bSuccess = false;
	}

	if(bSuccess)
	{
		const int16 LengthX = static_cast<int16>(RefBoxExtent.X * 2.0);
		const int16 LengthY = static_cast<int16>(RefBoxExtent.Y * 2.0);

		ChunkCountX = static_cast<int16>(Extent.MaxX - Extent.MinX) / LengthX;
		ChunkCountY = static_cast<int16>(Extent.MaxY - Extent.MinY) / LengthY;

		ChunkLengthX = LengthX;
		ChunkLengthY = LengthY;

		FloorExtent = Extent;

		NotifyStateText = NSLOCTEXT("Dev.Preprocess.Chunk", "BakingChunk_Message_Success", "작업 완료");
		CompletionState = SNotificationItem::CS_Success;
		SoundFilePath = TEXT("/Engine/EditorSounds/Notifications/CompileSuccess_Cue.CompileSuccess_Cue");
	}

	if (Notification.IsValid())
	{
		Notification->SetSubText(NotifyStateText);
		Notification->SetCompletionState(CompletionState);
	}

	if (GEditor)
	{
		GEditor->PlayEditorSound(SoundFilePath);
	}
}

void AItemChunkManager::AddItem(int32 IndexInStorage, FTransform Transform)
{
	const FVector Position = Transform.GetLocation();
	const int32 ChunkIndex = static_cast<int32>(Position.Y) / ChunkLengthY * ChunkCountX + static_cast<int32>(Position.X) / ChunkLengthX;
	Chunks[ChunkIndex].InsertItem(IndexInStorage, Position);
}
#endif

// Called when the game starts or when spawned
void AItemChunkManager::BeginPlay()
{
	Super::BeginPlay();
	const int32 TotalChunkCount = ChunkCountX * ChunkCountY;

	Chunks.Reserve(TotalChunkCount);

	const FVector2d ChunkOrigin(FloorExtent.MinX, FloorExtent.MinY);
	const int16 Width = ChunkLengthX;
	const int16 Height = ChunkLengthY;
	const int16 ChunkCountEachLine = ChunkCountX;
	for (int32 Chunk = 0; Chunk < TotalChunkCount; ++Chunk)
	{
		FVector2d LeftBottom = { ChunkOrigin.X + Width * (Chunk % ChunkCountEachLine), ChunkOrigin.Y + Height * (Chunk / ChunkCountEachLine) };
		// TODO: LeftBottom에 의존성이 생길 것 같은데 나중에 개선 포인트가 될지 체크 필요.
		FVector2d RightTop = { ChunkOrigin.X + Width, ChunkOrigin.Y + Height};
		enum { TreeDepth = 2 };
		Chunks.Add(FQuadTree(TreeDepth, LeftBottom, RightTop));
	}

	FloorImsc->SetStaticMesh(FloorMesh);
}

// Called every frame
void AItemChunkManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

