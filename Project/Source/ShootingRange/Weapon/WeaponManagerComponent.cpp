#include "WeaponManagerComponent.h"

#include "WeaponInstance.h"
#include "WeaponData.h"
#include "WeaponModifier.h"
#include "GameplayAbilities/ProjectAbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"

UWeaponManagerComponent::UWeaponManagerComponent()
{
	EnsureCapacity();
}

bool UWeaponManagerComponent::AddWeaponAtIndex(UWeaponData* WeaponData, int32 Index)
{
	if (!ensure(Index < Capacity && Index >= 0))
	{
		return false;
	}

	RemoveWeaponAtIndex(Index);

	if (!(WeaponData))
	{
		return true;
	}

	if (!ensure(WeaponData->WeaponInstanceClass))
	{
		return false;
	}

	AddedWeapons[Index] = WeaponData;

	//If data is valid instance needs to also be valid
	WeaponInstances[Index] = NewObject<UWeaponInstance>(this, WeaponData->WeaponInstanceClass), Index;
	WeaponInstances[Index]->Initialize();
	WeaponInstances[Index]->OnInstanceModified.AddDynamic(this, &UWeaponManagerComponent::OnWeaponInstanceModified);

	//If its the current weapon, clear the proxy
	// TODO link anim layer
	if (Index == ActiveIndex)
	{
		//This "resets" the active weapon index
		SetActiveIndex(ActiveIndex);
	}

	return true;
}

void UWeaponManagerComponent::RemoveWeaponAtIndex(int32 Index)
{
	if (!ensure(Index < Capacity && Index >= 0))
	{
		return;
	}

	if (Index == ActiveIndex)
	{
		CleanActiveWeapon();
	}

	AddedWeapons[Index] = nullptr;
	WeaponInstances[Index] = nullptr;

}

void UWeaponManagerComponent::SetActiveIndex(int32 NewActiveIndex)
{
	CleanActiveWeapon();

	ActiveIndex = NewActiveIndex;

	//check if valid data has been selected
	UWeaponData* WeaponData = AddedWeapons[ActiveIndex];
	UWeaponInstance* WeaponInstance = WeaponInstances[ActiveIndex];
	if (!(WeaponData && WeaponInstance))
	{
		return;
	}

	//Spawn the weapon proxy & attach it
	CurrentWeaponProxy = GetWorld()->SpawnActor<AActor>(WeaponData->WeaponProxyActor, FTransform::Identity);
	USkeletalMeshComponent* SKMC = GetOwner()->GetComponentByClass<USkeletalMeshComponent>();
	CurrentWeaponProxy->AttachToComponent(
		SKMC,
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		WeaponData->ProxyAttachSocket);

	//Spawn and attach the mod proxies
	USkeletalMeshComponent* ProxySKMC = CurrentWeaponProxy->GetComponentByClass<USkeletalMeshComponent>();
	for (auto&& KVPair : WeaponInstance->Modifiers)
	{
		const UWeaponModifierInstance* ModInstance = KVPair.Value;
		if (!(ModInstance && ModInstance->DataRef && ModInstance->DataRef->ModifierProxyActorClass))
		{
			continue;
		}

		//Spawn the weapon proxy & attach it
		AActor* NewModProxy = GetWorld()->SpawnActor<AActor>(ModInstance->DataRef->ModifierProxyActorClass, FTransform::Identity);
		NewModProxy->AttachToComponent(
			ProxySKMC,
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			KVPair.Value->DataRef->ProxyAttachSocket);

		ModifierProxies.Add(NewModProxy);
	}

	//add the ability set stuff
	UProjectAbilitySystemComponent* PASC = GetOwner()->FindComponentByClass<UProjectAbilitySystemComponent>();

	if (ensure(PASC))
	{
		for (auto&& KVPair : WeaponData->WeaponAbilitySets)
		{
			if (KVPair.Value)
			{
				PASC->AddAbilitySet(KVPair.Value->AbilitySet, KVPair.Key);
			}
		}
	}

	//Add the input mapping
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (APlayerController* AsPC = Cast<APlayerController>(Pawn->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(AsPC->GetLocalPlayer()))
			{
				FModifyContextOptions Options;
				Subsystem->AddMappingContext(WeaponData->WeaponInputs, 0, Options);
			}
		}
	}
}

