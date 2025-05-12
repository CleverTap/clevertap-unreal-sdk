// Copyright CleverTap All Rights Reserved.
#include "SampleMainMenu.h"

#include "CleverTapInstanceConfig.h"
#include "CleverTapProperties.h"
#include "CleverTapInstance.h"
#include "CleverTapSample.h"
#include "CleverTapSubsystem.h"

#include "Engine.h"

namespace {

FString FCleverTapSampleKeyValuePairArrayToString(TArrayView<const FCleverTapSampleKeyValuePair> Params)
{
	FString Result{ TEXT("{") };

	const int32 NumParams = Params.Num();
	if (NumParams > 0)
	{
		for (const FCleverTapSampleKeyValuePair& Param : Params)
		{
			Result.AppendChar(TCHAR{ '"' });
			Result.Append(Param.Key);
			Result.Append(TEXT("\": \""));
			Result.Append(Param.Value);
			Result.Append(TEXT("\", "));
		}

		// Remove trailing ", "
		Result.LeftChopInline(2);
	}

	Result.AppendChar(TCHAR{ '}' });
	return Result;
}

FCleverTapProperties& PopulatePropertiesWith(
	FCleverTapProperties& Properties, TArrayView<const FCleverTapSampleKeyValuePair> KeyValuePairs)
{
	Properties.Reserve(Properties.Num() + KeyValuePairs.Num());

	for (const FCleverTapSampleKeyValuePair& Param : KeyValuePairs)
	{
		FCleverTapPropertyValue* const ExistingValue = Properties.Find(Param.Key);
		if (ExistingValue)
		{
			UE_LOG(
				LogCleverTapSample, Warning, TEXT("Property parameter '%s' already exists. Value will be overwritten"));
			ExistingValue->Emplace<FString>(Param.Value);
		}
		else
		{
			Properties.Add(Param.Key, Param.Value);
		}
	}

	return Properties;
}

} // namespace

//===============================================
// UCleverTapSampleProductList

void UCleverTapSampleProductList::AddProduct(const TArray<FCleverTapSampleKeyValuePair>& Params)
{
	ProductParameters.Add(Params);
}

int UCleverTapSampleProductList::ProductCount() const
{
	return ProductParameters.Num();
}

TArrayView<const FCleverTapSampleKeyValuePair> UCleverTapSampleProductList::GetProductParameters(int Index) const
{
	check(Index >= 0);
	check(Index < ProductParameters.Num());
	return TArrayView<const FCleverTapSampleKeyValuePair>(ProductParameters[Index]);
}

static FText FormatPushPermissionText(ECleverTapPushPermissionStatus Status)
{
	const FText StatusArg = [Status]() {
		switch (Status)
		{
			case ECleverTapPushPermissionStatus::Unknown:
				return NSLOCTEXT("CleverTapSample", "PushPermissionGranted_Unknown", "UNKNOWN");
			case ECleverTapPushPermissionStatus::Granted:
				return NSLOCTEXT("CleverTapSample", "PushPermissionGranted_True", "TRUE");
			case ECleverTapPushPermissionStatus::NotGranted:
				return NSLOCTEXT("CleverTapSample", "PushPermissionGranted_False", "FALSE");

			default:
			{
				UE_LOG(LogCleverTapSample, Error, TEXT("Unknown ECleverTapPushPermissionStatus value: %d"),
					static_cast<uint8>(Status));
				return FText{};
			}
		}
	}();

	return FText::Format(
		NSLOCTEXT("CleverTapSample", "SampleMainMenuPushPermGrantedText", "Push Permission Granted: {Granted}"),
		FFormatNamedArguments{ { "Granted", StatusArg } });
}

//===============================================
// USampleMainMenu

bool USampleMainMenu::Initialize()
{
	CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
	check(CleverTapSys != nullptr);

	if (!Super::Initialize())
	{
		return false;
	}

	if (CleverTapSys->IsSharedInstanceInitialized())
	{
		ConfigureSharedInstance();
	}

	PopulateUI();
	return true;
}

