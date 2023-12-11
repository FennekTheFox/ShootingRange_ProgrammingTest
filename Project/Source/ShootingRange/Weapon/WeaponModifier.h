#pragma once

#include "CoreMinimal.h"

#include "Engine/DataAsset.h"

#include "WeaponModifier.generated.h"


class UWeaponInstance;

/**
 * The Data class for weapon modifiers
 */
UCLASS(BlueprintType, Blueprintable)
class UWeaponModifierData : public UDataAsset
{
	GENERATED_BODY()

public:
	//The proxy actor used for this mod
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<AActor> ModifierProxyActorClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ProxyAttachSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UWeaponModifierInstance> ModInstanceClass;
};

UCLASS(BlueprintType, Blueprintable)
class UWeaponModifierInstance : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UWeaponModifierData> DataRef;

	UFUNCTION(BlueprintNativeEvent)
	void ApplyToInstance(UWeaponInstance* Instance);
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveFromInstance();

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UWeaponInstance> AppliedInstance;
};
