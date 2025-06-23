// Copyright CleverTap All Rights Reserved.
#include "CppEventTabViewModel.h"

#include "Algo/AnyOf.h"
#include "Algo/AllOf.h"
#include "CleverTapInstance.h"
#include "CleverTapProperties.h"
#include "CleverTapSample.h"
#include "CleverTapSubsystem.h"
#include "CleverTapSampleBlueprintFunctionLibrary.h"
#include "Engine.h"

namespace {
bool AreEventPropertiesValid(TArrayView<const FEventPropertyViewModel> Properties)
{
	const bool bAnyInvalidKey = Algo::AnyOf(Properties,
		[](const FEventPropertyViewModel& Prop) { return FStringView{ Prop.Key }.TrimStartAndEnd().IsEmpty(); });
	if (bAnyInvalidKey)
	{
		return false;
	}

	const bool bAllValidValues =
		Algo::AllOf(Properties, UCleverTapSampleBlueprintFunctionLibrary::IsEventPropertyValueValid);
	if (!bAllValidValues)
	{
		return false;
	}

	return true;
}

FCleverTapPropertyValue ToCleverTapPropertyValue(const FEventPropertyViewModel& VM)
{
	switch (VM.ValueType)
	{
		case EEventPropertyType::String:
		{
			return FCleverTapPropertyValue{ VM.ValueString };
		}

		case EEventPropertyType::Int32:
		{
			int32 Value{};
			LexFromString(Value, *VM.ValueString);
			return FCleverTapPropertyValue{ Value };
		}

		case EEventPropertyType::Int64:
		{
			int64 Value{};
			LexFromString(Value, *VM.ValueString);
			return FCleverTapPropertyValue{ Value };
		}

		case EEventPropertyType::Float:
		{
			float Value{};
			LexFromString(Value, *VM.ValueString);
			return FCleverTapPropertyValue{ Value };
		}

		case EEventPropertyType::Double:
		{
			double Value{};
			LexFromString(Value, *VM.ValueString);
			return FCleverTapPropertyValue{ Value };
		}

		case EEventPropertyType::Bool:
		{
			const bool bValue = VM.ValueString.Equals(TEXT("true"), ESearchCase::IgnoreCase);
			return FCleverTapPropertyValue{ bValue };
		}

		case EEventPropertyType::Date:
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
	}

	UE_LOG(LogCleverTapSample, Error, TEXT("Unhandled event property type: %d"), static_cast<uint8>(VM.ValueType));
	return FCleverTapPropertyValue{};
}
} // namespace

UCppEventTabViewModel::UCppEventTabViewModel(const FObjectInitializer& ObjectInitializer)
	: Super{ ObjectInitializer }
{
	FChargedEventProductViewModel& FirstProduct = Products.AddDefaulted_GetRef();
	FirstProduct.Properties.AddDefaulted();
}

void UCppEventTabViewModel::Edit(TFunctionRef<void(MutableContext&, UCppEventTabViewModel&)> EditFn)
{
	Super::Edit<UCppEventTabViewModel>(EditFn);
}

FString UCppEventTabViewModel::GetEventName_Implementation() const
{
	return EventName;
}

void UCppEventTabViewModel::SetEventName_Implementation(const FString& Value)
{
	Edit([&](MutableContext&, UCppEventTabViewModel& VM) { VM.EventName = Value; });
}

TArray<FEventPropertyViewModel> UCppEventTabViewModel::GetEventProperties_Implementation() const
{
	return EventProperties;
}

void UCppEventTabViewModel::AddEventProperty_Implementation(FEventPropertyViewModel Value)
{
	Edit([&](MutableContext&, UCppEventTabViewModel& VM) { VM.EventProperties.Add(MoveTemp(Value)); });
}

void UCppEventTabViewModel::SetEventPropertyAt_Implementation(int32 Index, FEventPropertyViewModel Value)
{
	if (!EventProperties.IsValidIndex(Index))
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("SetEventPropertyAt() - Invalid index: %d"), Index);
		return;
	}

	Edit([Index, &Value](MutableContext&, UCppEventTabViewModel& VM) { VM.EventProperties[Index] = MoveTemp(Value); });
}