void USampleMainMenu::InitializeSharedInstanceFromConfig()
{
	check(CleverTapSys != nullptr);

	CleverTapSys->InitializeSharedInstance();
	check(CleverTapSys->IsSharedInstanceInitialized());

	ConfigureSharedInstance();
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
	Config.LogLevel = ECleverTapLogLevel::Verbose;
	CleverTapSys->InitializeSharedInstance(Config);
	check(CleverTapSys->IsSharedInstanceInitialized());

	ConfigureSharedInstance();
	PopulateUI();
}

void USampleMainMenu::ConfigureSharedInstance()
{
	check(CleverTapSys->IsSharedInstanceInitialized());
	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();

	// Localize the names and descriptions of the android notification channels & groups
	{
		CleverTap.LocalizeAndroidNotificationChannelGroup(
			TEXT("general"), NSLOCTEXT("CleverTapSample", "ChannelGroupName_general", "General"));

		CleverTap.LocalizeAndroidNotificationChannelGroup(
			TEXT("system"), NSLOCTEXT("CleverTapSample", "ChannelGroupName_system", "System"));

		CleverTap.LocalizeAndroidNotificationChannelGroup(
			TEXT("marketing"), NSLOCTEXT("CleverTapSample", "ChannelGroupName_marketing", "Marketing"));

		CleverTap.LocalizeAndroidNotificationChannel(TEXT("messages"),
			NSLOCTEXT("CleverTapSample", "ChannelName_messages", "Messages"),
			NSLOCTEXT("CleverTapSample", "ChannelDesc_messages", "Chat Messages"));

		CleverTap.LocalizeAndroidNotificationChannel(TEXT("reminders"),
			NSLOCTEXT("CleverTapSample", "ChannelName_reminders", "Reminders"),
			NSLOCTEXT("CleverTapSample", "ChannelDesc_reminders", "Forgotten Something?"));

		CleverTap.LocalizeAndroidNotificationChannel(TEXT("news"),
			NSLOCTEXT("CleverTapSample", "ChannelName_news", "News"),
			NSLOCTEXT("CleverTapSample", "ChannelDesc_news", "Important news and alerts"));

		CleverTap.LocalizeAndroidNotificationChannel(TEXT("system_alerts"),
			NSLOCTEXT("CleverTapSample", "ChannelName_system_alerts", "System Alerts"),
			NSLOCTEXT("CleverTapSample", "ChannelDesc_system_alerts", "Important System Messages"));

		CleverTap.LocalizeAndroidNotificationChannel(TEXT("updates"),
			NSLOCTEXT("CleverTapSample", "ChannelName_updates", "Updates"),
			NSLOCTEXT("CleverTapSample", "ChannelDesc_updates", "New Version Announcments"));

		CleverTap.LocalizeAndroidNotificationChannel(TEXT("promotions"),
			NSLOCTEXT("CleverTapSample", "ChannelName_promotions", "Promotions"),
			NSLOCTEXT("CleverTapSample", "ChannelDesc_promotions", "Special offers"));

		CleverTap.LocalizeAndroidNotificationChannel(TEXT("surveys"),
			NSLOCTEXT("CleverTapSample", "ChannelName_surveys", "Surveys"),
			NSLOCTEXT("CleverTapSample", "ChannelDesc_surveys", "Special offers"));
	}

	// simple test of the OnPushPermissionResponse notification
	CleverTap.OnPushPermissionResponse.AddUObject(this, &USampleMainMenu::OnPushPermissionResponse);

	// simple test of the OnPushNotificationClicked notification
	CleverTap.OnPushNotificationClicked.AddUObject(this, &USampleMainMenu::OnPushNotificationClicked);

	// simple test of the OnOpenUrl notification
	CleverTap.OnOpenUrl.AddUObject(this, &USampleMainMenu::OnOpenUrl);

	CleverTap.RegisterCleverTapUrlHandler(
		[WeakThis = TSoftObjectPtr<USampleMainMenu>{ this }](FString Url, ECleverTapChannel Channel) {
			// Make sure to do the update on the game thread
			AsyncTask(ENamedThreads::GameThread, [WeakThis, Channel, Url = MoveTemp(Url)] {
				if (auto* const Self = WeakThis.Get())
				{
					if (Self->DeepLinkText)
					{
						Self->DeepLinkText->SetText(FText::FromString(Url));
					}
				}
			});
			return true;
		});

	// we're now ready to handle the OnPushNotificationClicked notifications, enable them.
	// if we were launched in response to clicking on a notification, this will generate a
	// call to the OnPushNotificationClicked() we just registered
	CleverTap.EnableOnPushNotificationClicked();

	// In-App Callbacks
	CleverTap.OnInAppNotificationShown.AddUObject(this, &USampleMainMenu::OnInAppNotificationShown);
	CleverTap.OnInAppNotificationDismissed.AddUObject(this, &USampleMainMenu::OnInAppNotificationDismissed);

	// In-App Messaging Filter
	CleverTap.RegisterInAppNotificationFilter([](const FCleverTapProperties& Payload) {
		UE_LOG(LogCleverTapSample, Log, TEXT("InAppNotificationFilter(Payload=%s)"), *ToDebugString(Payload));
		return true;
	});
}

