// Copyright CleverTap All Rights Reserved.
//  CleverTapGameEvents.cpp
//  CleverTapSample

#include "CleverTapGameEvents.h"
#include "CleverTapSubsystem.h"
#include "CleverTapInstance.h"
#include "CleverTapProperties.h"
#include "Engine.h"

namespace
{
	UCleverTapInstance* GetCT()
	{
		auto* Sys = GEngine ? GEngine->GetEngineSubsystem<UCleverTapSubsystem>() : nullptr;
		return (Sys && Sys->IsSharedInstanceInitialized()) ? &Sys->SharedInstance() : nullptr;
	}
}

void UCleverTapGameEvents::RecordResourceEvent(
	ECleverTapResourceFlow Flow,
	const FString& Currency,
	float Amount,
	const FString& Category,
	const FString& ItemName)
{
	auto* CT = GetCT(); if (!CT) return;
	FCleverTapProperties Props;
	Props.Map.Add(TEXT("flow"),     FString(Flow == ECleverTapResourceFlow::Earned ? TEXT("Earned") : TEXT("Spent")));
	Props.Map.Add(TEXT("currency"), Currency);
	Props.Map.Add(TEXT("amount"),   Amount);
	Props.Map.Add(TEXT("category"), Category);
	Props.Map.Add(TEXT("item"),     ItemName);
	CT->PushEventWithProperties(TEXT("GameEvent_Resource"), Props);
}

void UCleverTapGameEvents::RecordLevelEvent(
	ECleverTapLevelStatus Status,
	const FString& World,
	const FString& Level,
	const FString& Stage,
	int32 Score)
{
	auto* CT = GetCT(); if (!CT) return;
	static const TCHAR* StatusStr[] = { TEXT("Started"), TEXT("Completed"), TEXT("Failed") };
	const uint8 StatusIdx = static_cast<uint8>(Status);
	if (StatusIdx >= UE_ARRAY_COUNT(StatusStr)) return;
	FCleverTapProperties Props;
	Props.Map.Add(TEXT("status"), FString(StatusStr[StatusIdx]));
	Props.Map.Add(TEXT("world"),  World);
	if (!Level.IsEmpty()) Props.Map.Add(TEXT("level"), Level);
	if (!Stage.IsEmpty()) Props.Map.Add(TEXT("stage"), Stage);
	if (Score != -1)      Props.Map.Add(TEXT("score"), Score);
	CT->PushEventWithProperties(TEXT("GameEvent_Level"), Props);
}

void UCleverTapGameEvents::RecordGameEvent(
	const FString& EventName,
	float Value)
{
	auto* CT = GetCT(); if (!CT) return;
	FCleverTapProperties Props;
	Props.Map.Add(TEXT("event"), EventName);
	if (Value != -1.0f) Props.Map.Add(TEXT("value"), Value);
	CT->PushEventWithProperties(TEXT("GameEvent_Design"), Props);
}

void UCleverTapGameEvents::RecordAdEvent(
	ECleverTapAdEvent Action,
	ECleverTapAdFormat Format,
	const FString& Placement)
{
	auto* CT = GetCT(); if (!CT) return;
	static const TCHAR* ActionStr[] = { TEXT("Shown"), TEXT("Clicked"), TEXT("Skipped"), TEXT("RewardGranted") };
	static const TCHAR* FormatStr[] = { TEXT(""), TEXT("Banner"), TEXT("Interstitial"), TEXT("RewardedVideo"), TEXT("OfferWall"), TEXT("Playable") };
	const uint8 ActionIdx = static_cast<uint8>(Action);
	const uint8 FormatIdx = static_cast<uint8>(Format);
	if (ActionIdx >= UE_ARRAY_COUNT(ActionStr)) return;
	if (FormatIdx >= UE_ARRAY_COUNT(FormatStr)) return;
	FCleverTapProperties Props;
	Props.Map.Add(TEXT("action"), FString(ActionStr[ActionIdx]));
	const FString Fmt = FormatStr[FormatIdx];
	if (!Fmt.IsEmpty())       Props.Map.Add(TEXT("format"),    Fmt);
	if (!Placement.IsEmpty()) Props.Map.Add(TEXT("placement"), Placement);
	CT->PushEventWithProperties(TEXT("GameEvent_Ad"), Props);
}

void UCleverTapGameEvents::RecordErrorEvent(
	ECleverTapErrorSeverity Severity,
	const FString& Description)
{
	auto* CT = GetCT(); if (!CT) return;
	static const TCHAR* SeverityStr[] = { TEXT("Low"), TEXT("Medium"), TEXT("High"), TEXT("Critical") };
	const uint8 SeverityIdx = static_cast<uint8>(Severity);
	if (SeverityIdx >= UE_ARRAY_COUNT(SeverityStr)) return;
	FCleverTapProperties Props;
	Props.Map.Add(TEXT("severity"),    FString(SeverityStr[SeverityIdx]));
	Props.Map.Add(TEXT("description"), Description);
	CT->PushEventWithProperties(TEXT("GameEvent_Error"), Props);
}
//
//  Created by Nilesh Sharma on 28/09/26.
//  Copyright © 2026 Epic Games, Inc. All rights reserved.
//

