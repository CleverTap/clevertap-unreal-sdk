// Copyright CleverTap All Rights Reserved.
#include "ViewModels/CppPETabViewModel.h"

#include "CleverTapInstance.h"
#include "CleverTapSubsystem.h"
#include "CleverTapSample.h"
#include "Engine.h"

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

void UCppPETabViewModel::Edit(TFunctionRef<void(MutableContext&, UCppPETabViewModel&)> EditFn)
{
	Super::Edit<UCppPETabViewModel>(EditFn);
}

UCppPETabViewModel& UCppPETabViewModel::SetFetchStatus(MutableContext&, const FString& Status)
{
	FetchStatus = Status;
	return *this;
}

UCppPETabViewModel& UCppPETabViewModel::SetVariableDisplayValue(MutableContext&, const FString& Name, const FString& Value)
{
	VariableDisplayValues.Add(Name, Value);
	return *this;
}

UCppPETabViewModel& UCppPETabViewModel::SetVariablesDefined(MutableContext&, bool bNewDefined)
{
	bDefined = bNewDefined;
	return *this;
}

void UCppPETabViewModel::PostInitProperties()
{
	Super::PostInitProperties();
	if (GetWorld())
	{
		auto& CT = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
		CT.OnVariablesFetched.AddDynamic(this, &UCppPETabViewModel::OnVariablesFetched);
		CT.OnVariablesChanged.AddDynamic(this, &UCppPETabViewModel::OnVariablesChanged);
	}
}

void UCppPETabViewModel::BeginDestroy()
{
	if (GetWorld())
	{
		auto& CT = GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
		CT.OnVariablesFetched.RemoveAll(this);
		CT.OnVariablesChanged.RemoveAll(this);
	}
	Super::BeginDestroy();
}

static UCleverTapInstance& GetCT()
{
	return GEngine->GetEngineSubsystem<UCleverTapSubsystem>()->SharedInstance();
}

// ---------------------------------------------------------------------------
// Private registration helpers (called only from DefineVariables)
// ---------------------------------------------------------------------------

void UCppPETabViewModel::RegisterStringVar(const FString& Name, const FString& Default)
{
	GetCT().DefineStringVariable(Name, Default);
	if (!VariableNames.Contains(Name)) VariableNames.Add(Name);
	VariableDisplayValues.Add(Name, Default);
	VariableTypes.Add(Name, EPEVarType::String);
}

void UCppPETabViewModel::RegisterIntVar(const FString& Name, int32 Default)
{
	GetCT().DefineIntVariable(Name, Default);
	if (!VariableNames.Contains(Name)) VariableNames.Add(Name);
	VariableDisplayValues.Add(Name, FString::FromInt(Default));
	VariableTypes.Add(Name, EPEVarType::Int);
}

void UCppPETabViewModel::RegisterInt64Var(const FString& Name, int64 Default)
{
	GetCT().DefineInt64Variable(Name, Default);
	if (!VariableNames.Contains(Name)) VariableNames.Add(Name);
	VariableDisplayValues.Add(Name, FString::Printf(TEXT("%lld"), Default));
	VariableTypes.Add(Name, EPEVarType::Int64);
}

void UCppPETabViewModel::RegisterFloatVar(const FString& Name, float Default)
{
	GetCT().DefineFloatVariable(Name, Default);
	if (!VariableNames.Contains(Name)) VariableNames.Add(Name);
	VariableDisplayValues.Add(Name, FString::SanitizeFloat(Default));
	VariableTypes.Add(Name, EPEVarType::Float);
}

void UCppPETabViewModel::RegisterDoubleVar(const FString& Name, double Default)
{
	GetCT().DefineDoubleVariable(Name, Default);
	if (!VariableNames.Contains(Name)) VariableNames.Add(Name);
	VariableDisplayValues.Add(Name, FString::Printf(TEXT("%g"), Default));
	VariableTypes.Add(Name, EPEVarType::Double);
}

void UCppPETabViewModel::RegisterBoolVar(const FString& Name, bool Default)
{
	GetCT().DefineBoolVariable(Name, Default);
	if (!VariableNames.Contains(Name)) VariableNames.Add(Name);
	VariableDisplayValues.Add(Name, Default ? TEXT("true") : TEXT("false"));
	VariableTypes.Add(Name, EPEVarType::Bool);
}

void UCppPETabViewModel::RegisterStringMapVar(const FString& Name, const TMap<FString, FString>& Default)
{
	GetCT().DefineStringMapVariable(Name, Default);
	if (!VariableNames.Contains(Name)) VariableNames.Add(Name);
	TArray<FString> Pairs;
	for (const auto& KV : Default) Pairs.Add(KV.Key + TEXT(": ") + KV.Value);
	VariableDisplayValues.Add(Name, TEXT("{") + FString::Join(Pairs, TEXT(", ")) + TEXT("}"));
	VariableTypes.Add(Name, EPEVarType::StringMap);
}

