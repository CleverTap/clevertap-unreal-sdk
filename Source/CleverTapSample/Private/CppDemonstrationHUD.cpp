// Copyright CleverTap All Rights Reserved.
#include "CppDemonstrationHUD.h"

#include "CleverTapSample.h"
#include "CleverTapSubsystem.h"
#include "CppDemonstrationSaveGame.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "UserWidgetView.h"
#include "ViewModels/CppLoginPageViewModel.h"
#include "ViewModels/CppMainMenuViewModel.h"
#include "ViewModels/CppPrivacyTabViewModel.h"
#include "ViewModels/CppProfileTabViewModel.h"
#include "ViewModels/CppUserProfileViewModel.h"

namespace {
const FString SAVE_GAME_SLOT_NAME = TEXT("CppSaveState");
} // namespace

void ACppDemonstrationHUD::BeginPlay()
{
	// Sanity checks
	auto* const CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());

	Super::BeginPlay();

	// Set cursor mode for UI
	FInputModeGameAndUI Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetHideCursorDuringCapture(false);
	PlayerOwner->SetInputMode(Mode);

	// Try to load the save state and switch to the main menu if we've logged in before
	if (UGameplayStatics::DoesSaveGameExist(SAVE_GAME_SLOT_NAME, /*UserIndex=*/0))
	{
		SaveState =
			Cast<UCppDemonstrationSaveGame>(UGameplayStatics::LoadGameFromSlot(SAVE_GAME_SLOT_NAME, /*UserIndex=*/0));
		ApplyPrivacySettingsFromSaveState();

		// This will transition to the MainMenu
		Login(SaveState->Name, SaveState->Email, SaveState->Phone, SaveState->Identity, SaveState->CustomCleverTapId);
	}
	else
	{
		SetUIState(ECppDemonstrationUIState::Login);
	}
}

void ACppDemonstrationHUD::DeleteSaveState()
{
	SaveState = nullptr;
	if (UGameplayStatics::DoesSaveGameExist(SAVE_GAME_SLOT_NAME, /*UserIndex=*/0))
	{
		UGameplayStatics::DeleteGameInSlot(SAVE_GAME_SLOT_NAME, /*UserIndex=*/0);
	}
}

void ACppDemonstrationHUD::Login(const FString& Name, const FString& Email, const FString& Phone,
	const FString& Identity, const FString& CustomCleverTapId)
{
	FCleverTapProperties Profile;
	if (!Name.IsEmpty())
	{
		Profile.Map.Add("Name", Name);
	}

	if (!Email.IsEmpty())
	{
		Profile.Map.Add("Email", Email);
	}

	if (!Phone.IsEmpty())
	{
		Profile.Map.Add("Phone", Phone);
	}

	if (!Identity.IsEmpty())
	{
		Profile.Map.Add("Identity", Identity);
	}

	auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
	if (CustomCleverTapId.IsEmpty())
	{
		CleverTapInst.OnUserLogin(Profile);
	}
	else
	{
		CleverTapInst.OnUserLoginWithCleverTapId(Profile, CustomCleverTapId);
	}

	UCppDemonstrationSaveGame& Save = GetOrCreateSaveState();
	Save.Name = Name;
	Save.Email = Email;
	Save.Phone = Phone;
	Save.Identity = Identity;
	Save.CustomCleverTapId = CustomCleverTapId;
	UGameplayStatics::SaveGameToSlot(&Save, SAVE_GAME_SLOT_NAME, /*UserIndex=*/0);

	SetUIState(ECppDemonstrationUIState::MainMenu);
}

UCppDemonstrationSaveGame& ACppDemonstrationHUD::GetOrCreateSaveState()
{
	if (SaveState == nullptr)
	{
		SaveState = Cast<UCppDemonstrationSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UCppDemonstrationSaveGame::StaticClass()));
	}
	return *SaveState;
}

void ACppDemonstrationHUD::ApplyPrivacySettingsFromSaveState()
{
	check(SaveState != nullptr);

	auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
	CleverTapInst.SetOptOut(SaveState->bIsOptOut);
	CleverTapInst.SetOffline(SaveState->bIsOffline);
	CleverTapInst.SetNetworkInformationRecording(!SaveState->bIsNotRecordingNetInfo);
}

void ACppDemonstrationHUD::SyncSaveStateToViewModels(TScriptInterface<IViewModelInterface> VM)
{
	check(MainMenuVM != nullptr);
	UCppDemonstrationSaveGame& Save = GetOrCreateSaveState();

	auto const Profile = MainMenuVM->GetProfileTab()->GetUserProfile();
	Save.Name = Profile->GetProfileName();
	Save.Email = Profile->GetEmail();
	Save.Phone = Profile->GetPhone();

	auto const Privacy = MainMenuVM->GetPrivacyTab();
	Save.bIsOptOut = Privacy->GetOptOut();
	Save.bIsOffline = Privacy->GetOffline();
	Save.bIsNotRecordingNetInfo = !Privacy->GetNetworkRecording();

	UGameplayStatics::SaveGameToSlot(&Save, SAVE_GAME_SLOT_NAME, /*UserIndex=*/0);
}

