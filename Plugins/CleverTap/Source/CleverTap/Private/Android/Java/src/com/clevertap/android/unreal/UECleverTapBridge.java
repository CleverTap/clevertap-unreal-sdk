package com.clevertap.android.unreal;

import com.clevertap.android.sdk.inapp.CTLocalInApp;
import java.util.Map;
import org.json.JSONObject;

// Utilities to make it easier to use the CleverTapAPI from Unreal/C++ 
public class UECleverTapBridge {

    public static JSONObject buildPushPrimerAlertConfig(Map<String, Object> params) {
        return CTLocalInApp.builder()
                .setInAppType(CTLocalInApp.InAppType.ALERT)
                .setTitleText((String) params.get("TitleText"))
                .setMessageText((String) params.get("MessageText"))
                .followDeviceOrientation((boolean) params.get("FollowDeviceOrientation"))
                .setPositiveBtnText((String) params.get("PositiveButtonText"))
                .setNegativeBtnText((String) params.get("NegativeButtonText"))
                .setFallbackToSettings((boolean) params.get("FallbackToSettings"))
                .build();
    }

    public static JSONObject buildPushPrimerHalfInterstitialConfig(Map<String, Object> params) {
        // unfortunately this builder doesn't allow conditional breaks in the chain,
        // so we have to set all parameters & all must be provided
        return CTLocalInApp.builder()
                .setInAppType(CTLocalInApp.InAppType.HALF_INTERSTITIAL)
                .setTitleText((String) params.get("TitleText"))
                .setMessageText((String) params.get("MessageText"))
                .followDeviceOrientation((boolean) params.get("FollowDeviceOrientation"))
                .setPositiveBtnText((String) params.get("PositiveButtonText"))
                .setNegativeBtnText((String) params.get("NegativeButtonText"))
                .setFallbackToSettings((boolean) params.get("FallbackToSettings"))
                .setImageUrl((String) params.getOrDefault("ImageURL", ""))
                .setBackgroundColor((String) params.get("BackgroundColor"))
                .setBtnBorderColor((String) params.get("ButtonBorderColor"))
                .setTitleTextColor((String) params.get("TitleTextColor"))
                .setMessageTextColor((String) params.get("MessageTextColor"))
                .setBtnTextColor((String) params.get("ButtonTextColor"))
                .setBtnBackgroundColor((String) params.get("ButtonBackgroundColor"))
                .setBtnBorderRadius((String) params.get("ButtonBorderRadius"))
                .build();
    }

}
