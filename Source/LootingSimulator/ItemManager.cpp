// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemManager.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Math/RandomStream.h"
// Sets default values
AItemManager::AItemManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AItemManager::OnConstruction(const FTransform& Transform)
{

}

// Called when the game starts or when spawned
void AItemManager::BeginPlay()
{
	Super::BeginPlay();
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
			ItemIsmcMap[Mesh]->AddInstance(FTransform(FVector(PosX, PosY, 0.0)));
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