void UCppEventTabViewModel::RemoveEventPropertyAt_Implementation(int32 Index)
{
	if (!EventProperties.IsValidIndex(Index))
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("SetEventPropertyAt() - Invalid index: %d"), Index);
		return;
	}

	Edit([Index](MutableContext&, UCppEventTabViewModel& VM) { VM.EventProperties.RemoveAt(Index); });
}

bool UCppEventTabViewModel::IsRecordEventEnabled_Implementation() const
{
	if (EventName.IsEmpty())
	{
		return false;
	}

	if (!AreEventPropertiesValid(EventProperties))
	{
		return false;
	}

	return true;
}

void UCppEventTabViewModel::RecordEvent_Implementation()
{
	FCleverTapProperties Params;
	Params.Map.Reserve(EventProperties.Num());

	for (const auto& Prop : EventProperties)
	{
		const auto Existing = Params.Map.Find(Prop.Key);
		if (Existing)
		{
			UE_LOG(LogCleverTapSample, Log, TEXT("RecordEvent overwrites existing '%s' value of '%s' with '%s'"),
				*Prop.Key, *ToDebugString(*Existing), *Prop.ValueString);
			*Existing = ToCleverTapPropertyValue(Prop);
		}
		else
		{
			Params.Map.Add(Prop.Key, ToCleverTapPropertyValue(Prop));
		}
	}

	Edit([&](MutableContext& Ctx, UCppEventTabViewModel& VM) {
		VM.LastRecordedEventData = FString::Printf(TEXT("name = %s"), *EventName);
		VM.LastRecordedEventData.Appendf(TEXT("\nparams = %s"), *ToDebugString(Params));
	});

	UE_LOG(LogCleverTapSample, Log, TEXT("Calling RecordEvent with name='%s', params: %s"), *EventName,
		*LastRecordedEventData);

	auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
	if (Params.Map.Num() == 0)
	{
		CleverTapInst.PushEvent(EventName);
	}
	else
	{
		CleverTapInst.PushEvent(EventName, Params);
	}
}

FString UCppEventTabViewModel::GetLastRecordedEventData_Implementation() const
{
	return LastRecordedEventData;
}

TArray<FEventPropertyViewModel> UCppEventTabViewModel::GetChargedEventProperties_Implementation() const
{
	return ChargedEventProperties;
}

void UCppEventTabViewModel::AddChargedEventProperty_Implementation(FEventPropertyViewModel Value)
{
	Edit([&](MutableContext&, UCppEventTabViewModel& VM) { VM.ChargedEventProperties.Add(MoveTemp(Value)); });
}

void UCppEventTabViewModel::SetChargedEventPropertyAt_Implementation(int32 Index, FEventPropertyViewModel Value)
{
	if (!ChargedEventProperties.IsValidIndex(Index))
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("SetChargedEventPropertyAt() - Invalid index: %d"), Index);
		return;
	}

	Edit([Index, &Value](
			 MutableContext&, UCppEventTabViewModel& VM) { VM.ChargedEventProperties[Index] = MoveTemp(Value); });
}

void UCppEventTabViewModel::RemoveChargedEventPropertyAt_Implementation(int32 Index)
{
	if (!ChargedEventProperties.IsValidIndex(Index))
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("SetChargedEventPropertyAt() - Invalid index: %d"), Index);
		return;
	}

	Edit([Index](MutableContext&, UCppEventTabViewModel& VM) { VM.ChargedEventProperties.RemoveAt(Index); });
}

TArray<FChargedEventProductViewModel> UCppEventTabViewModel::GetChargedEventProducts_Implementation() const
{
	return Products;
}

void UCppEventTabViewModel::AddChargedEventProduct_Implementation(FChargedEventProductViewModel Value)
{
	Edit([&](MutableContext&, UCppEventTabViewModel& VM) {
		Value.ProductIndex = VM.Products.Num();
		if (Value.Properties.Num() == 0)
		{
			Value.Properties.AddDefaulted();
		}

		VM.Products.Add(MoveTemp(Value));
	});
}

