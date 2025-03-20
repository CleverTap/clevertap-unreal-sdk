// Copyright CleverTap All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

using FCleverTapProfileValue = TVariant<int64, float, bool, FString, FDateTime, TArray<FString>>;
using FCleverTapProfile = TMap<FString, FCleverTapProfileValue>;
