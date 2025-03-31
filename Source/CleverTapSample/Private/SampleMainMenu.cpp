// Copyright CleverTap All Rights Reserved.
#include "SampleMainMenu.h"

#include "CleverTapInstanceConfig.h"
#include "CleverTapProperties.h"
#include "CleverTapInstance.h"
#include "CleverTapSample.h"
#include "CleverTapSubsystem.h"

#include "Engine.h"

namespace {

FString FCleverTapSampleKeyValuePairArrayToString(const TArray<FCleverTapSampleKeyValuePair>& Params)
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

} // namespace

static void SampleTestCode()
{
	auto CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
	CleverTapSys->SetLogLevel(ECleverTapLogLevel::Verbose);
	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();

	// event without properties
	CleverTap.PushEvent(TEXT("Event No Props"));

	// event with properties
	FCleverTapProperties Actions;
	Actions.Add("Product Name", "Casio Chronograph Watch");
	Actions.Add("Category", "Mens Accessories");
	Actions.Add("Price", 59.99);
	// Actions.put("Date", FCleverTapDate::Today() ); TODO
	CleverTap.PushEvent(TEXT("Product viewed"), Actions);

	// charge event
	FCleverTapProperties ChargeDetails;
	ChargeDetails.Add("Amount", 300);
	ChargeDetails.Add("Payment Mode", "Credit card");
	ChargeDetails.Add("Charged ID", 24052014);

	FCleverTapProperties Item1;
	Item1.Add("Product category", "books");
	Item1.Add("Book name", "The Millionaire next door");
	Item1.Add("Quantity", 1);

	FCleverTapProperties Item2;
	Item2.Add("Product category", "books");
	Item2.Add("Book name", "Achieving inner zen");
	Item2.Add("Quantity", 1);

	FCleverTapProperties Item3;
	Item3.Add("Product category", "books");
	Item3.Add("Book name", "Chuck it, let's do it");
	Item3.Add("Quantity", 5);

	TArray<FCleverTapProperties> Items;
	Items.Add(Item1);
	Items.Add(Item2);
	Items.Add(Item3);

	CleverTap.PushChargedEvent(ChargeDetails, Items);
}

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
	Config.LogLevel = ECleverTapLogLevel::Verbose;
	CleverTapSys->InitializeSharedInstance(Config);

	check(CleverTapSys->IsSharedInstanceInitialized());
	PopulateUI();
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

	for (const FCleverTapSampleKeyValuePair& Param : Params)
	{
		FCleverTapPropertyValue* const ExistingValue = Profile.Find(Param.Key);
		if (ExistingValue)
		{
			UE_LOG(LogCleverTapSample, Warning,
				TEXT("PushProfile() parameter '%s' already exists. Value will be overwritten"));
			ExistingValue->Emplace<FString>(Param.Value);
		}
		else
		{
			Profile.Add(Param.Key, Param.Value);
		}
	}

	UE_LOG(LogCleverTapSample, Log, TEXT("Calling PushProfile with Name='%s', Email='%s', Phone='%s', Params=%s"),
		*Name, *Email, *Phone, *FCleverTapSampleKeyValuePairArrayToString(Params));

	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();
	CleverTap.PushProfile(Profile);
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

	SampleTestCode();
}
