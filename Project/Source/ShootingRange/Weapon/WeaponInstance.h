#pragma once

#include "CoreMinimal.h"

#include "GameplayTagContainer.h"

#include "WeaponInstance.generated.h"

class UWeaponModifierInstance;
class UWeaponAmmunitionInstance;
class UWeaponData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponInstanceModified, UWeaponInstance*, Instance);

/**
*	The weapon instance object holds information about the current weapon instance
*/
UCLASS(BlueprintType, Blueprintable)
class UWeaponInstance : public UObject 
{	
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TObjectPtr<UWeaponData> DataRef;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<UWeaponAmmunitionInstance> SelectedAmmoType;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TMap<FGameplayTag, TObjectPtr<UWeaponModifierInstance>> Modifiers;

	void Initialize();
	UWeaponAmmunitionInstance* GetAmmunitionType();

	UFUNCTION(BlueprintCallable)
	void SetAmmoType(UWeaponAmmunitionData* NewAmmoType);

	UFUNCTION(BlueprintCallable)
	void SetModifier(FGameplayTag SocketID, UWeaponModifierData* Modifier);

	UPROPERTY(BlueprintAssignable)
	FWeaponInstanceModified OnInstanceModified;
};

