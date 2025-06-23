#include "CppUserProfileViewModel.h"

#include "CleverTapInstance.h"
#include "CleverTapSample.h"
#include "CleverTapSubsystem.h"
#include "Engine.h"

const FString& UCppUserProfileViewModel::GetCleverTapId() const
{
	return CleverTapId;
}

const FString& UCppUserProfileViewModel::GetProfileName() const
{
	return Name;
}

const FString& UCppUserProfileViewModel::GetEmail() const
{
	return Email;
}

const FString& UCppUserProfileViewModel::GetPhone() const
{
	return Phone;
}

TArrayView<FProfilePropertyViewModel const> UCppUserProfileViewModel::GetProfileProperties() const
{
	return Properties;
}

void UCppUserProfileViewModel::Edit(TFunctionRef<void(MutableContext&, UCppUserProfileViewModel&)> EditFn)
{
	Super::Edit<UCppUserProfileViewModel>(EditFn);
}

UCppUserProfileViewModel& UCppUserProfileViewModel::SetCleverTapId(MutableContext&, const FString& Value)
{
	CleverTapId = Value;
	return *this;
}

UCppUserProfileViewModel& UCppUserProfileViewModel::SetProfileName(MutableContext&, const FString& Value)
{
	Name = Value;
	return *this;
}

UCppUserProfileViewModel& UCppUserProfileViewModel::SetEmail(MutableContext&, const FString& Value)
{
	Email = Value;
	return *this;
}

UCppUserProfileViewModel& UCppUserProfileViewModel::SetPhone(MutableContext&, const FString& Value)
{
	Phone = Value;
	return *this;
}

UCppUserProfileViewModel& UCppUserProfileViewModel::SetProfileProperties(
	MutableContext&, const TArray<FProfilePropertyViewModel>& Value)
{
	Properties = Value;
	return *this;
}

void UCppUserProfileViewModel::Tick(float DeltaTime)
{
	auto* const CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
	if (CleverTapSys == nullptr || !CleverTapSys->IsSharedInstanceInitialized())
	{
		return;
	}

	auto& CleverTapInst = CleverTapSys->SharedInstance();
	FString CurrentCleverTapId = CleverTapInst.GetCleverTapId();
	if (CurrentCleverTapId != CleverTapId)
	{
		Edit([&](MutableContext& Ctx, UCppUserProfileViewModel& VM) { VM.CleverTapId = MoveTemp(CurrentCleverTapId); });
	}
}

TStatId UCppUserProfileViewModel::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UCppUserProfileViewModel, STATGROUP_Tickables);
}

FString UCppUserProfileViewModel::GetCleverTapId_Implementation() const
{
	return GetCleverTapId();
}

FString UCppUserProfileViewModel::GetProfileName_Implementation() const
{
	return GetProfileName();
}

void UCppUserProfileViewModel::SetProfileName_Implementation(const FString& Value)
{
	Edit([&](MutableContext& Ctx, UCppUserProfileViewModel& VM) { VM.SetProfileName(Ctx, Value); });
}

FString UCppUserProfileViewModel::GetEmail_Implementation() const
{
	return GetEmail();
}

void UCppUserProfileViewModel::SetEmail_Implementation(const FString& Value)
{
	Edit([&](MutableContext& Ctx, UCppUserProfileViewModel& VM) { VM.SetEmail(Ctx, Value); });
}

FString UCppUserProfileViewModel::GetPhone_Implementation() const
{
	return GetPhone();
}

void UCppUserProfileViewModel::SetPhone_Implementation(const FString& Value)
{
	Edit([&](MutableContext& Ctx, UCppUserProfileViewModel& VM) { VM.SetPhone(Ctx, Value); });
}

TArray<FProfilePropertyViewModel> UCppUserProfileViewModel::GetProfileProperties_Implementation()
{
	return Properties;
}

void UCppUserProfileViewModel::AddProfileProperty_Implementation(FProfilePropertyViewModel Value)
{
	Edit([&](MutableContext& Ctx, UCppUserProfileViewModel& VM) { VM.Properties.Add(MoveTemp(Value)); });
}

void UCppUserProfileViewModel::SetProfilePropertyAt_Implementation(int32 Index, FProfilePropertyViewModel Value)
{
	if (!Properties.IsValidIndex(Index))
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("SetProfilePropertyAt() - Invalid property index: %d"), Index);
		return;
	}

	Edit([&](MutableContext& Ctx, UCppUserProfileViewModel& VM) { VM.Properties[Index] = MoveTemp(Value); });
}

void UCppUserProfileViewModel::RemoveProfilePropertyAt_Implementation(int32 Index)
{
	if (!Properties.IsValidIndex(Index))
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("RemoveProfilePropertyAt() - Invalid property index: %d"), Index);
		return;
	}

	Edit([=](MutableContext& Ctx, UCppUserProfileViewModel& VM) { VM.Properties.RemoveAt(Index); });
}
