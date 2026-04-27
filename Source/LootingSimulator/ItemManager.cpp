// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemManager.h"
#include "ItemChunkManager.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Math/RandomStream.h"
#include "EngineUtils.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Misc/DataValidation.h"
#include "Widgets/Notifications/SNotificationList.h"

// Sets default values
AItemManager::AItemManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AItemManager::OnConstruction(const FTransform& Transform)
{

}

#if WITH_EDITOR
EDataValidationResult AItemManager::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);
	const int32 ItemCount = ItemList.Num();
	// check if the item id is unique.
	TSet<int16> SeenIds;
	SeenIds.Reserve(ItemCount);
	bool bSuccess = true;
	for (int32 Item = 0; Item < ItemCount; ++Item)
	{
		const int16 CurId = ItemList[Item].Id;
		if(SeenIds.Find(CurId))
		{
			Context.AddError(FText::Format(NSLOCTEXT("Editor.Validator.Item", "Invalid_Reason_DuplicateId", "중복 사용된 ID가 있습니다. 각 ID는 고유해야 합니다. Actor Name: {0}, ID: {1}, Index: {2}"), FText::FromString(GetName()), FText::AsNumber(CurId), FText::AsNumber(Item)));
			bSuccess = false;
		}
		else
		{
			SeenIds.Add(CurId);
		}

		if(CurId <= 0)
		{
			Context.AddError(FText::Format(NSLOCTEXT("Editor.Validator.Item", "Invalid_Reason_NegativeOrZeroId", "ID는 1보다 커야 합니다. Actor Name: {0}, Index: {1}"), FText::FromString(GetName()), FText::AsNumber(Item)));
			bSuccess = false;
		}

		if (ItemList[Item].Name.IsEmptyOrWhitespace())
		{
			Context.AddError(FText::Format(NSLOCTEXT("Editor.Validator.Item", "Invalid_Reason_EmptyName", "이름이 공백이거나 올바르지 않습니다. Actor Name: {0}, Index: {1}"), FText::FromString(GetName()), FText::AsNumber(Item)));
			bSuccess = false;
		}

		if (ItemList[Item].SpawnCount <= 0)
		{
			Context.AddError(FText::Format(NSLOCTEXT("Editor.Validator.Item", "Invalid_Reason_NegativeOrZeroSpawnCount", "SpawnCount는 1보다 커야 합니다. Actor Name: {0}, Index: {1}"), FText::FromString(GetName()), FText::AsNumber(Item)));
			bSuccess = false;
		}

		if (ItemList[Item].WidthInInventory <= 0)
		{
			Context.AddError(FText::Format(NSLOCTEXT("Editor.Validator.Item", "Invalid_Reason_NegativeOrZeroWidth", "WidthInInventory는 1보다 커야 합니다.(1~127) Actor Name: {0}, Index: {1}"), FText::FromString(GetName()), FText::AsNumber(Item)));
			bSuccess = false;
		}

		if (ItemList[Item].HeightInInventory <= 0)
		{
			Context.AddError(FText::Format(NSLOCTEXT("Editor.Validator.Item", "Invalid_Reason_NegativeOrZeroHeight", "HeightInInventory는 1보다 커야 합니다.(1~127) Actor Name: {0}, Index: {1}"), FText::FromString(GetName()), FText::AsNumber(Item)));
			bSuccess = false;
		}

		if(!ItemList[Item].Mesh)
		{
			Context.AddError(FText::Format(NSLOCTEXT("Editor.Validator.Item", "Invalid_Reason_NoMesh", "Mesh가 설정되지 않은 아이템이 존재합니다. Actor Name: {0}, Index: {1}"), FText::FromString(GetName()), FText::AsNumber(Item)));
			bSuccess = false;
		}
	}

	if (!bSuccess)
	{
		Result = EDataValidationResult::Invalid;

		FNotificationInfo NotificationInfo(NSLOCTEXT("Editor.Validator.Item", "Validator_Fail_InvalidDataFound", "ItemManager에 비정상 데이터 발견"));
		NotificationInfo.ExpireDuration = 10.0f;
		NotificationInfo.bFireAndForget = true;
		NotificationInfo.Image = FAppStyle::GetBrush("Icons.Error");
		NotificationInfo.SubText = NSLOCTEXT("Editor.Validator.Item", "Validator_Message_Guide", "에디터 내 Output Log 메세지를 확인하여 발견된 문제들을 수정하세요.");
		TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(NotificationInfo);

		GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileFailed_Cue.CompileFailed_Cue"));
	}

	return Result;
}
#endif

// Called when the game starts or when spawned
void AItemManager::BeginPlay()
{
	Super::BeginPlay();
	UWorld* const World = GetWorld();
	AItemChunkManager* ChunkManager = nullptr;
	if(World)
	{
		for (AItemChunkManager* Manager : TActorRange<AItemChunkManager>(World))
		{
			SpawnArea = Manager->FloorExtent;
			ChunkManager= Manager;
		}
	}

	for (FItemTypeData& Item : ItemList)
	{
		if (!Item.Mesh)
		{
			UE_LOG(LogTemp, Error, TEXT("mesh is not set."));
		}
	}

	int32 MeshCount = ItemList.Num();
	for (int32_t IsmcIndex = 0; IsmcIndex < MeshCount; ++IsmcIndex)
	{
		ItemIsmcMap.Add(IsmcIndex, NewObject<UInstancedStaticMeshComponent>(this));
		ItemIsmcMap[IsmcIndex]->RegisterComponent();
		ItemIsmcMap[IsmcIndex]->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
		ItemIsmcMap[IsmcIndex]->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		ItemIsmcMap[IsmcIndex]->SetStaticMesh(ItemList[IsmcIndex].Mesh);
	}

	constexpr int32_t FixedSeed = 123456;
	FRandomStream RandomStream(FixedSeed);

	for (int32_t Mesh = 0; Mesh < MeshCount; ++Mesh)
	{
		const int32 SpawnCount = ItemList[Mesh].SpawnCount;
		const int32 CurrentItemId = ItemList[Mesh].Id;
		for (int Item = 0; Item < SpawnCount; ++Item)
		{
			const double PosX = RandomStream.FRandRange(SpawnArea.MinX, SpawnArea.MaxX);
			const double PosY = RandomStream.FRandRange(SpawnArea.MinY, SpawnArea.MaxY);
			const FTransform Transform = FTransform(FVector(PosX, PosY, 0.0));
			ItemIsmcMap[Mesh]->AddInstance(Transform);

			// configuring Item Storage
			ItemStorage.Ids.Add(CurrentItemId);
			ItemStorage.Positions.Add(Transform);
			const int32 IndexInStorage = ItemStorage.Ids.Num() - 1;
			ChunkManager->AddItem(IndexInStorage, Transform);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("Item ISMC was set."));
}

void AItemManager::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ItemIsmcMap.Empty();
}

// Called every frame
void AItemManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

