// Copyright CleverTap All Rights Reserved.
#include "SampleMainMenu.h"

#include "CleverTapInstanceConfig.h"
#include "CleverTapProperties.h"
#include "CleverTapInstance.h"
#include "CleverTapSubsystem.h"

#include "Engine.h"

static FCleverTapProperties GetExampleCleverTapProfile()
{
	FCleverTapProperties Profile;
	Profile.Add("Name", "Jack Montana");			 // String
	Profile.Add("Identity", 61026032);				 // String or number
	Profile.Add("Email", "jack@gmail.com");			 // email
	Profile.Add("Phone", "+14155551234");			 // Phone (with the country code, starting with +)
	Profile.Add("Gender", "M");						 // Can be either M or F
	Profile.Add("DOB", FCleverTapDate(1953, 3, 13)); // Date of Birth.

	// optional fields. controls whether the user will be sent email, push etc.
	Profile.Add("MSG-email", false);   // Disable email notifications
	Profile.Add("MSG-push", true);	   // Enable push notifications
	Profile.Add("MSG-sms", false);	   // Disable SMS notifications
	Profile.Add("MSG-whatsapp", true); // Enable WhatsApp notifications

	TArray<FString> stuff;
	stuff.Add("bag");
	stuff.Add("shoes");
	Profile.Add("MyStuff", stuff); // ArrayList of Strings

	TArray<FString> otherStuff = { "Jeans", "Perfume" };
	Profile["MyStuff"] = otherStuff;
	return Profile;
}

static void SampleTestCode()
{
	auto CleverTapSys = GEngine->GetEngineSubsystem<UCleverTapSubsystem>();
	CleverTapSys->SetLogLevel(ECleverTapLogLevel::Verbose);
	ICleverTapInstance& CleverTap = CleverTapSys->SharedInstance();

	// User Profile
	FCleverTapProperties Profile = GetExampleCleverTapProfile();
	CleverTap.OnUserLogin(Profile);
	CleverTap.PushProfile(Profile);

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
