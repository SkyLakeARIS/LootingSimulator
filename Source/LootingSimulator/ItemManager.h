// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemManager.generated.h"

USTRUCT(Category="Item|Info")
struct FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int16 ID;
	UPROPERTY(EditAnywhere)
	int8 WidthInInventory;
	UPROPERTY(EditAnywhere)
	int8 HeightInInventory;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMesh> Mesh;
};

USTRUCT()
struct FItemStorage
{
	GENERATED_BODY()

	TSparseArray<int16> IDs;
	TSparseArray<FVector3f> Positions;
};

USTRUCT()
struct FSpawnAreaBounds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	double MinX;
	UPROPERTY(EditAnywhere)
	double MaxX;
	UPROPERTY(EditAnywhere)
	double MinY;
	UPROPERTY(EditAnywhere)
	double MaxY;
};

UCLASS()
class LOOTINGSIMULATOR_API AItemManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemManager();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(EditAnywhere, Category = "Item|Spawn")
	FSpawnAreaBounds SpawnArea;

	UPROPERTY(EditAnywhere, Category = "Item|Info")
	TArray<FItemData> ItemList;

	UPROPERTY()
	TMap<int16, UInstancedStaticMeshComponent*> ItemIsmcMap;

	FItemStorage ItemStorage;
};