void USampleMainMenu::OnPushPermissionResponse(bool bGranted)
{
	UE_LOG(LogCleverTapSample, Log, TEXT("OnPushPermissionResponse(bGranted=%s)"),
		bGranted ? TEXT("TRUE") : TEXT("FALSE"));

	if (PushPermissionGrantedText)
	{
		const ECleverTapPushPermissionStatus Status =
			bGranted ? ECleverTapPushPermissionStatus::Granted : ECleverTapPushPermissionStatus::NotGranted;
		PushPermissionGrantedText->SetText(FormatPushPermissionText(Status));
	}
}

void USampleMainMenu::OnPushNotificationClicked(const FCleverTapProperties& NotificationPayload)
{
	FString PayloadString = ToDebugString(NotificationPayload);
	UE_LOG(LogCleverTapSample, Log, TEXT("OnPushNotificationClicked(NotificationPayload=%s)"), *PayloadString);

	// update PushNotificationClickedText with a dump of the payload
	if (PushNotificationClickedText)
	{
		PushNotificationClickedText->SetText(FText::Format(
			NSLOCTEXT("CleverTapSample", "PushNotificationClickedPayload", "Push Notification Clicked: {Payload}"),
			FFormatNamedArguments{ { "Payload", FText::FromString(PayloadString) } }));
	}

	// Switch to the tab with the PushNotificationClickedText
	if (TabSwitcher)
	{
		TabSwitcher->SetActiveWidgetIndex(0);
	}
}

void USampleMainMenu::OnInAppNotificationShown(const FCleverTapProperties& NotificationPayload)
{
	FString PayloadString = ToDebugString(NotificationPayload);
	UE_LOG(LogCleverTapSample, Log, TEXT("OnInAppNotificationShown(NotificationPayload=%s)"), *PayloadString);

	if (InAppShownText)
	{
		InAppShownText->SetText(FText::FromString(PayloadString));
	}

	// Switch to the tab with the InAppShownText
	if (TabSwitcher)
	{
		TabSwitcher->SetActiveWidgetIndex(0);
	}
}

void USampleMainMenu::OnInAppNotificationDismissed(
	const FCleverTapProperties& Extras, const FCleverTapProperties& ActionExtras)
{
	FString ExtrasString = ToDebugString(Extras);
	FString ActionExtrasString = ToDebugString(ActionExtras);
	UE_LOG(LogCleverTapSample, Log, TEXT("OnInAppNotificationDismissed(Extras=%s, ActionExtras=%s)"), *ExtrasString,
		*ActionExtrasString);

	if (InAppDismissedText)
	{
		InAppDismissedText->SetText(FText::FromString(ExtrasString));
	}

	if (InAppDismissedActionText)
	{
		InAppDismissedActionText->SetText(FText::FromString(ActionExtrasString));
	}

	// Switch to the tab with the text boxes
	if (TabSwitcher)
	{
		TabSwitcher->SetActiveWidgetIndex(0);
	}
}

