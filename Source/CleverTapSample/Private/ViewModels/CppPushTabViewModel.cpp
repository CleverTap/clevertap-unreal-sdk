// Copyright CleverTap All Rights Reserved.
#include "ViewModels/CppPushTabViewModel.h"

#include "CleverTapSubsystem.h"
#include "CleverTapInstance.h"
#include "CleverTapSample.h"
#include "CppMainMenuViewModel.h"
#include "Engine.h"

void UCppPushTabViewModel::Edit(TFunctionRef<void(MutableContext&, UCppPushTabViewModel&)> EditFn)
{
	Super::Edit<UCppPushTabViewModel>(EditFn);
}

UCppPushTabViewModel& UCppPushTabViewModel::SetPushNotificationData(MutableContext&, const FString& Value)
{
	PushNotificationData = Value;
	return *this;
}

UCppPushTabViewModel& UCppPushTabViewModel::SetPushNotificationPermissionStatus(
	MutableContext&, ECleverTapPushPermissionStatus Value)
{
	PushNotificationPermissionStatus = Value;
	return *this;
}

UCppPushTabViewModel& UCppPushTabViewModel::SetDeepLinkData(MutableContext&, const FString& Value)
{
	DeepLinkData = Value;
	return *this;
}

UCppPushTabViewModel& UCppPushTabViewModel::SetInAppShownData(MutableContext&, const FString& Value)
{
	InAppShownData = Value;
	return *this;
}

UCppPushTabViewModel& UCppPushTabViewModel::SetInAppDismissedData(MutableContext&, const FString& Value)
{
	InAppDismissedData = Value;
	return *this;
}

UCppPushTabViewModel& UCppPushTabViewModel::SetInAppDismissedActionData(MutableContext&, const FString& Value)
{
	InAppDismissedActionData = Value;
	return *this;
}

UCppPushTabViewModel& UCppPushTabViewModel::SetInAppButtonData(MutableContext&, const FString& Value)
{
	InAppButtonData = Value;
	return *this;
}

void UCppPushTabViewModel::BeginDestroy()
{
	if (GetWorld())
	{
		auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
		CleverTapInst.OnPushPermissionResponse.RemoveAll(this);
		CleverTapInst.OnPushNotificationClicked.RemoveAll(this);
		CleverTapInst.OnOpenUrl.RemoveAll(this);
		CleverTapInst.OnInAppNotificationShown.RemoveAll(this);
		CleverTapInst.OnInAppNotificationDismissed.RemoveAll(this);
		CleverTapInst.OnInAppNotificationButtonClicked.RemoveAll(this);
	}

	Super::BeginDestroy();
}

void UCppPushTabViewModel::PostInitProperties()
{
	Super::PostInitProperties();

	if (GetWorld())
	{
		auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
		CleverTapInst.OnPushPermissionResponse.AddDynamic(this, &UCppPushTabViewModel::OnPushPermissionResponse);
		CleverTapInst.OnPushNotificationClicked.AddDynamic(this, &UCppPushTabViewModel::OnPushNotificationClicked);
		CleverTapInst.OnOpenUrl.AddDynamic(this, &UCppPushTabViewModel::OnOpenURL);
		CleverTapInst.OnInAppNotificationShown.AddDynamic(this, &UCppPushTabViewModel::OnInAppNotificationShown);
		CleverTapInst.OnInAppNotificationDismissed.AddDynamic(
			this, &UCppPushTabViewModel::OnInAppNotificationDismissed);
		CleverTapInst.OnInAppNotificationButtonClicked.AddDynamic(
			this, &UCppPushTabViewModel::OnInAppNotificationButtonClicked);
	}
}

FString UCppPushTabViewModel::GetPushNotificationData_Implementation() const
{
	return PushNotificationData;
}

void UCppPushTabViewModel::ClearPushNotificationData_Implementation()
{
	Edit([](MutableContext& Ctx, UCppPushTabViewModel& VM) { VM.SetPushNotificationData(Ctx, FString{}); });
}

