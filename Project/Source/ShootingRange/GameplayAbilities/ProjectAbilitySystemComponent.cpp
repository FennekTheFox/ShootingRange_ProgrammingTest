#include "ProjectAbilitySystemComponent.h"

#include "AbilitySet.h"
#include "ProjectGameplayAbility.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"

void UProjectAbilitySystemComponent::AddAbilitySet(const FAbilitySet& AbilitySet, FGameplayTag ContextKey)
{
	//To ensure proper management the same context key cannot be used twice
	if (ContextData.Contains(ContextKey))
	{
		return;
	}

	FAbilitySetApplicationData& NewData = ContextData.FindOrAdd(ContextKey);

	//Give the abilities
	for (const FGameplayAbilitySpecDef& AbilitySpecDef : AbilitySet.Abilities)
	{
		FGameplayAbilitySpec AbilitySpec(AbilitySpecDef.Ability);
		NewData.GivenAbilities.Add(GiveAbility(AbilitySpec));
	}

	//Give the attribute sets
	//NOTE no checks if the attribute set was already added, dangerous
	for (TSubclassOf<UAttributeSet> AttributeSetClass : AbilitySet.Attributes)
	{
		UAttributeSet* NewSet = NewObject<UAttributeSet>(GetTransientPackage(), AttributeSetClass);
		NewData.SpawnedAttributes.Add(NewSet);
		AddSpawnedAttribute(NewSet);
	}

	//Apply the effects
	for (const FGameplayEffectSpec Spec : AbilitySet.Effects)
	{
		NewData.AppliedEffects.Add(ApplyGameplayEffectSpecToSelf(Spec));
	}
}

void UProjectAbilitySystemComponent::RemoveAbilitySet(const FAbilitySet& AbilitySet, FGameplayTag ContextKey)
{
	//Ensure the context key has been added to begin with
	if (!ContextData.Contains(ContextKey))
	{
		return;
	}

	FAbilitySetApplicationData* Data = ContextData.Find(ContextKey);

	if (!ensure(Data))
	{
		return;
	}

	//Clear the abilities
	for (FGameplayAbilitySpecHandle& AbilityHandle : Data->GivenAbilities)
	{
		ClearAbility(AbilityHandle);
	}

	//Give the attribute sets
	for (TSubclassOf<UAttributeSet> AttributeSetClass : AbilitySet.Attributes)
	{
		TArray<UAttributeSet*>& AttributeSets = GetSpawnedAttributes_Mutable();

		for (UAttributeSet* AttributeSet : AttributeSets)
		{
			if (Data->SpawnedAttributes.Contains(AttributeSet))
			{
				RemoveSpawnedAttribute(AttributeSet);
			}
		}
	}

	//Apply the effects
	for (FActiveGameplayEffectHandle& EffectHandle : Data->AppliedEffects)
	{
		RemoveActiveGameplayEffect(EffectHandle);
	}

	ContextData.Remove(ContextKey);
}

void UProjectAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	if (APawn* Pawn = Cast<APawn>(GetAvatarActor()))
	{
		if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
		{
			BindAbilityInputs(AbilitySpec, PC);
		}
	}
}

void UProjectAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);
}

void UProjectAbilitySystemComponent::BindAbilityInputs(FGameplayAbilitySpec& Spec, APlayerController* PC)
{
	UProjectGameplayAbility* Ability = Cast<UProjectGameplayAbility>(Spec.Ability);

	if (!Ability)
	{
		return;
	}

	if (Ability->InputAction)
	{
		if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			FInputActionAbilityBindings* AbilityBindings = AbilityBindingsPerInputAction.Find(Ability->InputAction);
			if (!AbilityBindings)
			{
				AbilityBindings = &(AbilityBindingsPerInputAction.Add(Ability->InputAction));
				AbilityBindings->BindingTriggered = EnhancedInputComp->BindAction(Ability->InputAction.Get(), ETriggerEvent::Triggered, this, &ThisClass::AbilityInputActionTriggered, Ability->InputAction.Get()).GetHandle();
				AbilityBindings->BindingCompleted = EnhancedInputComp->BindAction(Ability->InputAction.Get(), ETriggerEvent::Completed, this, &ThisClass::AbilityInputActionCancelledOrCompleted, Ability->InputAction.Get()).GetHandle();
				AbilityBindings->BindingCancelled = EnhancedInputComp->BindAction(Ability->InputAction.Get(), ETriggerEvent::Canceled, this, &ThisClass::AbilityInputActionCancelledOrCompleted, Ability->InputAction.Get()).GetHandle();
			}
			AbilityBindings->Abilities.Add(Spec.Handle);
		}
	}
}

void UProjectAbilitySystemComponent::UnbindAbilityInputs(FGameplayAbilitySpec& Spec, APlayerController* PC)
{
	UProjectGameplayAbility* Ability = Cast<UProjectGameplayAbility>(Spec.Ability);

	if (!Ability)
	{
		return;
	}

	if (Ability->InputAction)
	{
		if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PC->InputComponent))
		{
			FInputActionAbilityBindings* AbilityBindings = AbilityBindingsPerInputAction.Find(Ability->InputAction);
			if (AbilityBindings)
			{
				AbilityBindings->Abilities.Remove(Spec.Handle);
				if (AbilityBindings->Abilities.Num() == 0)
				{
					EnhancedInputComp->RemoveBindingByHandle(AbilityBindings->BindingTriggered);
					EnhancedInputComp->RemoveBindingByHandle(AbilityBindings->BindingCompleted);
					EnhancedInputComp->RemoveBindingByHandle(AbilityBindings->BindingCancelled);
					AbilityBindingsPerInputAction.Remove(Ability->InputAction);
				}
			}
		}
	}
}

void UProjectAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;

	// Process all abilities with buffered activation inputs
	for (auto It = BufferedAbilityActivations.CreateIterator(); It; ++It)
	{
		if (It->Value > GetWorld()->GetRealTimeSeconds())
		{
			AbilitiesToActivate.AddUnique(It->Key);
		}
		else
		{
			It.RemoveCurrent();
		}
	}

	// Process all abilities that had their input action triggered this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputTriggeredSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true; // InputPressed to us means "input was triggered and not completed/cancelled since"

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);

					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, SpecHandle, AbilitySpec->ActivationInfo.GetActivationPredictionKey());
				}
				else
				{
					if (const UProjectGameplayAbility* AbilityCDO = CastChecked<UProjectGameplayAbility>(AbilitySpec->Ability))
					{
						for (UAbilityActivationTrigger* ActivationTrigger : AbilityCDO->ActivationTriggers)
						{
							if (UAbilityActivationTrigger_OnInputTriggered* OnInputTriggered = Cast<UAbilityActivationTrigger_OnInputTriggered>(ActivationTrigger))
							{
								AbilitiesToActivate.AddUnique(AbilitySpec->Handle);

								if (OnInputTriggered->InputBufferDuration > 0.f)
								{
									BufferedAbilityActivations.Add(SpecHandle, GetWorld()->GetRealTimeSeconds() + OnInputTriggered->InputBufferDuration);
								}

								break;
							}
						}
					}
				}
			}
		}
	}

	// Try to activate all the abilities that are from (buffered) input triggers.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send a input event to the ability because of the press.
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		const bool bBegunToActivateSuccessfully = TryActivateAbility(AbilitySpecHandle);
		if (bBegunToActivateSuccessfully)
		{
			BufferedAbilityActivations.Remove(AbilitySpecHandle);
		}
	}

	// Process all abilities that had their input action completed this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputCompletedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false; // InputPressed to us means "input was triggered and not completed/cancelled since"

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);

					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, SpecHandle, AbilitySpec->ActivationInfo.GetActivationPredictionKey());
				}
			}
		}
	}

	// Clear the cached ability handles.
	ClearAbilityInput();
}

void UProjectAbilitySystemComponent::AbilityInputActionTriggered(const FInputActionInstance& InputActionInstance, UInputAction* InputAction)
{
	if (InputAction)
	{
		FInputActionAbilityBindings* AbilityBindings = AbilityBindingsPerInputAction.Find(InputAction);
		if (AbilityBindings)
		{
			for (FGameplayAbilitySpecHandle Handle : AbilityBindings->Abilities)
			{
				InputTriggeredSpecHandles.AddUnique(Handle);
			}
		}
	}
}

void UProjectAbilitySystemComponent::AbilityInputActionCancelledOrCompleted(const FInputActionInstance& InputActionInstance, UInputAction* InputAction)
{
	if (InputAction)
	{
		FInputActionAbilityBindings* AbilityBindings = AbilityBindingsPerInputAction.Find(InputAction);
		if (AbilityBindings)
		{
			for (FGameplayAbilitySpecHandle Handle : AbilityBindings->Abilities)
			{
				InputCompletedSpecHandles.AddUnique(Handle);
			}
		}
	}
}

void UProjectAbilitySystemComponent::ClearAbilityInput()
{
	InputTriggeredSpecHandles.Reset();
	InputCompletedSpecHandles.Reset();
}

void UProjectAbilitySystemComponent::OnPawnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	if (APlayerController* OldPlayerController = Cast<APlayerController>(OldController))
	{
		for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			UnbindAbilityInputs(Spec, OldPlayerController);
		}
	}

	if (APlayerController* NewPlayerController = Cast<APlayerController>(NewController))
	{
		for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
		{
			BindAbilityInputs(Spec, NewPlayerController);
		}
	}
}

void UProjectAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (const UProjectGameplayAbility* AbilityCDO = CastChecked<UProjectGameplayAbility>(AbilitySpec.Ability))
		{
			AbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
		}
	}
}

void UProjectAbilitySystemComponent::AddDefaults(AActor* InOwnerActor, AActor* InAvatarActor)
{
	for (auto&& KVPair : DefaultAbilitySets)
	{
		if (!ensure(KVPair.Value))
		{
			continue;
		}

		AddAbilitySet(KVPair.Value->AbilitySet, KVPair.Key);
	}
}

void UProjectAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	if (!ensure(ActorInfo) || !ensure(InOwnerActor))
	{
		return;
	}

	const bool bAvatarChanged = (InAvatarActor != AbilityActorInfo->AvatarActor);

	if (bAvatarChanged)
	{
		// remove callbacks from previous avatar
		if (APawn* Pawn = Cast<APawn>(AbilityActorInfo->AvatarActor))
		{
			Pawn->ReceiveControllerChangedDelegate.RemoveDynamic(this, &UProjectAbilitySystemComponent::OnPawnControllerChanged);
		}
	}

	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	if (bAvatarChanged)
	{
		AddDefaults(InOwnerActor, InAvatarActor);

		// add callbacks to new avatar
		if (APawn* Pawn = Cast<APawn>(GetAvatarActor()))
		{
			Pawn->ReceiveControllerChangedDelegate.AddDynamic(this, &UProjectAbilitySystemComponent::OnPawnControllerChanged);
		}

		TryActivateAbilitiesOnSpawn();
	}
}
