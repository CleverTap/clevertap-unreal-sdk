#include "WorldObject.h"

UWorld* UWorldObject::GetWorld() const
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return nullptr;
	}

	auto* const Outer = GetOuter();
	if (Outer == nullptr)
	{
		return nullptr;
	}

	if (Outer->HasAnyFlags(RF_BeginDestroyed))
	{
		return nullptr;
	}

	if (Outer->IsUnreachable())
	{
		return nullptr;
	}

	return Outer->GetWorld();
}

void UWorldObject::PostInitProperties()
{
	Super::PostInitProperties();

	Initialize();
}
