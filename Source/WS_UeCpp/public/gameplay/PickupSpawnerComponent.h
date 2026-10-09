// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PickupSpawnerComponent.generated.h"

class AMainCharacter;
class UPickupComponent;
class UGravityGunComponent;
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
	TWeakObjectPtr<AMainCharacter> Character = nullptr;
	TWeakObjectPtr<APlayerCameraManager> PlayerCameraManager = nullptr;
	TWeakObjectPtr<UGravityGunComponent> GravityGunComponent = nullptr;
	
#pragma region Spawn Pickup
public:
	void SpawnNormalPickup();
	void SpawnThrowPickup();
	void SpawnTakePickup();
	void DebugSpawnerCount();
	
protected:
	// Helpers
	AActor* SpawnAndBindPickup(UClass* PickupClass);
	void CountAndBindPickups(UClass* PickupClass, unsigned int& PickupCount);
	void UnbindPickups(UClass* PickupClass);
	
	void LaunchCooldownTimer();
	void ClearCooldownTimer();
	
	void EndCooldown();
	
protected:
	// Spawn
	UPROPERTY(EditDefaultsOnly, Category="PickupSpawner")
	TSubclassOf<AActor> NormalPickup = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="PickupSpawner")
	TSubclassOf<AActor> ThrowPickup = nullptr;
	UPROPERTY(EditDefaultsOnly, Category="PickupSpawner")
	TSubclassOf<AActor> TakePickup = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="PickupSpawner", meta=(ClampMin = "100.0", ClampMax = "500"))
	float SpawnDistance = 150.0f;
	
	// Count
	UPROPERTY(EditDefaultsOnly, Category="PickupSpawner|Cap", meta=(ClampMin = "0", ClampMax = "200"))
	unsigned int NormalPickupCap = 50.0f;
	UPROPERTY(EditDefaultsOnly, Category="PickupSpawner|Cap", meta=(ClampMin = "0", ClampMax = "200"))
	unsigned int ThrowPickupCap = 50.0f;
	UPROPERTY(EditDefaultsOnly, Category="PickupSpawner|Cap", meta=(ClampMin = "0", ClampMax = "200"))
	unsigned int TakePickupCap = 50.0f;
	UPROPERTY(EditDefaultsOnly, Category="PickupSpawner|Cap", meta=(ClampMin = "0", ClampMax = "500"))
	unsigned int TotalPickupCap = 200.0f;
	
	unsigned int NormalPickupCount = 0;
	unsigned int ThrowPickupCount = 0;
	unsigned int TakePickupCount = 0;
	unsigned int TotalPickupCount = 0;
	
	// Cooldown
	UPROPERTY(EditdefaultsOnly, Category="PickupSpawner", meta=(ClampMin = "0.0", ClampMax = "10.0"))
	float SpawnCooldown = 1.0f;
	
	FTimerHandle CooldownTimerHandle;
	bool bIsOnCooldown = false;
	
# pragma endregion
	
protected:
	UFUNCTION()
	void OnPickupDestroyed(UPickupComponent* PickupComponent);
};
