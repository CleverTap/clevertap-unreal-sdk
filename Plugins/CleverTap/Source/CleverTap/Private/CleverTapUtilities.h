// Copyright CleverTap All Rights Reserved.
#pragma once

#include "Containers/UnrealString.h"
#include "Math/Color.h"

namespace CleverTapSDK {
namespace Details {
struct IgnoreImpl
{
	// clang-format off
	template <typename... Ts> constexpr void operator()(Ts&&...) const {}
	template <typename T> constexpr void operator=(T&&) const {}
	// clang-format on
};
} // namespace Details

/**
 * Used to ignore unused variables or return results.
 */
constexpr Details::IgnoreImpl Ignore{};

/**
 * Converts a FColor into a #RRGGBB formatted string
 */
FORCEINLINE FString ColorToHexString(const FColor& Color)
{
	return FString::Printf(TEXT("#%02X%02X%02X"), Color.R, Color.G, Color.B);
}

} // namespace CleverTapSDK
