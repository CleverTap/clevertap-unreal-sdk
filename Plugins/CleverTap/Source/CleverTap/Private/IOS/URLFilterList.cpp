#include "IOS/URLFilterList.h"

#include "CleverTapConfig.h"
#include "CleverTapLog.h"

namespace CleverTapSDK { namespace IOS {

FURLFilterList::FURLFilterList(const UCleverTapConfig& Config)
{
	Requirements.Reserve(4);
	Requirements.Add(FRequiredParts{
		Config.DeepLinkSchemeFilterSlot1, Config.DeepLinkHostFilterSlot1, Config.DeepLinkPathPrefixFilterSlot1 });
	Requirements.Add(FRequiredParts{
		Config.DeepLinkSchemeFilterSlot2, Config.DeepLinkHostFilterSlot2, Config.DeepLinkPathPrefixFilterSlot2 });
	Requirements.Add(FRequiredParts{
		Config.DeepLinkSchemeFilterSlot3, Config.DeepLinkHostFilterSlot3, Config.DeepLinkPathPrefixFilterSlot3 });
	Requirements.Add(FRequiredParts{
		Config.DeepLinkSchemeFilterSlot4, Config.DeepLinkHostFilterSlot4, Config.DeepLinkPathPrefixFilterSlot4 });

	Requirements.RemoveAllSwap([](const FRequiredParts& Parts) {
		return Parts.Scheme.IsEmpty() && Parts.HostName.IsEmpty() && Parts.PathPrefix.IsEmpty();
	});
}

bool FURLFilterList::IsFilteredURL(const FString& URL) const
{
	UE_LOG(LogCleverTap, Log, TEXT("IsFilteredURL(%s)"), *URL);

	if (Requirements.Num() == 0)
	{
		return false;
	}

	if (URL.IsEmpty())
	{
		UE_LOG(LogCleverTap, Warning, TEXT("IsFilteredURL() - Filtering an empty URL"));
		return true;
	}

	FStringView URLView{ URL };

	// First remove the scheme:// part of the URL if present
	const FStringView Scheme = [URLView] {
		int32 SchemeSplitIdx{};
		if (URLView.FindChar(TCHAR{ ':' }, SchemeSplitIdx))
		{
			check(URLView.SubStr(SchemeSplitIdx, 3) == FStringView{ TEXT("://") });
			return URLView.Left(SchemeSplitIdx);
		}

		return FStringView{};
	}();
	if (Scheme.IsEmpty())
	{
		UE_LOG(LogCleverTap, Warning,
			TEXT("IsFilteredURL() - URL '%s' is missing a required scheme:// element and is being filtered"), *URL);
		return true;
	}

	URLView.RemovePrefix(Scheme.Len() + 3);

	// Find and adjust for the host name part
	const FStringView HostName = [URLView] {
		int32 UrlSepIdx{};
		if (URLView.FindChar(TCHAR{ '/' }, UrlSepIdx))
		{
			return URLView.SubStr(0, UrlSepIdx);
		}
		return URLView;
	}();

	// Adjust the view again, intentionally leaving the '/' path separator if there is one
	check(!HostName.IsEmpty());
	URLView.RemovePrefix(HostName.Len());

	const FStringView RemainingPath{ URLView };
	UE_LOG(LogCleverTap, Log,
		TEXT("IsFilteredURL() - Matching URL parts {scheme: \"%s\", hostname: \"%s\", prefix: \"%s\"}"),
		*FString{ Scheme }, *FString{ HostName }, *FString{ RemainingPath });

	for (const FRequiredParts& Req : Requirements)
	{
		const bool bMeetsSchemeReq = Req.Scheme == Scheme;
		const bool bMeetsHostNameReq = Req.HostName.IsEmpty() || Req.HostName == HostName;
		const bool bMeetsPrefixReq = RemainingPath.IsEmpty() || RemainingPath.StartsWith(Req.PathPrefix);
		if (bMeetsSchemeReq && bMeetsHostNameReq && bMeetsPrefixReq)
		{
			UE_LOG(LogCleverTap, Log,
				TEXT("IsFilteredURL() - URL '%s' matches {scheme: \"%s\", hostname: \"%s\", prefix: \"%s\"}"), *URL,
				*Req.Scheme, *Req.HostName, *Req.PathPrefix);
			return false;
		}

		UE_LOG(LogCleverTap, Log,
			TEXT(
				"IsFilteredURL() - URL '%s' does not match requirements {scheme: \"%s\", hostname: \"%s\", prefix: \"%s\"}"),
			*URL, *Req.Scheme, *Req.HostName, *Req.PathPrefix);
	}

	// Requirements were specified and this URL met none of them
	return true;
}

}} // namespace CleverTapSDK::IOS