void USampleMainMenu::OnOpenUrl(const FString& Url)
{
	UE_LOG(LogCleverTapSample, Log, TEXT("OnOpenUrl(%s)"), *Url);
	DeepLinkText->SetText(FText::FromString(Url));

	// Switch to the tab with the DeepLinkText display
	if (TabSwitcher)
	{
		TabSwitcher->SetActiveWidgetIndex(0);
	}
}

void USampleMainMenu::OnUserLogin(const FString& Name, const FString& Email, const FString& Identity)
{
	OnUserLoginWithCleverTapId(Name, Email, Identity, /*CleverTapId=*/FString{});
}

void USampleMainMenu::OnUserLoginWithCleverTapId(
	const FString& Name, const FString& Email, const FString& Identity, const FString& CleverTapId)
{
	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());

	FCleverTapProperties Profile;
	if (!Name.IsEmpty())
	{
		Profile.Add("Name", Name);
	}

	if (!Email.IsEmpty())
	{
		Profile.Add("Email", Email);
	}

	if (!Identity.IsEmpty())
	{
		Profile.Add("Identity", Identity);
	}

	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();

	if (CleverTapId.IsEmpty())
	{
		UE_LOG(LogCleverTapSample, Log, TEXT("Calling OnUserLogin with Name='%s', Email='%s', Identity='%s'"), *Name,
			*Email, *Identity);
		CleverTap.OnUserLogin(Profile);
	}
	else
	{
		UE_LOG(LogCleverTapSample, Log,
			TEXT("Calling OnUserLogin with CleverTapId='%s', Name='%s', Email='%s', Identity='%s'"), *CleverTapId,
			*Name, *Email, *Identity);
		CleverTap.OnUserLogin(Profile, CleverTapId);
	}
}

void USampleMainMenu::PromptForPushPermissionWithoutPrimer()
{
	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());
	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();

	bool bFallbackToSettings = true;
	CleverTap.PromptForPushPermission(bFallbackToSettings);
}

void USampleMainMenu::PromptForPushPermissionWithAlertPrimer()
{
	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());
	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();

	FCleverTapPushPrimerAlertConfig PrimerConfig;
	PrimerConfig.TitleText = NSLOCTEXT("CleverTapSample", "PushPrimerAlertTitle", "Alert Title Text");
	PrimerConfig.MessageText = NSLOCTEXT("CleverTapSample", "PushPrimerAlertMessage", "Alert Message Text");
	PrimerConfig.PositiveButtonText = NSLOCTEXT("CleverTapSample", "PushPrimerAlertPositiveButton", "Positive");
	PrimerConfig.NegativeButtonText = NSLOCTEXT("CleverTapSample", "PushPrimerAlertNegativeButton", "Negative");
	PrimerConfig.bFallbackToSettings = true;
	CleverTap.PromptForPushPermission(PrimerConfig);
}

void USampleMainMenu::PromptForPushPermissionWithHalfInterstitialPrimer()
{
	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());
	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();

	FCleverTapPushPrimerHalfInterstitialConfig PrimerConfig;
	PrimerConfig.TitleText = NSLOCTEXT("CleverTapSample", "PushPrimerHalfInterstitialTitle", "Push Primer Title Text");
	PrimerConfig.MessageText =
		NSLOCTEXT("CleverTapSample", "HalfInterstitialPushPrimerMessage", "Half-Interstitial Push Primer Message Text");
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
	PrimerConfig.ButtonBorderRadius = TEXT("10"); //? todo do we really want a string here?

	CleverTap.PromptForPushPermission(PrimerConfig);
}

