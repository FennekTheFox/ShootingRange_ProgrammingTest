#include "WeaponInstance.h"

#include "WeaponData.h"
#include "WeaponModifier.h"
#include "WeaponAmmunition.h"

void UWeaponInstance::Initialize()
{
	if (!ensure(DataRef))
	{
		return;
	}

	if (ensure(DataRef->DefaultAmmo->InstanceClass))
	{
		SelectedAmmoType = NewObject<UWeaponAmmunitionInstance>(this, DataRef->DefaultAmmo->InstanceClass);
	}

	for (const FWeaponModSocket& ModSocket : DataRef->ModSockets)
	{
		if (!(ModSocket.Default && ModSocket.Default->ModInstanceClass))
		{
			Modifiers.Add(ModSocket.SocketID, nullptr);
			continue;
		}

		UWeaponModifierInstance* NewModInstance = NewObject<UWeaponModifierInstance>(this, ModSocket.Default->ModInstanceClass);
		Modifiers.Add(ModSocket.SocketID, NewModInstance);
		NewModInstance->ApplyToInstance(this);
	}

}

UWeaponAmmunitionInstance* UWeaponInstance::GetAmmunitionType()
{
	return SelectedAmmoType;
}

void UWeaponInstance::SetAmmoType(UWeaponAmmunitionData* NewAmmoType)
{
	if (ensure(NewAmmoType))
	{
		SelectedAmmoType = NewObject<UWeaponAmmunitionInstance>(this, NewAmmoType->InstanceClass);
	}
}

void UWeaponInstance::SetModifier(FGameplayTag SocketID, UWeaponModifierData* Modifier)
{
	if (!ensure(Modifiers.Contains(SocketID)))
	{
		return;
	}

	//Remove the old modifier
	UWeaponModifierInstance* OldInstance = *Modifiers.Find(SocketID);
	if (OldInstance)
	{
		OldInstance->RemoveFromInstance();
	}


	//Add the new modifier
	if (Modifier)
	{
		UWeaponModifierInstance* NewModInstance = NewObject<UWeaponModifierInstance>(this, Modifier->ModInstanceClass);
		Modifiers.Add(SocketID, NewModInstance);
		NewModInstance->ApplyToInstance(this);
	}
	else
	{
		Modifiers.Add(SocketID, nullptr);
	}

	OnInstanceModified.Broadcast(this);
}
