// Copyright CleverTap All Rights Reserved.
#include "ViewModels/CppProfileTabViewModel.h"

#include "Algo/AllOf.h"
#include "Algo/AnyOf.h"
#include "CleverTapInstance.h"
#include "CleverTapSample.h"
#include "CleverTapSampleBlueprintFunctionLibrary.h"
#include "CleverTapSubsystem.h"
#include "CppDemonstrationHUD.h"
#include "Engine.h"
#include "ViewModels/CppUserProfileViewModel.h"
#include "ViewModels/ProfilePropertyViewModel.h"

namespace {
FCleverTapPropertyValue ToCleverTapPropertyValue(const FProfilePropertyViewModel& VM)
{
	switch (VM.ValueType)
	{
		case EProfilePropertyType::String:
		{
			return FCleverTapPropertyValue{ VM.ValueString };
		}

		case EProfilePropertyType::Int32:
		{
			int32 Value{};
			LexFromString(Value, *VM.ValueString);
			return FCleverTapPropertyValue{ Value };
		}

		case EProfilePropertyType::Int64:
		{
			int64 Value{};
			LexFromString(Value, *VM.ValueString);
			return FCleverTapPropertyValue{ Value };
		}

		case EProfilePropertyType::Float:
		{
			float Value{};
			LexFromString(Value, *VM.ValueString);
			return FCleverTapPropertyValue{ Value };
		}

		case EProfilePropertyType::Double:
		{
			double Value{};
			LexFromString(Value, *VM.ValueString);
			return FCleverTapPropertyValue{ Value };
		}

		case EProfilePropertyType::Bool:
		{
			const bool bValue = VM.ValueString.Equals(TEXT("true"), ESearchCase::IgnoreCase);
			return FCleverTapPropertyValue{ bValue };
		}

		case EProfilePropertyType::Date:
		{
			TArray<FString> Parts{};
			VM.ValueString.ParseIntoArray(Parts, TEXT("-"));
			check(Parts.Num() == 3);

			int32 Year{};
			LexFromString(Year, *Parts[0]);

			int32 Month{};
			LexFromString(Month, *Parts[1]);

			int32 Day{};
			LexFromString(Day, *Parts[2]);

			return FCleverTapPropertyValue{ FCleverTapDate{ Year, Month, Day } };
		}

		case EProfilePropertyType::Array_String:
		{
			TArray<FString> Values;

			FString CurrentValue{};
			for (int32 i = 0; i < VM.ValueString.Len(); ++i)
			{
				const TCHAR CurrentChar = VM.ValueString[i];
				if (CurrentChar == TCHAR{ '\\' })
				{
					// Skip this and next value
					++i;
				}
				else if (CurrentChar == TCHAR{ ',' })
				{
					// Add entry
					Values.Add(MoveTemp(CurrentValue));
					CurrentValue = FString{};
				}
				else
				{
					CurrentValue.AppendChar(CurrentChar);
				}
			}

			return FCleverTapPropertyValue{ Values };
		}

		case EProfilePropertyType::Array_Int32:
		{
			TArray<int32> Values;

			TArray<FString> Parts;
			VM.ValueString.ParseIntoArray(Parts, TEXT(","));
			for (auto& Part : Parts)
			{
				Part.TrimStartAndEndInline();

				int32 Value{};
				LexFromString(Value, *Part);

				Values.Add(Value);
			}

			return FCleverTapPropertyValue{ Values };
		}

		case EProfilePropertyType::Array_Int64:
		{
			TArray<int64> Values;

			TArray<FString> Parts;
			VM.ValueString.ParseIntoArray(Parts, TEXT(","));
			for (auto& Part : Parts)
			{
				Part.TrimStartAndEndInline();

				int64 Value{};
				LexFromString(Value, *Part);

				Values.Add(Value);
			}

			return FCleverTapPropertyValue{ Values };
		}

		case EProfilePropertyType::Array_Float:
		{
			TArray<float> Values;

			TArray<FString> Parts;
			VM.ValueString.ParseIntoArray(Parts, TEXT(","));
			for (auto& Part : Parts)
			{
				Part.TrimStartAndEndInline();

				float Value{};
				LexFromString(Value, *Part);

				Values.Add(Value);
			}

			return FCleverTapPropertyValue{ Values };
		}

		case EProfilePropertyType::Array_Double:
		{
			TArray<double> Values;

			TArray<FString> Parts;
			VM.ValueString.ParseIntoArray(Parts, TEXT(","));
			for (auto& Part : Parts)
			{
				Part.TrimStartAndEndInline();

				double Value{};
				LexFromString(Value, *Part);

				Values.Add(Value);
			}

			return FCleverTapPropertyValue{ Values };
		}

		case EProfilePropertyType::Array_Bool:
		{
			TArray<bool> Values;

			TArray<FString> Parts;
			VM.ValueString.ParseIntoArray(Parts, TEXT(","));
			for (auto& Part : Parts)
			{
				Part.TrimStartAndEndInline();
				Values.Add(Part.Equals(TEXT("true"), ESearchCase::IgnoreCase));
			}

			return FCleverTapPropertyValue{ Values };
		}
	}

	UE_LOG(LogCleverTapSample, Error, TEXT("Unhandled profile property type: %d"), static_cast<uint8>(VM.ValueType));
	return FCleverTapPropertyValue{};
}
} // namespace

