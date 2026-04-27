// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemManager.h"
#include "QuadTree.h"
#include "ItemChunkManager.generated.h"

UCLASS()

class LOOTINGSIMULATOR_API AItemChunkManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemChunkManager();

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category="Chunk|Preprocessing", meta = (DisplayName = "(Optimize) Bake Floors"))
	void BakeFloorsToIsmc();
#endif

	void AddItem(int32 IndexInStorage, FTransform Transform);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(EditAnywhere, Category="Chunk|Mesh")
	TObjectPtr<UStaticMesh> FloorMesh = nullptr;

	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> FloorImsc = nullptr;

	UPROPERTY()
	FSpawnAreaBounds FloorExtent;
	UPROPERTY()
	int16 ChunkCountX = 0;
	UPROPERTY()
	int16 ChunkCountY = 0;
	UPROPERTY()
	int16 ChunkLengthX = 0;
	UPROPERTY()
	int16 ChunkLengthY = 0;

	TArray<FQuadTree> Chunks;
};
