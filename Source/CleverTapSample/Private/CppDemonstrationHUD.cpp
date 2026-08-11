// Copyright CleverTap All Rights Reserved.
#include "CppDemonstrationHUD.h"

#include "CleverTapSample.h"
#include "CleverTapSubsystem.h"
#include "CppDemonstrationSaveGame.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "UserWidgetView.h"
#include "ViewModels/CppLoginPageViewModel.h"
#include "ViewModels/CppMainMenuViewModel.h"
#include "ViewModels/CppPrivacyTabViewModel.h"
#include "ViewModels/CppProfileTabViewModel.h"
#include "ViewModels/CppUserProfileViewModel.h"

namespace {
const FString SAVE_GAME_SLOT_NAME = TEXT("CppSaveState");

// On Android, FPaths::ProjectSavedDir() expands to ../../../CleverTapSample/Saved/
// which normalises to a path outside the app's writable files/ directory
// (3 levels above GFilePathBase lands at .../Android/data/{package}/ where
// creating subdirectories is blocked by Android 11+ scoped storage).
// GamePersistentDownloadDir() returns getExternalFilesDir() — always writable.
FString GetSaveFilePath()
{
#if PLATFORM_ANDROID
	return FString(FPlatformMisc::GamePersistentDownloadDir()) / TEXT("SaveGames") / (SAVE_GAME_SLOT_NAME + TEXT(".sav"));
#else
	return FPaths::ProjectSavedDir() / TEXT("SaveGames") / (SAVE_GAME_SLOT_NAME + TEXT(".sav"));
#endif
}

bool SaveGameToFile(UCppDemonstrationSaveGame& SaveGameObj)
{
	TArray<uint8> Data;
	if (!UGameplayStatics::SaveGameToMemory(&SaveGameObj, Data))
	{
		return false;
	}
	const FString FilePath = GetSaveFilePath();
	IFileManager::Get().MakeDirectory(*FPaths::GetPath(FilePath), /*Tree=*/true);
	return FFileHelper::SaveArrayToFile(Data, *FilePath);
}

UCppDemonstrationSaveGame* LoadGameFromFile()
{
	const FString FilePath = GetSaveFilePath();
	TArray<uint8> Data;
	if (!FFileHelper::LoadFileToArray(Data, *FilePath))
	{
		return nullptr;
	}
	return Cast<UCppDemonstrationSaveGame>(UGameplayStatics::LoadGameFromMemory(Data));
}

void DeleteGameFile()
{
	IFileManager::Get().Delete(*GetSaveFilePath());
}

} // namespace

void ACppDemonstrationHUD::BeginPlay()
{
	// Sanity checks
	auto* const CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());

	LocalizeAndroidNotificationChannels();

	Super::BeginPlay();

	// Set cursor mode for UI
	FInputModeGameAndUI Mode;
	Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	Mode.SetHideCursorDuringCapture(false);
	PlayerOwner->SetInputMode(Mode);
	
	//UE_LOG(LogTemp, Display, TEXT("Hello, World!"));
	
	UE_LOG(LogCleverTapSample, Display, TEXT("[SaveGame] Loading from: %s"), *GetSaveFilePath());
	SaveState = LoadGameFromFile();
	if (SaveState == nullptr)
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("[SaveGame] Load returned null — no save file on disk."));
		SetUIState(ECppDemonstrationUIState::Login);
	}
	else if (!SaveState->HasLoginData())
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("[SaveGame] Save file found but no login data."));
		SetUIState(ECppDemonstrationUIState::Login);
	}
	else
	{
		UE_LOG(LogCleverTapSample, Display, TEXT("[SaveGame] Restoring login state."));
		ApplyPrivacySettingsFromSaveState();
		RestoreLoginFromSaveState();
		SetUIState(ECppDemonstrationUIState::MainMenu);
	}
}