FString UCppPushTabViewModel::GetPushNotificationPermissionStatus_Implementation() const
{
	const FText StatusArg = [this]() {
		switch (PushNotificationPermissionStatus)
		{
			case ECleverTapPushPermissionStatus::Unknown:
			{
				return NSLOCTEXT("CleverTapSample", "PushPermissionGranted_Unknown", "UNKNOWN");
			}
			case ECleverTapPushPermissionStatus::Granted:
			{
				return NSLOCTEXT("CleverTapSample", "PushPermissionGranted_True", "TRUE");
			}
			case ECleverTapPushPermissionStatus::NotGranted:
			{
				return NSLOCTEXT("CleverTapSample", "PushPermissionGranted_False", "FALSE");
			}

			default:
			{
				UE_LOG(LogCleverTapSample, Error, TEXT("Unknown ECleverTapPushPermissionStatus value: %d"),
					static_cast<uint8>(PushNotificationPermissionStatus));
				return FText{};
			}
		}
	}();

	return FText::Format(
		NSLOCTEXT("CleverTapSample", "SamplePushTabPushPermGrantedText", "Push Permission Granted: {Granted}"),
		FFormatNamedArguments{ { "Granted", StatusArg } })
		.ToString();
}

void UCppPushTabViewModel::PromptForPushPermission_Implementation(EPushPermissionPrimerType PrimerType)
{
	auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();

	switch (PrimerType)
	{
		case EPushPermissionPrimerType::None:
		{
			CleverTapInst.PromptForPushPermission(/*bFallbackToSettings=*/true);
		}
		break;

		case EPushPermissionPrimerType::Alert:
		{
			FCleverTapPushPrimerAlertConfig PrimerConfig;
			PrimerConfig.TitleText = NSLOCTEXT("CleverTapSample", "PushPrimerAlertTitle", "Alert Title Text");
			PrimerConfig.MessageText = NSLOCTEXT("CleverTapSample", "PushPrimerAlertMessage", "Alert Message Text");
			PrimerConfig.PositiveButtonText = NSLOCTEXT("CleverTapSample", "PushPrimerAlertPositiveButton", "Positive");
			PrimerConfig.NegativeButtonText = NSLOCTEXT("CleverTapSample", "PushPrimerAlertNegativeButton", "Negative");
			PrimerConfig.bFallbackToSettings = true;
			CleverTapInst.PromptForPushPermission(PrimerConfig);
		}
		break;

		case EPushPermissionPrimerType::HalfInterstitial:
		{
			FCleverTapPushPrimerHalfInterstitialConfig PrimerConfig;
			PrimerConfig.TitleText =
				NSLOCTEXT("CleverTapSample", "PushPrimerHalfInterstitialTitle", "Push Primer Title Text");
			PrimerConfig.MessageText = NSLOCTEXT(
				"CleverTapSample", "HalfInterstitialPushPrimerMessage", "Half-Interstitial Push Primer Message Text");
			PrimerConfig.PositiveButtonText =
				NSLOCTEXT("CleverTapSample", "HalfInterstitialPushPrimerPositiveButton", "Positive");
			PrimerConfig.NegativeButtonText =
				NSLOCTEXT("CleverTapSample", "HalfInterstitialPushPrimerNegativeButton", "Negative");
			PrimerConfig.bFallbackToSettings = true;

			PrimerConfig.ImageURL = TEXT("https://icons.iconarchive.com/icons/treetog/junior/64/camera-icon.png");
			PrimerConfig.BackgroundColor = FColor::White;
			PrimerConfig.TitleTextColor = FColor::Blue;
			PrimerConfig.MessageTextColor = FColor::Red;
			PrimerConfig.ButtonBorderColor = FColor::Blue;
			PrimerConfig.ButtonTextColor = FColor::Black;
			PrimerConfig.ButtonBackgroundColor = FColor::Silver;
			PrimerConfig.ButtonBorderRadius = TEXT("10");

			CleverTapInst.PromptForPushPermission(PrimerConfig);
		}
		break;
	}
}

FString UCppPushTabViewModel::GetDeepLinkData_Implementation() const
{
	return DeepLinkData;
}

void UCppPushTabViewModel::ClearDeepLinkData_Implementation()
{
	Edit([](MutableContext& Ctx, UCppPushTabViewModel& VM) { VM.SetDeepLinkData(Ctx, FString{}); });
}

