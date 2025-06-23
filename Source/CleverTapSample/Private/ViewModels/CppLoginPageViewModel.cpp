#include "CppLoginPageViewModel.h"

#include "CleverTapConfig.h"
#include "CppDemonstrationHUD.h"
#include "ViewModels/CppUserProfileViewModel.h"

UCppLoginPageViewModel::UCppLoginPageViewModel(const FObjectInitializer& ObjectInitializer)
	: Super{ ObjectInitializer }
{
	UserProfile = CreateDefaultSubobject<UCppUserProfileViewModel>("User Profile");
	UserProfile->OnViewModelChanged().AddDynamic(this, &UCppLoginPageViewModel::PropogateViewModelChanged);
}

UCppUserProfileViewModel* UCppLoginPageViewModel::GetUserProfile() const
{
	return UserProfile;
}

void UCppLoginPageViewModel::Edit(TFunctionRef<void(MutableContext&, UCppLoginPageViewModel&)> EditFn)
{
	Super::Edit<UCppLoginPageViewModel>(EditFn);
}

UCppLoginPageViewModel& UCppLoginPageViewModel::SetIdentity(MutableContext&, const FString& Value)
{
	Identity = Value;
	return *this;
}

UCppLoginPageViewModel& UCppLoginPageViewModel::SetCustomCleverTapId(MutableContext&, const FString& Value)
{
	CustomCleverTapId = Value;
	return *this;
}

TScriptInterface<IUserProfileViewModelInterface> UCppLoginPageViewModel::GetUserProfile_Implementation() const
{
	return TScriptInterface<IUserProfileViewModelInterface>{ UserProfile };
}

FString UCppLoginPageViewModel::GetIdentity_Implementation() const
{
	return Identity;
}

void UCppLoginPageViewModel::SetIdentity_Implementation(const FString& Value)
{
	Edit([&](MutableContext& Ctx, UCppLoginPageViewModel& VM) { VM.SetIdentity(Ctx, Value); });
}

FString UCppLoginPageViewModel::GetCustomCleverTapId_Implementation() const
{
	return CustomCleverTapId;
}

void UCppLoginPageViewModel::SetCustomCleverTapId_Implementation(const FString& Value)
{
	Edit([&](MutableContext& Ctx, UCppLoginPageViewModel& VM) { VM.SetCustomCleverTapId(Ctx, Value); });
}

bool UCppLoginPageViewModel::IsCustomCleverTapIdVisible_Implementation() const
{
	const auto* const DefaultConfig = UCleverTapConfig::StaticClass()->GetDefaultObject<UCleverTapConfig>();
	check(DefaultConfig != nullptr);
	return DefaultConfig->bUseCustomCleverTapId;
}

bool UCppLoginPageViewModel::IsLoginEnabled_Implementation() const
{
	const auto* const DefaultConfig = UCleverTapConfig::StaticClass()->GetDefaultObject<UCleverTapConfig>();
	check(DefaultConfig != nullptr);
	if (DefaultConfig->bUseCustomCleverTapId && FStringView{ CustomCleverTapId }.TrimStartAndEnd().IsEmpty())
	{
		return false;
	}

	return true;
}

void UCppLoginPageViewModel::Login_Implementation()
{
	// Walk up the outer chain until we find our HUD
	UObject* CurrentOuter = GetOuter();
	while (CurrentOuter != nullptr && !CurrentOuter->IsA<ACppDemonstrationHUD>())
	{
		CurrentOuter = CurrentOuter->GetOuter();
	}
	if (CurrentOuter != nullptr)
	{
		auto* const HUD = Cast<ACppDemonstrationHUD>(CurrentOuter);
		check(HUD != nullptr);

		HUD->Login(UserProfile->GetProfileName(), UserProfile->GetEmail(), UserProfile->GetPhone(), Identity,
			CustomCleverTapId);
	}
}
