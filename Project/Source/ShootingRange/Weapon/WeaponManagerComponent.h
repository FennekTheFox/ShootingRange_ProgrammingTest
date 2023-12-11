#pragma once

#include "CoreMinimal.h"

#include "WeaponManagerComponent.generated.h"


class UWeaponData;
class UWeaponInstance;

/**
 * A component that manages the equipping (and unequipping) of weapons. It is handed the
 * weapon data asset, and creates the corresponding weapon instance. It also manages the 
 * currently selected weapon
 */
UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class UWeaponManagerComponent : public UActorComponent 
{
	GENERATED_BODY()

public:
	UWeaponManagerComponent();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ClampMin = 1))
	int32 Capacity = 4;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (ClampMin = 1))
	TArray<TObjectPtr<UWeaponData>> DefaultWeapons;

	//Sets the weapon at the index
	UFUNCTION(BlueprintCallable)
	bool AddWeaponAtIndex(UWeaponData* WeaponData, int32 Index);
	//Removes the weapon at the index
	UFUNCTION(BlueprintCallable)
	void RemoveWeaponAtIndex(int32 Index);

	//Updates the proxy, abilities and inputs for the new active weapon
	UFUNCTION(BlueprintCallable)
	void SetActiveIndex(int32 NewActiveIndex);
	//Cycles to the next index
	UFUNCTION(BlueprintCallable)
	void CycleNext(bool bCanWrap = true, bool bCanSelectEmpty = false);
	//Cycles to the previous index
	UFUNCTION(BlueprintCallable)
	void CyclePrevious(bool bCanWrap = true, bool bCanSelectEmpty = false);
	//Utility function used to predict the resulting index of cycling
	UFUNCTION(BlueprintPure)
	int32 PredictNewIndex(bool bForward, bool bCanWrap = true, bool bCanSelectEmpty = false);

	//Returns the active weapon's data object
	UFUNCTION(BlueprintPure)
	UWeaponData* GetActiveWeaponData();
	//Returns the active weapon's instance object
	UFUNCTION(BlueprintPure)
	UWeaponInstance* GetActiveWeaponInstance();
	//Returns the active weapon's proxy actor
	UFUNCTION(BlueprintPure)
	AActor* GetActiveWeaponProxy();

	UFUNCTION(BlueprintPure)
	TArray<UWeaponData*> GetAllWeaponData();
	UFUNCTION(BlueprintPure)
	TArray<UWeaponInstance*> GetAllWeaponInstances();

#if WITH_EDITORONLY_DATA
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void BeginPlay() override;

private:
	UFUNCTION()
	void OnWeaponInstanceModified(UWeaponInstance* Instance);

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UWeaponData>> AddedWeapons;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UWeaponInstance>> WeaponInstances;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> CurrentWeaponProxy;

	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<AActor>> ModifierProxies;

	UPROPERTY(VisibleAnywhere)
	int32 ActiveIndex = 0;

	void EnsureCapacity();
	void CleanActiveWeapon();
};