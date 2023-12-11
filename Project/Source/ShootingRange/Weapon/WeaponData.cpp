#include "WeaponData.h"

#include "WeaponAmmunition.h"

#if WITH_EDITORONLY_DATA
void UWeaponData::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	EnsureAssetIntegrity();
}

void UWeaponData::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	EnsureAssetIntegrity();
}
#endif

void UWeaponData::EnsureAssetIntegrity()
{
	//Make sure the default ammo is supported
	SupportedAmmoTypes.AddUnique(DefaultAmmo);

	//Make sure theres no duplicates in the supported ammo array
	TArray<TObjectPtr<UWeaponAmmunitionData>> TempDataArray;

	for (TObjectPtr<UWeaponAmmunitionData> Data : SupportedAmmoTypes)
	{
		TempDataArray.AddUnique(Data);
	}

	SupportedAmmoTypes = TempDataArray;
}

