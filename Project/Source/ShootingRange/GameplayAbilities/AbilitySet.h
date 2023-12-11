#pragma once

#include "Engine/DataAsset.h"

#include "GameplayEffect.h"

#include "AbilitySet.generated.h"

class UProjectGameplayAbility;
class UAttributeSet;

//A struct that bundles abilities, attributes and effects
USTRUCT(BlueprintType)
struct FAbilitySet
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FGameplayAbilitySpecDef> Abilities;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TSubclassOf<UAttributeSet>> Attributes;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FGameplayEffectSpec> Effects;
};


//A data asset wrapper to enable reusability of the same ability set
UCLASS(BlueprintType)
class UAbilitySetData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FAbilitySet AbilitySet;
};