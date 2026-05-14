// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemManager.generated.h"

USTRUCT(Category="Item|Info")
struct FItemDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int16 Id = 0;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Item Name. Max Text Length is 64"))
	FText Name;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Total Items To Spawn In World"))
	int32 SpawnCount = 0;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int8 WidthInInventory = 0;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int8 HeightInInventory = 0;
	UPROPERTY(EditAnywhere, Category = "Rendering")
	TObjectPtr<UStaticMesh> Mesh = nullptr;
};

USTRUCT()
struct FItemStorage
{
	GENERATED_BODY()

	TSparseArray<int16> Ids;
	TSparseArray<FTransform> Positions;
};

USTRUCT()
struct FSpawnAreaBounds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	double MinX = TNumericLimits<double>::Max();
	UPROPERTY(EditAnywhere)
	double MaxX = TNumericLimits<double>::Lowest();
	UPROPERTY(EditAnywhere)
	double MinY = TNumericLimits<double>::Max();
	UPROPERTY(EditAnywhere)
	double MaxY = TNumericLimits<double>::Lowest();
};

UCLASS()
class LOOTINGSIMULATOR_API AItemManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemManager();
	virtual void OnConstruction(const FTransform& Transform) override;
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
	
#endif
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	FSpawnAreaBounds SpawnArea;

	UPROPERTY(EditAnywhere, Category = "Item|Info")
	TArray<FItemDefinition> ItemDefinitions;

	UPROPERTY()
	TMap<int16, UInstancedStaticMeshComponent*> ItemIsmcMap;

	FItemStorage ItemStorage;
};
