#pragma once

#include "CoreMinimal.h"

#include "ProjectPlayerController.generated.h"

class UProjectAbilitySystemComponent;

UCLASS(BlueprintType, Blueprintable)
class AProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	UProjectAbilitySystemComponent* GetProjectAbilitySystemComponent();

};