void ACppDemonstrationHUD::SetUIState(ECppDemonstrationUIState Value)
{
	if (State == Value)
	{
		return;
	}

	switch (State)
	{
		case ECppDemonstrationUIState::Login:
		{
			EndDisplay_LoginPage();
		}
		break;

		default:
		{
		}
		break;
	}

	State = Value;

	switch (State)
	{
		case ECppDemonstrationUIState::Login:
		{
			BeginDisplay_LoginPage();
		}
		break;

		case ECppDemonstrationUIState::MainMenu:
		{
			BeginDisplay_MainMenu();
		}
		break;

		default:
		{
			UE_LOG(LogCleverTapSample, Error, TEXT("Invalid transitioned to UI state: %d"), static_cast<uint8>(State));
		}
		break;
	}
}

void ACppDemonstrationHUD::BeginDisplay_LoginPage()
{
	check(ActiveView == nullptr);
	check(LoginPageVM == nullptr);

	UClass* const LoginPageClass = CleverTapLoginPageViewClassName.TryLoadClass<UUserWidgetView>();
	if (LoginPageClass == nullptr)
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("Failed to find CleverTap Login Page View Class: %s"),
			*CleverTapLoginPageViewClassName.ToString());
		return;
	}

	auto NewView = CreateWidget<UUserWidgetView>(PlayerOwner, LoginPageClass);
	check(NewView != nullptr);

	LoginPageVM = NewObject<UCppLoginPageViewModel>(this);
	LoginPageVM->Edit([](UCppViewModelBase::MutableContext& Ctx, UCppLoginPageViewModel& VM) {
		VM.SetIdentity(Ctx, TEXT("61026032"));

		VM.GetUserProfile()
			->SetProfileName(Ctx, TEXT("Jack Montana"))
			.SetEmail(Ctx, TEXT("jack@gmail.com"))
			.SetPhone(Ctx, TEXT("+14155551234"));
	});
	LoginPageVM->AttachToView(NewView);

	ActiveView = NewView;
	ActiveView->AddToViewport();

	UE_LOG(LogCleverTapSample, Log, TEXT("Displaying UI: Login Page"));
}

void ACppDemonstrationHUD::EndDisplay_LoginPage()
{
	check(ActiveView != nullptr);
	check(LoginPageVM != nullptr);
	check(LoginPageVM->GetAttachedView() == ActiveView);
	check(MainMenuVM == nullptr || MainMenuVM->GetAttachedView() != ActiveView);

	UE_LOG(LogCleverTapSample, Log, TEXT("Removing UI: Login Page"));
	LoginPageVM->AttachToView(nullptr);
	LoginPageVM = nullptr;

	ActiveView->RemoveFromViewport();
	ActiveView = nullptr;
}

void ACppDemonstrationHUD::BeginDisplay_MainMenu()
{
	check(ActiveView == nullptr);
	check(MainMenuVM == nullptr);

	UClass* const MenuClass = CleverTapMainMenuViewClassName.TryLoadClass<UUserWidgetView>();
	if (MenuClass == nullptr)
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("Failed to find CleverTap Menu View Class: %s"),
			*CleverTapMainMenuViewClassName.ToString());
		return;
	}

	auto NewView = CreateWidget<UUserWidgetView>(PlayerOwner, MenuClass);
	check(NewView != nullptr);

	auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();

	MainMenuVM = NewObject<UCppMainMenuViewModel>(this);
	MainMenuVM->Edit([&CleverTapInst, &Save = GetOrCreateSaveState()](
						 UCppViewModelBase::MutableContext& Ctx, UCppMainMenuViewModel& VM) {
		VM.SetActiveTab(Ctx, EMainMenuTab::Profile);

		auto const UserProfile = VM.GetProfileTab()->GetUserProfile();
		UserProfile->SetCleverTapId(Ctx, CleverTapInst.GetCleverTapId())
			.SetProfileName(Ctx, Save.Name)
			.SetEmail(Ctx, Save.Email)
			.SetPhone(Ctx, Save.Phone);

		VM.GetPrivacyTab()
			->SetOptOut(Ctx, Save.bIsOptOut)
			.SetOffline(Ctx, Save.bIsOffline)
			.SetNetworkRecording(Ctx, !Save.bIsNotRecordingNetInfo);
	});
	MainMenuVM->AttachToView(NewView);
	MainMenuVM->OnViewModelChanged().AddDynamic(this, &ACppDemonstrationHUD::SyncSaveStateToViewModels);

	ActiveView = NewView;
	ActiveView->AddToViewport();

	// Mark as we're ready to get callbacks for certain clevertap events
	CleverTapInst.EnableOnPushNotificationClicked();
	CleverTapInst.EnableOnOpenUrl();

	UE_LOG(LogCleverTapSample, Log, TEXT("Displaying UI: Main Menu"));
}
