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

//

static FText FormatPushPermissionGrantedText(bool bGranted)
{
	return FText::Format(
		NSLOCTEXT("CleverTapSample", "SampleMainMenuPushPermGrantedText", "Push Permission Granted: {Granted}"), [&] {
			FFormatNamedArguments Args;
			Args.Add("Granted", FText::FromString(bGranted ? TEXT("TRUE") : TEXT("FALSE")));
			return Args;
		}());
}

//===============================================
// USampleMainMenu

bool USampleMainMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
	check(CleverTapSys != nullptr);

	ConfigureSharedInstance();
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

	// simple test of the OnPushPermissionResponse notification
	CleverTap.OnPushPermissionResponse.AddUObject(this, &USampleMainMenu::OnPushPermissionResponse);
}

void USampleMainMenu::OnPushPermissionResponse(bool bGranted)
{
	UE_LOG(LogCleverTapSample, Log, TEXT("OnPushPermissionResponse(bGranted=%s)"),
		bGranted ? TEXT("TRUE") : TEXT("FALSE"));

	if (PushPermissionGrantedText)
	{
		PushPermissionGrantedText->SetText(FormatPushPermissionGrantedText(bGranted));
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
	PrimerConfig.TitleText = FText::FromString(TEXT("Alert Title"));
	PrimerConfig.MessageText = FText::FromString(TEXT("Alert Message"));
	PrimerConfig.PositiveButtonText = FText::FromString(TEXT("Positive"));
	PrimerConfig.NegativeButtonText = FText::FromString(TEXT("Negative"));
	PrimerConfig.bFallbackToSettings = true;
	CleverTap.PromptForPushPermission(PrimerConfig);
}

void USampleMainMenu::PromptForPushPermissionWithHalfInterstitialPrimer()
{
	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());
	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();

	FCleverTapPushPrimerHalfInterstitialConfig PrimerConfig;
	PrimerConfig.TitleText = FText::FromString(TEXT("Primer Title"));
	PrimerConfig.MessageText = FText::FromString(TEXT("Primer Message"));
	PrimerConfig.PositiveButtonText = FText::FromString(TEXT("Positive"));
	PrimerConfig.NegativeButtonText = FText::FromString(TEXT("Negative"));
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

	check(CleverTapSys != nullptr);
	check(CleverTapSys->IsSharedInstanceInitialized());
	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
	CleverTap.PushProfile(Profile);
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

void USampleMainMenu::Tick(float DeltaTime)
{
	if (CleverTapSys == nullptr || !CleverTapSys->IsSharedInstanceInitialized())
	{
		return;
	}

	FString CleverTapId = CleverTapSys->SharedInstance().GetCleverTapId();
	if (LastSeenCleverTapId != CleverTapSys->SharedInstance().GetCleverTapId())
	{
		PopulateUI();
		LastSeenCleverTapId = MoveTemp(CleverTapId);
	}
}

TStatId USampleMainMenu::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(USampleMainMenu, STATGROUP_Tickables);
}

void USampleMainMenu::PopulateUI() const
{
	if (CleverTapSys == nullptr || !CleverTapSys->IsSharedInstanceInitialized())
	{
		return;
	}

	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();

	if (CleverTapIdText)
	{
		const FString CleverTapId = CleverTap.GetCleverTapId();
		CleverTapIdText->SetText(
			FText::Format(NSLOCTEXT("CleverTapSample", "SampleMainMenuIdText", "CleverTap Id: {Id}"), [&] {
				FFormatNamedArguments Args;
				Args.Add("Id", FText::FromString(CleverTapId));
				return Args;
			}()));
	}

	if (PushPermissionGrantedText)
	{
		bool bGranted = CleverTap.IsPushPermissionGranted();
		PushPermissionGrantedText->SetText(FormatPushPermissionGrantedText(bGranted));
	}
}