FString UCppPushTabViewModel::GetInAppShownData_Implementation() const
{
	return InAppShownData;
}

FString UCppPushTabViewModel::GetInAppDismissedData_Implementation() const
{
	return InAppDismissedData;
}

FString UCppPushTabViewModel::GetInAppDismissedActionData_Implementation() const
{
	return InAppDismissedActionData;
}

FString UCppPushTabViewModel::GetInAppButtonData_Implementation() const
{
	return InAppButtonData;
}

void UCppPushTabViewModel::ClearInAppData_Implementation()
{
	Edit([](MutableContext& Ctx, UCppPushTabViewModel& VM) {
		VM.SetInAppShownData(Ctx, FString{})
			.SetInAppDismissedData(Ctx, FString{})
			.SetInAppDismissedActionData(Ctx, FString{})
			.SetInAppButtonData(Ctx, FString{});
	});
}

void UCppPushTabViewModel::OnPushPermissionResponse(bool bGranted)
{
	Edit([=](MutableContext& Ctx, UCppPushTabViewModel& VM) {
		VM.PushNotificationPermissionStatus =
			bGranted ? ECleverTapPushPermissionStatus::Granted : ECleverTapPushPermissionStatus::NotGranted;
	});
}

void UCppPushTabViewModel::OnPushNotificationClicked(const FCleverTapProperties& NotificationPayload)
{
	UCppMainMenuViewModel* const MainMenu = Cast<UCppMainMenuViewModel>(GetOuter());
	check(MainMenu != nullptr);

	MainMenu->Edit([&, this](MutableContext& Ctx, UCppMainMenuViewModel& MainMenuVM) {
		this->PushNotificationData = ToDebugString(NotificationPayload);
		MainMenuVM.SetActiveTab(Ctx, EMainMenuTab::Push);
	});
}

void UCppPushTabViewModel::OnOpenURL(const FString& Url)
{
	UCppMainMenuViewModel* const MainMenu = Cast<UCppMainMenuViewModel>(GetOuter());
	check(MainMenu != nullptr);

	MainMenu->Edit([&, this](MutableContext& Ctx, UCppMainMenuViewModel& MainMenuVM) {
		this->DeepLinkData = Url;
		MainMenuVM.SetActiveTab(Ctx, EMainMenuTab::Push);
	});
}

void UCppPushTabViewModel::OnInAppNotificationShown(const FCleverTapProperties& NotificationPayload)
{
	UCppMainMenuViewModel* const MainMenu = Cast<UCppMainMenuViewModel>(GetOuter());
	check(MainMenu != nullptr);

	MainMenu->Edit([&, this](MutableContext& Ctx, UCppMainMenuViewModel& MainMenuVM) {
		this->InAppShownData = ToDebugString(NotificationPayload);
		MainMenuVM.SetActiveTab(Ctx, EMainMenuTab::Push);
	});
}

void UCppPushTabViewModel::OnInAppNotificationDismissed(
	const FCleverTapProperties& Extras, const FCleverTapProperties& ActionExtras)
{
	UCppMainMenuViewModel* const MainMenu = Cast<UCppMainMenuViewModel>(GetOuter());
	check(MainMenu != nullptr);

	MainMenu->Edit([&, this](MutableContext& Ctx, UCppMainMenuViewModel& MainMenuVM) {
		this->InAppDismissedData = ToDebugString(Extras);
		this->InAppDismissedActionData = ToDebugString(ActionExtras);
		MainMenuVM.SetActiveTab(Ctx, EMainMenuTab::Push);
	});
}

void UCppPushTabViewModel::OnInAppNotificationButtonClicked(const FCleverTapProperties& ButtonPayload)
{
	UCppMainMenuViewModel* const MainMenu = Cast<UCppMainMenuViewModel>(GetOuter());
	check(MainMenu != nullptr);

	MainMenu->Edit([&, this](MutableContext& Ctx, UCppMainMenuViewModel& MainMenuVM) {
		this->InAppButtonData = ToDebugString(ButtonPayload);
		MainMenuVM.SetActiveTab(Ctx, EMainMenuTab::Push);
	});
}