void USampleMainMenu::PushProfile(
	const FString& Name, const FString& Email, const FString& Phone, const TArray<FCleverTapSampleKeyValuePair>& Params)
{
	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());

	FCleverTapProperties Profile;
	if (!Name.IsEmpty())
	{
		Profile.Add("Name", Name);
	}

	if (!Email.IsEmpty())
	{
		Profile.Add("Email", Email);
	}

	if (!Phone.IsEmpty())
	{
		Profile.Add("Phone", Phone);
	}

	PopulatePropertiesWith(Profile, Params);

	UE_LOG(LogCleverTapSample, Log, TEXT("Calling PushProfile with Name='%s', Email='%s', Phone='%s', Params=%s"),
		*Name, *Email, *Phone, *FCleverTapSampleKeyValuePairArrayToString(Params));

	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
	CleverTap.PushProfile(Profile);
}

void USampleMainMenu::PushProfileDataTypeTest()
{
	UE_LOG(LogCleverTapSample, Log, TEXT("Calling PushProfile with all the datatypes"));

	FCleverTapProperties Profile;
	Profile.Add("Test_String", "Jack Fish");
	Profile.Add("Test_Int", int32(1332));
	Profile.Add("Test_Long", int64(13320000123456789));
	Profile.Add("Test_Float", 1.36f);
	Profile.Add("Test_Double", 3.141592653589);
	Profile.Add("Test_Bool", true);
	Profile.Add("Test_Date", FCleverTapDate(1953, 3, 13));

	Profile.Add("Test_StringArray", TArray<FString>{ "one", "two", "three" });

	// these get converted to arrays of strings:
	Profile.Add("Test_IntArray", TArray<int32>{ 1, 2, 3 });
	Profile.Add("Test_LongArray", TArray<int64>{ 1, 2, 3 });
	Profile.Add("Test_DoubleArray", TArray<double>{ 1.1, 2.2, 3.3 });
	Profile.Add("Test_FloatArray", TArray<float>{ 1.1f, 2.2f, 3.3f });
	Profile.Add("Test_BoolArray", TArray<bool>{ true, false, true });

	UE_LOG(LogCleverTapSample, Log, TEXT("Properties=%s"), *ToDebugString(Profile));

	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());
	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
	CleverTap.PushProfile(Profile);

	// can increment/decrement number properties
	CleverTap.IncrementValue("Test_Long", 3);
	CleverTap.DecrementValue("Test_Long", 1);
	CleverTap.IncrementValue("Test_Double", 3.3);
	CleverTap.DecrementValue("Test_Double", 1.1);

	// exercise the multi-value methods;
	// should result in an array holding just "four", but it's async!
	CleverTap.RemoveValueForKey("Test_MVM");
	CleverTap.AddMultiValueForKey("Test_MVM", "one");
	CleverTap.AddMultiValuesForKey("Test_MVM", { "two", "three", "four" });
	CleverTap.RemoveMultiValueForKey("Test_MVM", "three");
	CleverTap.RemoveMultiValuesForKey("Test_MVM", { "one", "two" });

	// Exercise GetProperty() on each type.
	// As the above modifications happen asynchronously. GetProperty() will take awhile to be updated with the new
	// profile values. The below will likely only show the correct output on the second call to this function.
	for (const auto Pair : Profile)
	{
		UE_LOG(LogCleverTapSample, Log, TEXT("%s=%s"), *Pair.Key, *ToDebugString(CleverTap.GetProperty(Pair.Key)));
	}
	UE_LOG(LogCleverTapSample, Log, TEXT("Test_MVM=%s"), *ToDebugString(CleverTap.GetProperty("Test_MVM")));
}

