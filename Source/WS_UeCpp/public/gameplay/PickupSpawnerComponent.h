// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickupSpawnerComponent.generated.h"

class AMainCharacter;
class UPickupComponent;
class APlayerCameraManager;

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class WS_UECPP_API UPickupSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPickupSpawnerComponent();
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
protected:
	TWeakObjectPtr<APlayerCameraManager> PlayerCameraManager = nullptr;
	
#pragma region Spawn Pickup
public:
	void SpawnNormalPickup();
	void SpawnThrowPickup();
	void SpawnTakePickup();
	
protected:
	AActor* SpawnPickup(UClass* PickupClass);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="PickupSpawner")
	TSubclassOf<AActor> NormalPickup = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="PickupSpawner")
	TSubclassOf<AActor> ThrowPickup = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="PickupSpawner")
	TSubclassOf<AActor> TakePickup = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="PickupSpawner", meta=(ClampMin = "100.0", ClampMax = "500.0"))
	float SpawnDistance = 150.0f;
	
# pragma endregion
	
protected:
	UFUNCTION()
	void OnPickupDestroyed(UPickupComponent* PickupComponent);
};
