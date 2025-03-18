// Copyright CleverTap All Rights Reserved.
#pragma once

namespace CleverTapSDK {
namespace Details {
struct IgnoreImpl
{
	template <typename... Ts> constexpr void operator()(Ts&&...) const {}
	template <typename T> constexpr void operator=(T&&) const {}
};
} // namespace Details

/**
 * Used to ignore unused variables or return results.
 */
constexpr Details::IgnoreImpl Ignore{};
} // namespace CleverTapSDK