void UCppPETabViewModel::RegisterFileVar(const FString& Name)
{
	GetCT().DefineFileVariable(Name);
	if (!VariableNames.Contains(Name)) VariableNames.Add(Name);
	VariableDisplayValues.Add(Name, TEXT("(not downloaded)"));
	VariableTypes.Add(Name, EPEVarType::File);
}

// ---------------------------------------------------------------------------
// Interface implementations — the 4 action buttons
// ---------------------------------------------------------------------------

void UCppPETabViewModel::DefineVariables_Implementation()
{
	if (bDefined)
	{
		UE_LOG(LogCleverTapSample, Log, TEXT("PE: Variables already defined — skipping re-definition."));
		return;
	}

	// ---- Define all demo variables, mirroring the Unity SDK demo ----
	RegisterStringVar(TEXT("var_string"),          TEXT("Hello Unreal!"));
	RegisterIntVar   (TEXT("ue_var_int"),             10);
	RegisterInt64Var (TEXT("var_long"),            1000000000LL);
	RegisterIntVar   (TEXT("var_short"),           5);   // short → int on backend
	RegisterIntVar   (TEXT("var_byte"),            255); // byte  → int on backend
	RegisterFloatVar (TEXT("var_float"),           1.5f);
	RegisterDoubleVar(TEXT("var_double"),          3.14159265358979);
	RegisterBoolVar  (TEXT("var_bool"),            true);
	RegisterStringMapVar(TEXT("ue_var_dict"),
		TMap<FString,FString>{ {TEXT("key1"), TEXT("value1")}, {TEXT("key2"), TEXT("value2")} });
	RegisterStringVar(TEXT("ue.var.group.hello"),     TEXT("world"));
	RegisterStringVar(TEXT("ue.var.group.foo"),       TEXT("bar"));
	RegisterStringVar(TEXT("ue.android.samsung.s1"),  TEXT("samsung_val1"));
	RegisterFileVar  (TEXT("var_file"));

	Edit([](MutableContext& Ctx, UCppPETabViewModel& VM) {
		VM.SetVariablesDefined(Ctx, true);
		VM.SetFetchStatus(Ctx, TEXT("Variables defined — ready to Sync or Fetch"));
	});

	UE_LOG(LogCleverTapSample, Log, TEXT("PE: Defined %d variables."), VariableNames.Num());
}

void UCppPETabViewModel::SyncVariables_Implementation()
{
	GetCT().SyncVariables();
	Edit([](MutableContext& Ctx, UCppPETabViewModel& VM) {
		VM.SetFetchStatus(Ctx, TEXT("Synced definitions to dashboard"));
	});
	UE_LOG(LogCleverTapSample, Log, TEXT("PE: SyncVariables called — definitions pushed to dashboard."));
}

void UCppPETabViewModel::FetchVariables_Implementation()
{
	Edit([](MutableContext& Ctx, UCppPETabViewModel& VM) {
		VM.SetFetchStatus(Ctx, TEXT("Fetching..."));
	});
	GetCT().FetchVariables();
}

void UCppPETabViewModel::PrintVariables_Implementation()
{
	UE_LOG(LogCleverTapSample, Log, TEXT("PE: ---- Variable Values ----"));
	for (const FString& Name : VariableNames)
	{
		const FString* Val = VariableDisplayValues.Find(Name);
		UE_LOG(LogCleverTapSample, Log, TEXT("PE:   %s = %s"), *Name, Val ? **Val : TEXT("(undefined)"));
	}
	UE_LOG(LogCleverTapSample, Log, TEXT("PE: ---------------------------"));
	UE_LOG(LogCleverTapSample, Log, TEXT("PE: Status: %s"), *FetchStatus);

	// Also update the status line so it's visible in-app
	Edit([](MutableContext& Ctx, UCppPETabViewModel& VM) {
		VM.SetFetchStatus(Ctx, TEXT("Values printed to log"));
	});
}

// ---------------------------------------------------------------------------
// Per-variable increment / decrement
// ---------------------------------------------------------------------------

void UCppPETabViewModel::IncrementVariable_Implementation(const FString& Name)
{
	const EPEVarType* TypePtr = VariableTypes.Find(Name);
	if (!TypePtr) return;

	FString* Current = VariableDisplayValues.Find(Name);
	if (!Current) return;

	FString NewValue;
	switch (*TypePtr)
	{
		case EPEVarType::Int:
		case EPEVarType::Int64:
			NewValue = FString::Printf(TEXT("%lld"), FCString::Atoi64(**Current) + 1);
			break;
		case EPEVarType::Float:
			NewValue = FString::SanitizeFloat(FCString::Atof(**Current) + 1.0f);
			break;
		case EPEVarType::Double:
			NewValue = FString::Printf(TEXT("%g"), FCString::Atod(**Current) + 1.0);
			break;
		case EPEVarType::Bool:
			NewValue = (*Current == TEXT("true")) ? TEXT("false") : TEXT("true");
			break;
		default:
			return; // string, map, file — no numeric increment
	}

	Edit([&Name, &NewValue](MutableContext& Ctx, UCppPETabViewModel& VM) {
		VM.SetVariableDisplayValue(Ctx, Name, NewValue);
	});
}

