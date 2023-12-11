#include "ProjectGameplayAbility.h"
#include "AbilitySystemComponent.h"


void UProjectGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UProjectGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	// Try to activate if activation policy is on spawn.
	if (ActorInfo && !Spec.IsActive())
	{
		for (UAbilityActivationTrigger* ActivationTrigger : ActivationTriggers)
		{
			if (UAbilityActivationTrigger_OnAbilityGranted* OnAbilityGrantedTrigger = Cast<UAbilityActivationTrigger_OnAbilityGranted>(ActivationTrigger))
			{
				UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
				const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

				// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
				if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
				{
					ASC->TryActivateAbility(Spec.Handle);
				}
				break;
			}
		}
	}
}