void UCppEventTabViewModel::SetChargedEventProductAt_Implementation(int32 Index, FChargedEventProductViewModel Value)
{
	if (!Products.IsValidIndex(Index))
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("SetChargedEventProductAt() - Invalid index: %d"), Index);
		return;
	}

	Edit([Index, &Value](MutableContext&, UCppEventTabViewModel& VM) { VM.Products[Index] = MoveTemp(Value); });
}

void UCppEventTabViewModel::RemoveChargedEventProductAt_Implementation(int32 Index)
{
	if (!Products.IsValidIndex(Index))
	{
		UE_LOG(LogCleverTapSample, Warning, TEXT("RemoveChargedEventProductAt() - Invalid index: %d"), Index);
		return;
	}

	Edit([Index](MutableContext&, UCppEventTabViewModel& VM) {
		VM.Products.RemoveAt(Index);
		// Re-number products above this index
		for (int32 i = Index; i < VM.Products.Num(); ++i)
		{
			VM.Products[i].ProductIndex = i;
		}
	});
}

bool UCppEventTabViewModel::IsRecordChargedEventEnabled_Implementation() const
{
	if (!AreEventPropertiesValid(ChargedEventProperties))
	{
		return false;
	}

	if (Products.Num() == 0)
	{
		return false;
	}

	for (const auto& Product : Products)
	{
		if (!AreEventPropertiesValid(Product.Properties))
		{
			return false;
		}
	}

	return true;
}

void UCppEventTabViewModel::RecordChargedEvent_Implementation()
{
	FCleverTapProperties Details;
	Details.Map.Reserve(ChargedEventProperties.Num());

	for (const auto& Prop : ChargedEventProperties)
	{
		const auto Existing = Details.Map.Find(Prop.Key);
		if (Existing)
		{
			UE_LOG(LogCleverTapSample, Log, TEXT("RecordChargedEvent overwrites existing '%s' value of '%s' with '%s'"),
				*Prop.Key, *ToDebugString(*Existing), *Prop.ValueString);
			*Existing = ToCleverTapPropertyValue(Prop);
		}
		else
		{
			Details.Map.Add(Prop.Key, ToCleverTapPropertyValue(Prop));
		}
	}

	TArray<FCleverTapProperties> Items;
	Items.Reserve(Products.Num());

	for (auto const& Product : Products)
	{
		FCleverTapProperties& ProductDetails = Items.AddDefaulted_GetRef();
		ProductDetails.Map.Reserve(Product.Properties.Num());

		for (auto const& Prop : Product.Properties)
		{
			const auto Existing = ProductDetails.Map.Find(Prop.Key);
			if (Existing)
			{
				UE_LOG(LogCleverTapSample, Log,
					TEXT("RecordChargedEvent for product %d overwrites existing '%s' value of '%s' with '%s'"),
					Product.ProductIndex, *Prop.Key, *ToDebugString(*Existing), *Prop.ValueString);
				*Existing = ToCleverTapPropertyValue(Prop);
			}
			else
			{
				ProductDetails.Map.Add(Prop.Key, ToCleverTapPropertyValue(Prop));
			}
		}
	}

	Edit([&](MutableContext& Ctx, UCppEventTabViewModel& VM) {
		VM.LastRecordedChargedEventData = ToDebugString(Details);

		int i = 0;
		for (const auto& Item : Items)
		{
			VM.LastRecordedChargedEventData.Appendf(TEXT("\nproduct[%d] = %s"), i, *ToDebugString(Item));
			i += 1;
		}
	});

	UE_LOG(LogCleverTapSample, Log, TEXT("Calling RecordChargedEvent with details: %s"), *LastRecordedChargedEventData);

	auto& CleverTapInst = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
	CleverTapInst.PushChargedEvent(Details, Items);
}

FString UCppEventTabViewModel::GetLastRecordedChargedEventData_Implementation() const
{
	return LastRecordedChargedEventData;
}
