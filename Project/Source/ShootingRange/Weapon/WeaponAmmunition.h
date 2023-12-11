#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "WeaponAmmunition.generated.h"

class UWeaponData;
class UWeaponInstance;
class APlayerController;

USTRUCT(BlueprintType)
struct FAmmoFireData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	FTransform FireTransform = FTransform::Identity;

	UPROPERTY(BlueprintReadWrite)
	float Speed = 0.f;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UWeaponInstance> WeaponInstance;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController;
};


UCLASS(BlueprintType, Blueprintable)
class UWeaponAmmunitionData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UWeaponAmmunitionInstance> InstanceClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;

};

UCLASS(BlueprintType, Blueprintable)
class UWeaponAmmunitionInstance : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UWeaponAmmunitionData> DataRef;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnFire(UPARAM(ref)const FAmmoFireData& Data);
	void OnFire_Implementation(const FAmmoFireData& Datat){};

	UWorld* GetWorld() const override
	{
		return Super::GetWorld();
	}


	bool ImplementsGetWorld() const override
	{
		return true;
	}
};