void ACppDemonstrationHUD::DeleteSaveState()
{
	SaveState = nullptr;
	DeleteGameFile();
	SetUIState(ECppDemonstrationUIState::Login);
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
	const bool bSaved = SaveGameToFile(Save);
	UE_LOG(LogCleverTapSample, Display, TEXT("[SaveGame] Login save %s to %s (Name=%s Email=%s Identity=%s)"),
		bSaved ? TEXT("succeeded") : TEXT("FAILED"), *GetSaveFilePath(), *Name, *Email, *Identity);

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

void ACppDemonstrationHUD::RestoreLoginFromSaveState()
{
	check(SaveState != nullptr);
	FCleverTapProperties Profile;
	if (!SaveState->Name.IsEmpty())     Profile.Map.Add("Name", SaveState->Name);
	if (!SaveState->Email.IsEmpty())    Profile.Map.Add("Email", SaveState->Email);
	if (!SaveState->Phone.IsEmpty())    Profile.Map.Add("Phone", SaveState->Phone);
	if (!SaveState->Identity.IsEmpty()) Profile.Map.Add("Identity", SaveState->Identity);

	auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
	if (SaveState->CustomCleverTapId.IsEmpty())
	{
		CleverTapInst.OnUserLogin(Profile);
	}
	else
	{
		CleverTapInst.OnUserLoginWithCleverTapId(Profile, SaveState->CustomCleverTapId);
	}
}

void ACppDemonstrationHUD::SyncSaveStateToViewModels(TScriptInterface<IViewModelInterface> VM)
{
	check(MainMenuVM != nullptr);
	if (SaveState == nullptr)
	{
		return;
	}
	UCppDemonstrationSaveGame& Save = GetOrCreateSaveState();

	auto const Profile = MainMenuVM->GetProfileTab()->GetUserProfile();
	Save.Name = Profile->GetProfileName();
	Save.Email = Profile->GetEmail();
	Save.Phone = Profile->GetPhone();

	auto const Privacy = MainMenuVM->GetPrivacyTab();
	Save.bIsOptOut = Privacy->GetOptOut();
	Save.bIsOffline = Privacy->GetOffline();
	Save.bIsNotRecordingNetInfo = !Privacy->GetNetworkRecording();

	if (!SaveGameToFile(Save))
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("[SaveGame] Failed to save state to file."));
	}
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

		case ECppDemonstrationUIState::MainMenu:
		{
			EndDisplay_MainMenu();
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
	CleverTapInst.ResumeInAppNotifications();

	UE_LOG(LogCleverTapSample, Log, TEXT("Displaying UI: Main Menu"));
}

void ACppDemonstrationHUD::EndDisplay_MainMenu()
{
	check(ActiveView != nullptr);
	check(MainMenuVM != nullptr);

	UE_LOG(LogCleverTapSample, Log, TEXT("Removing UI: Main Menu"));
	MainMenuVM->OnViewModelChanged().RemoveDynamic(this, &ACppDemonstrationHUD::SyncSaveStateToViewModels);
	MainMenuVM->AttachToView(nullptr);
	MainMenuVM = nullptr;

	ActiveView->RemoveFromViewport();
	ActiveView = nullptr;
}

void ACppDemonstrationHUD::LocalizeAndroidNotificationChannels()
{
	// Localize the names and descriptions of the android notification channels & groups
	auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
	CleverTapInst.LocalizeAndroidNotificationChannelGroup(
		TEXT("general"), NSLOCTEXT("CleverTapSample", "ChannelGroupName_general", "General"));

	CleverTapInst.LocalizeAndroidNotificationChannelGroup(
		TEXT("system"), NSLOCTEXT("CleverTapSample", "ChannelGroupName_system", "System"));

	CleverTapInst.LocalizeAndroidNotificationChannelGroup(
		TEXT("marketing"), NSLOCTEXT("CleverTapSample", "ChannelGroupName_marketing", "Marketing"));

	CleverTapInst.LocalizeAndroidNotificationChannel(TEXT("messages"),
		NSLOCTEXT("CleverTapSample", "ChannelName_messages", "Messages"),
		NSLOCTEXT("CleverTapSample", "ChannelDesc_messages", "Chat Messages"));

	CleverTapInst.LocalizeAndroidNotificationChannel(TEXT("reminders"),
		NSLOCTEXT("CleverTapSample", "ChannelName_reminders", "Reminders"),
		NSLOCTEXT("CleverTapSample", "ChannelDesc_reminders", "Forgotten Something?"));

	CleverTapInst.LocalizeAndroidNotificationChannel(TEXT("news"),
		NSLOCTEXT("CleverTapSample", "ChannelName_news", "News"),
		NSLOCTEXT("CleverTapSample", "ChannelDesc_news", "Important news and alerts"));

	CleverTapInst.LocalizeAndroidNotificationChannel(TEXT("system_alerts"),
		NSLOCTEXT("CleverTapSample", "ChannelName_system_alerts", "System Alerts"),
		NSLOCTEXT("CleverTapSample", "ChannelDesc_system_alerts", "Important System Messages"));

	CleverTapInst.LocalizeAndroidNotificationChannel(TEXT("updates"),
		NSLOCTEXT("CleverTapSample", "ChannelName_updates", "Updates"),
		NSLOCTEXT("CleverTapSample", "ChannelDesc_updates", "New Version Announcments"));

	CleverTapInst.LocalizeAndroidNotificationChannel(TEXT("promotions"),
		NSLOCTEXT("CleverTapSample", "ChannelName_promotions", "Promotions"),
		NSLOCTEXT("CleverTapSample", "ChannelDesc_promotions", "Special offers"));

	CleverTapInst.LocalizeAndroidNotificationChannel(TEXT("surveys"),
		NSLOCTEXT("CleverTapSample", "ChannelName_surveys", "Surveys"),
		NSLOCTEXT("CleverTapSample", "ChannelDesc_surveys", "Special offers"));
}
