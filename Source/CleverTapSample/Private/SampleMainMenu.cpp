// Copyright CleverTap All Rights Reserved.
#include "SampleMainMenu.h"

#include "CleverTapInstanceConfig.h"
#include "CleverTapSubsystem.h"
#include "Engine.h"

bool USampleMainMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
	check(CleverTapSys != nullptr);

	PopulateUI();
	return true;
}

void USampleMainMenu::InitializeSharedInstanceFromConfig()
{
	check(CleverTapSys != nullptr);

	CleverTapSys->InitializeSharedInstance();

	check(CleverTapSys->IsSharedInstanceInitialized());
	PopulateUI();
}

void USampleMainMenu::ExplicitlyInitializeSharedInstance(
	const FString& Id, const FString& Token, const FString& RegionCode)
{
	check(CleverTapSys != nullptr);

	FCleverTapInstanceConfig Config;
	Config.ProjectId = Id;
	Config.ProjectToken = Token;
	Config.RegionCode = RegionCode;
	CleverTapSys->InitializeSharedInstance(Config);

	check(CleverTapSys->IsSharedInstanceInitialized());
	PopulateUI();
}

void USampleMainMenu::PopulateUI() const
{
	if (CleverTapSys == nullptr || !CleverTapSys->IsSharedInstanceInitialized())
	{
		return;
	}

	ICleverTapInstance& SharedInst = CleverTapSys->SharedInstance();

	if (CleverTapIdText)
	{
		const FString CleverTapId = SharedInst.GetCleverTapId();
		CleverTapIdText->SetText(
			FText::Format(NSLOCTEXT("CleverTapSample", "SampleMainMenuIdText", "CleverTap Id: {Id}"), [&] {
				FFormatNamedArguments Args;
				Args.Add("Id", FText::FromString(CleverTapId));
				return Args;
			}()));
	}
}
