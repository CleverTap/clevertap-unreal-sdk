// Copyright CleverTap All Rights Reserved.

#pragma once

class UCleverTapConfig;
class FCleverTapInstance;

namespace CleverTapSDK { namespace GenericPlatform {

class FPlatformInstance
{ };

struct FGenericPlatformSDK
{
	/**
	 * Initialize the common (shared) CleverTap instance. If successful this returns a non-null
	 *  instance object. This instance object can be used to call other SDK methods. It should
	 *  eventually be destroyed using the DestroyInstance(FCleverTapInstance&) method.
	 */
	static FCleverTapInstance* TryInitializeCommonInstance(const UCleverTapConfig& Config);

	/**
	 * Shutdown the specified CleverTap instance. After calling this method the specified instance
	 *  should not be used in other method calls.
	 */
	static void DestroyInstance(FCleverTapInstance& Inst);
};

} } // namespace CleverTapSDK::GenericPlatform