UCppProfileTabViewModel::UCppProfileTabViewModel(const FObjectInitializer& ObjectInitializer)
	: Super{ ObjectInitializer }
{
	UserProfile = CreateDefaultSubobject<UCppUserProfileViewModel>("User Profile");
	UserProfile->OnViewModelChanged().AddDynamic(this, &UCppProfileTabViewModel::PropogateViewModelChanged);
}

UCppUserProfileViewModel* UCppProfileTabViewModel::GetUserProfile() const
{
	return UserProfile;
}

void UCppProfileTabViewModel::Edit(TFunctionRef<void(MutableContext&, UCppProfileTabViewModel&)> EditFn)
{
	Super::Edit<UCppProfileTabViewModel>(EditFn);
}

TScriptInterface<IUserProfileViewModelInterface> UCppProfileTabViewModel::GetUserProfile_Implementation() const
{
	return TScriptInterface<IUserProfileViewModelInterface>{ UserProfile };
}

bool UCppProfileTabViewModel::IsPushProfileEnabled_Implementation() const
{
	TArrayView<const FProfilePropertyViewModel> Properties = UserProfile->GetProfileProperties();
	if (Properties.Num() > 0)
	{
		const bool bAnyInvalidKey = Algo::AnyOf(Properties,
			[](const FProfilePropertyViewModel& Prop) { return FStringView{ Prop.Key }.TrimStartAndEnd().IsEmpty(); });
		if (bAnyInvalidKey)
		{
			return false;
		}

		const bool bAllValidValues = Algo::AllOf(
			UserProfile->GetProfileProperties(), UCleverTapSampleBlueprintFunctionLibrary::IsProfilePropertyValueValid);
		if (!bAllValidValues)
		{
			return false;
		}
	}

	return true;
}

void UCppProfileTabViewModel::PushProfile_Implementation()
{
	check(IsPushProfileEnabled_Implementation());

	FCleverTapProperties Profile;
	const auto& Name = UserProfile->GetProfileName();
	if (!Name.IsEmpty())
	{
		Profile.Map.Add("Name", Name);
	}

	const auto& Email = UserProfile->GetEmail();
	if (!Email.IsEmpty())
	{
		Profile.Map.Add("Email", Email);
	}

	const auto& Phone = UserProfile->GetPhone();
	if (!Phone.IsEmpty())
	{
		Profile.Map.Add("Phone", Phone);
	}

	const auto& Properties = UserProfile->GetProfileProperties();
	Profile.Map.Reserve(Profile.Map.Num() + Properties.Num());
	for (const auto& Prop : Properties)
	{
		const auto Existing = Profile.Map.Find(Prop.Key);
		if (Existing)
		{
			UE_LOG(LogCleverTapSample, Log, TEXT("PushProfile overwrites existing '%s' value of '%s' with '%s'"),
				*Prop.Key, *ToDebugString(*Existing), *Prop.ValueString);
			*Existing = ToCleverTapPropertyValue(Prop);
		}
		else
		{
			Profile.Map.Add(Prop.Key, ToCleverTapPropertyValue(Prop));
		}
	}

	Edit([&](MutableContext& Ctx, UCppProfileTabViewModel& VM) { VM.LastPushedProfileData = ToDebugString(Profile); });
	UE_LOG(LogCleverTapSample, Log, TEXT("Calling PushProfile with: %s"), *LastPushedProfileData);

	auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
	CleverTapInst.PushProfile(Profile);
}

FString UCppProfileTabViewModel::GetLastPushedProfileData_Implementation() const
{
	return LastPushedProfileData;
}

void UCppProfileTabViewModel::DeleteSaveState_Implementation()
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
		HUD->DeleteSaveState();
	}
}