void UWeaponManagerComponent::CleanActiveWeapon()
{
	UWeaponData* WeaponData = AddedWeapons[ActiveIndex];
	UWeaponInstance* WeaponInstance = WeaponInstances[ActiveIndex];

	if (!(WeaponData && WeaponInstance))
	{
		return;
	}

	//Destroy the weapon proxy
	if (CurrentWeaponProxy)
	{
		CurrentWeaponProxy->Destroy();
		CurrentWeaponProxy = nullptr;
	}

	//Destroy the mod proxies
	for (AActor* ModProxy : ModifierProxies)
	{
		if (!ModProxy)
		{
			continue;
		}

		ModProxy->Destroy();
	}
	ModifierProxies.Reset();


	//Remove the ability set stuff
	UProjectAbilitySystemComponent* PASC = GetOwner()->FindComponentByClass<UProjectAbilitySystemComponent>();

	if (ensure(PASC))
	{
		for (auto&& KVPair : WeaponData->WeaponAbilitySets)
		{
			if (KVPair.Value)
			{
				PASC->RemoveAbilitySet(KVPair.Value->AbilitySet, KVPair.Key);
			}
		}
	}

	//Remove the Input Mapping Context
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (APlayerController* AsPC = Cast<APlayerController>(Pawn->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(AsPC->GetLocalPlayer()))
			{
				FModifyContextOptions Options;
				Subsystem->RemoveMappingContext(WeaponData->WeaponInputs);
			}
		}
	}
}


void UWeaponManagerComponent::CycleNext(bool bCanWrap /*= true*/, bool bCanSelectEmpty /*= false*/)
{
	int32 NewIndex = PredictNewIndex(true, bCanWrap, bCanSelectEmpty);

	if (NewIndex != ActiveIndex)
	{
		SetActiveIndex(NewIndex);
	}
}

void UWeaponManagerComponent::CyclePrevious(bool bCanWrap /*= true*/, bool bCanSelectEmpty /*= false*/)
{
	int32 NewIndex = PredictNewIndex(false, bCanWrap, bCanSelectEmpty);

	if (NewIndex != ActiveIndex)
	{
		SetActiveIndex(NewIndex);
	}
}

int32 UWeaponManagerComponent::PredictNewIndex(bool bForward, bool bCanWrap /*= true*/, bool bCanSelectEmpty /*= false*/)
{
	for (int i = 1; i < Capacity; i++)
	{
		int32 PotentialNewIndex = (bForward ? (ActiveIndex + i) : (ActiveIndex - i));

		if (!bCanWrap)
		{
			PotentialNewIndex =  (bForward ? (ActiveIndex + i) : (ActiveIndex - i));

			if (PotentialNewIndex < 0 || PotentialNewIndex >= Capacity)
			{
				//We dont wrap and we ran out of bounds
				return ActiveIndex;
			}

			if (WeaponInstances[PotentialNewIndex] || bCanSelectEmpty)
			{
				return PotentialNewIndex;
			}
		}

		//Clamp and wrap the index
		PotentialNewIndex = (PotentialNewIndex + Capacity) % Capacity;

		if (WeaponInstances[PotentialNewIndex] || bCanSelectEmpty)
		{
			return PotentialNewIndex;
		}
	}

	return ActiveIndex;
}


UWeaponData* UWeaponManagerComponent::GetActiveWeaponData()
{
	return AddedWeapons[ActiveIndex];
}

UWeaponInstance* UWeaponManagerComponent::GetActiveWeaponInstance()
{
	return WeaponInstances[ActiveIndex];
}

AActor* UWeaponManagerComponent::GetActiveWeaponProxy()
{
	return CurrentWeaponProxy;
}

TArray<UWeaponData*> UWeaponManagerComponent::GetAllWeaponData()
{
	return AddedWeapons;
}

TArray<UWeaponInstance*> UWeaponManagerComponent::GetAllWeaponInstances()
{
	return WeaponInstances;
}

#if WITH_EDITORONLY_DATA
void UWeaponManagerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	EnsureCapacity();
}

void UWeaponManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < Capacity; i++)
	{
		UWeaponData* Data = DefaultWeapons[i];
		AddWeaponAtIndex(Data, i);
	}
}

#endif

void UWeaponManagerComponent::OnWeaponInstanceModified(UWeaponInstance* Instance)
{
	//This refreshes the current weapon
	SetActiveIndex(ActiveIndex);
}

void UWeaponManagerComponent::EnsureCapacity()
{
	DefaultWeapons.SetNum(Capacity);
	AddedWeapons.SetNum(Capacity);
	WeaponInstances.SetNum(Capacity);
}
