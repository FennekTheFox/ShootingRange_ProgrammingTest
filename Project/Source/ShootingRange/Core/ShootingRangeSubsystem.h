#pragma once

#include "CoreMinimal.h"

#include "ShootingRangeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FScoreChangedDelegate);


UCLASS(BlueprintType)
class UShootingRangeSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	int32 GetScore();

	UFUNCTION(BlueprintCallable)
	void IncreaseScore(int32 Amount);

	UPROPERTY(BlueprintAssignable)
	FScoreChangedDelegate OnScoreChanged;

private:
	UPROPERTY()
	int32 Score;
};