void USampleMainMenu::RecordEvent(const FString& EventName, const TArray<FCleverTapSampleKeyValuePair>& Params)
{
	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());

	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
	if (Params.Num() == 0)
	{
		UE_LOG(LogCleverTapSample, Log, TEXT("Calling RecordEvent('%s')"), *EventName);
		CleverTap.PushEvent(EventName);
	}
	else
	{
		UE_LOG(LogCleverTapSample, Log, TEXT("Calling RecordEvent('%s', %s)"), *EventName,
			*FCleverTapSampleKeyValuePairArrayToString(Params));
		FCleverTapProperties Properties;
		PopulatePropertiesWith(Properties, Params);
		CleverTap.PushEvent(EventName, Properties);
	}
}

void USampleMainMenu::RecordChargedEvent(
	const TArray<FCleverTapSampleKeyValuePair>& Params, const UCleverTapSampleProductList* Products)
{
	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());

	if (Products == nullptr)
	{
		return;
	}

	FCleverTapProperties ChargeDetails{};
	PopulatePropertiesWith(ChargeDetails, Params);

	TArray<FCleverTapProperties> Items{};
	Items.Reserve(Products->ProductCount());
	for (int32 i = 0; i < Products->ProductCount(); ++i)
	{
		FCleverTapProperties& NewProduct = Items.AddDefaulted_GetRef();
		PopulatePropertiesWith(NewProduct, Products->GetProductParameters(i));
	}

	UE_LOG(LogCleverTapSample, Log, TEXT("Calling PushChargedEvent with %d products"), Items.Num());
	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
	CleverTap.PushChargedEvent(ChargeDetails, Items);
}

void USampleMainMenu::SetOptOut(bool bIsOptingOut)
{
	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());
	UE_LOG(LogCleverTapSample, Log, TEXT("Setting GDPR OptOut to %s"), bIsOptingOut ? TEXT("TRUE") : TEXT("FALSE"));

	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
	CleverTap.SetOptOut(bIsOptingOut);
}

void USampleMainMenu::SetNetworkInformationRecording(bool bEnableCollection)
{
	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());
	UE_LOG(LogCleverTapSample, Log, TEXT("Setting network information recording to %s"),
		bEnableCollection ? TEXT("TRUE") : TEXT("FALSE"));

	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
	CleverTap.SetNetworkInformationRecording(bEnableCollection);
}

void USampleMainMenu::Tick(float DeltaTime)
{
	if (CleverTapSys == nullptr || !CleverTapSys->IsSharedInstanceInitialized())
	{
		return;
	}

	const bool bNeedsUIRefresh = [this]() {
		ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
		if (LastSeenCleverTapId != CleverTap.GetCleverTapId())
		{
			return true;
		}

		if (bNeedsPushStatusRefresh && CleverTap.GetPushPermissionStatus() != ECleverTapPushPermissionStatus::Unknown)
		{
			return true;
		}

		return false;
	}();
	if (bNeedsUIRefresh)
	{
		PopulateUI();
	}
}

TStatId USampleMainMenu::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USampleMainMenu, STATGROUP_Tickables);
}

void USampleMainMenu::PopulateUI()
{
	if (CleverTapSys == nullptr || !CleverTapSys->IsSharedInstanceInitialized())
	{
		return;
	}

	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();

	if (CleverTapIdText)
	{
		FString CleverTapId = CleverTap.GetCleverTapId();
		CleverTapIdText->SetText(
			FText::Format(NSLOCTEXT("CleverTapSample", "SampleMainMenuIdText", "CleverTap Id: {Id}"), [&] {
				FFormatNamedArguments Args;
				Args.Add("Id", FText::FromString(CleverTapId));
				return Args;
			}()));
		LastSeenCleverTapId = MoveTemp(CleverTapId);
	}

	if (PushPermissionGrantedText)
	{
		const ECleverTapPushPermissionStatus Status = CleverTap.GetPushPermissionStatus();
		PushPermissionGrantedText->SetText(FormatPushPermissionText(Status));

		bNeedsPushStatusRefresh = (Status == ECleverTapPushPermissionStatus::Unknown);
	}
}
