#include "ProjectPlayerController.h"
#include "GameplayAbilities/ProjectAbilitySystemComponent.h"

void AProjectPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (UProjectAbilitySystemComponent* PASC = GetProjectAbilitySystemComponent())
	{
		PASC->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

UProjectAbilitySystemComponent* AProjectPlayerController::GetProjectAbilitySystemComponent()
{
	return GetPawn() ? GetPawn()->FindComponentByClass<UProjectAbilitySystemComponent>() : nullptr;
}

