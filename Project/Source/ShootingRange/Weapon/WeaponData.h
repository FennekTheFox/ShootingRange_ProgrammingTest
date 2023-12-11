#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"
#include "GameplayAbilities/AbilitySet.h"
#include "WeaponAmmunition.h"

#include "WeaponData.generated.h"

class UWeaponInstance;
class UInputMappingContext;
class UWeaponModifierData;
class UWeaponAmmunitionInstance;

USTRUCT(BlueprintType)
struct FWeaponModSocket
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag SocketID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText SocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UWeaponModifierData> Default;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UWeaponModifierData>> SupportedMods;
};


UCLASS(BlueprintType)
class UWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	//The weapon identifier
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag WeaponID;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UTexture2D> Icon;

	//The ability sets this weapon gives
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, TObjectPtr<UAbilitySetData>> WeaponAbilitySets;

	//The Instance class used to track runtime data
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UWeaponInstance> WeaponInstanceClass;

	//The Actor class that will be used as a proxy
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<AActor> WeaponProxyActor;

	//The attach socket for the proxy actor
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ProxyAttachSocket;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UWeaponAmmunitionData> DefaultAmmo;

	// The ammo types this weapon supports
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TObjectPtr<UWeaponAmmunitionData>> SupportedAmmoTypes;

	// The modifier sockets this weapon supports
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<FWeaponModSocket> ModSockets;

	// The IMC specifically used for this weapons inputs
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> WeaponInputs;

#if WITH_EDITORONLY_DATA
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
	void EnsureAssetIntegrity();
};