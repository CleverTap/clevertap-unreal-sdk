package com.clevertap.android.unreal;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;
import android.os.Build;
import com.clevertap.android.sdk.inapp.CTLocalInApp;
import java.util.Map;
import org.json.JSONObject;

// Utilities to make it easier to use the CleverTapAPI from Unreal/C++ 
public class UECleverTapBridge {

    // Update the name and description for an existing android notification channel
    // Returns true on success, false if no such channel or incompatible API.
    public static boolean localizeNotificationChannel(Context context, String channelId, String name,
            String description) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            // incompatible build!
            return false;
        }
        NotificationManager nm = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        NotificationChannel existing = nm.getNotificationChannel(channelId);
        if (existing == null) {
            // no such channel
            return false;
        }
        existing.setName(name);
        existing.setDescription(description);
        // Re-register to apply name/description changes
        nm.createNotificationChannel(existing);
        return true;
    }

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
