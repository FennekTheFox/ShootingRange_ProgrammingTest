#pragma once

#include "CoreMinimal.h"

#include "Abilities/GameplayAbility.h"
#include "ProjectGameplayAbility.generated.h"

class UInputAction;

UCLASS(BlueprintType, Abstract, EditInlineNew, DefaultToInstanced, CollapseCategories)
class UAbilityActivationTrigger : public UObject
{
	GENERATED_BODY()

public:

};

// Attempts to activate the ability once right after it has been granted.
UCLASS(BlueprintType, DisplayName = "On Ability Granted")
class UAbilityActivationTrigger_OnAbilityGranted : public UAbilityActivationTrigger
{
	GENERATED_BODY()
};

// Attempts to activate the ability each time its assigned Input Action triggers.
UCLASS(BlueprintType, DisplayName = "On Input Triggered")
class UAbilityActivationTrigger_OnInputTriggered : public UAbilityActivationTrigger
{
	GENERATED_BODY()

public:

	// For how long the ability will keep trying to activate after it couldn't immediately be activated on input started.
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float InputBufferDuration = 0.f;
};

// Attempts to activate the ability each time the specified Gameplay Tag gets added to its Owner.
UCLASS(BlueprintType, DisplayName = "On Tag Added")
class UAbilityActivationTrigger_OnTagAdded : public UAbilityActivationTrigger
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag GameplayTag;
};

// Attempts to activate the ability each time the specified Gameplay Tag gets added to its Owner, cancels it when the Tag gets removed.
UCLASS(BlueprintType, DisplayName = "While Tag Present")
class UAbilityActivationTrigger_WhileTagPresent : public UAbilityActivationTrigger
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag GameplayTag;
};

// Attempts to activate the ability every tick while the specified Gameplay Tag is present on its Owner, cancels it when the Tag gets removed.
UCLASS(BlueprintType, DisplayName = "While Tag Present (Ongoing)")
class UAbilityActivationTrigger_WhileTagPresentOngoing : public UAbilityActivationTrigger
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag GameplayTag;
};

// Attempts to activate the ability each time a Gameplay Event with the specified Gameplay Tag happens.
UCLASS(BlueprintType, DisplayName = "On Gameplay Event")
class UAbilityActivationTrigger_OnGameplayEvent : public UAbilityActivationTrigger
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FGameplayTag GameplayTag;
};

//The project specific extension of the Gameplay Ability
UCLASS(BlueprintType, Blueprintable)
class UProjectGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public: 
	// Defines when this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Ability Activation")
	TArray<TObjectPtr<UAbilityActivationTrigger>> ActivationTriggers;

	//The input action this ability is associated with
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<UInputAction> InputAction;
public:
	void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;
};