void UCppPETabViewModel::DecrementVariable_Implementation(const FString& Name)
{
	const EPEVarType* TypePtr = VariableTypes.Find(Name);
	if (!TypePtr) return;

	FString* Current = VariableDisplayValues.Find(Name);
	if (!Current) return;

	FString NewValue;
	switch (*TypePtr)
	{
		case EPEVarType::Int:
		case EPEVarType::Int64:
			NewValue = FString::Printf(TEXT("%lld"), FCString::Atoi64(**Current) - 1);
			break;
		case EPEVarType::Float:
			NewValue = FString::SanitizeFloat(FCString::Atof(**Current) - 1.0f);
			break;
		case EPEVarType::Double:
			NewValue = FString::Printf(TEXT("%g"), FCString::Atod(**Current) - 1.0);
			break;
		case EPEVarType::Bool:
			NewValue = (*Current == TEXT("true")) ? TEXT("false") : TEXT("true");
			break;
		default:
			return;
	}

	Edit([&Name, &NewValue](MutableContext& Ctx, UCppPETabViewModel& VM) {
		VM.SetVariableDisplayValue(Ctx, Name, NewValue);
	});
}

// ---------------------------------------------------------------------------
// Queries
// ---------------------------------------------------------------------------

FString UCppPETabViewModel::GetVariableDisplayValue_Implementation(const FString& Name) const
{
	const FString* Found = VariableDisplayValues.Find(Name);
	return Found ? *Found : TEXT("(not defined)");
}

FString UCppPETabViewModel::GetFetchStatus_Implementation() const
{
	return FetchStatus;
}

TArray<FString> UCppPETabViewModel::GetVariableNames_Implementation() const
{
	return VariableNames;
}

bool UCppPETabViewModel::AreVariablesDefined_Implementation() const
{
	return bDefined;
}

// ---------------------------------------------------------------------------
// Type-aware display refresh — called when OnVariablesFetched / OnVariablesChanged fire
// ---------------------------------------------------------------------------

void UCppPETabViewModel::RefreshDisplayValues(MutableContext& Ctx)
{
	auto& CT = GetCT();
	for (const FString& Name : VariableNames)
	{
		const EPEVarType* TypePtr = VariableTypes.Find(Name);
		if (!TypePtr) continue;

		FString DisplayValue;
		switch (*TypePtr)
		{
			case EPEVarType::String:
				DisplayValue = CT.GetStringVariable(Name, VariableDisplayValues.FindRef(Name));
				break;
			case EPEVarType::Int:
				DisplayValue = FString::FromInt(CT.GetIntVariable(Name, 0));
				break;
			case EPEVarType::Int64:
				DisplayValue = FString::Printf(TEXT("%lld"), CT.GetInt64Variable(Name, 0));
				break;
			case EPEVarType::Float:
				// Use the double getter to avoid the double→float cast that introduces
				// precision noise (e.g. 67.3 becoming 67.300003). Float variables are
				// stored as double internally; reading them back as double preserves
				// whatever value the server actually sent.
				// %.8g = up to 8 significant digits, covers all practical float values.
				DisplayValue = FString::Printf(TEXT("%.8g"), CT.GetDoubleVariable(Name, 0.0));
				break;
			case EPEVarType::Double:
				DisplayValue = FString::Printf(TEXT("%.8g"), CT.GetDoubleVariable(Name, 0.0));
				break;
			case EPEVarType::Bool:
				DisplayValue = CT.GetBoolVariable(Name, false) ? TEXT("true") : TEXT("false");
				break;
			case EPEVarType::StringMap:
			{
				TMap<FString, FString> Map = CT.GetStringMapVariable(Name, {});
				TArray<FString> Pairs;
				for (const auto& KV : Map) Pairs.Add(KV.Key + TEXT(": ") + KV.Value);
				DisplayValue = TEXT("{") + FString::Join(Pairs, TEXT(", ")) + TEXT("}");
				break;
			}
			case EPEVarType::File:
				DisplayValue = CT.GetFileVariablePath(Name);
				if (DisplayValue.IsEmpty()) DisplayValue = TEXT("(not downloaded)");
				break;
		}
		SetVariableDisplayValue(Ctx, Name, DisplayValue);
	}
}

void UCppPETabViewModel::OnVariablesFetched(bool bSuccess)
{
	Edit([bSuccess, this](MutableContext& Ctx, UCppPETabViewModel& VM) {
		VM.SetFetchStatus(Ctx, bSuccess ? TEXT("Fetched successfully") : TEXT("Fetch failed"));
		if (bSuccess) VM.RefreshDisplayValues(Ctx);
	});
}

void UCppPETabViewModel::OnVariablesChanged()
{
	Edit([this](MutableContext& Ctx, UCppPETabViewModel& VM) {
		VM.RefreshDisplayValues(Ctx);
	});
}
