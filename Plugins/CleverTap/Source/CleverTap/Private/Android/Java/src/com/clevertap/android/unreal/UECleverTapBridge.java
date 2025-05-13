package com.clevertap.android.unreal;

import android.app.NotificationChannel;
import android.app.NotificationChannelGroup;
import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import com.clevertap.android.sdk.CleverTapAPI;
import com.clevertap.android.sdk.inapp.CTLocalInApp;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import org.json.JSONException;
import org.json.JSONObject;

// Utilities to make it easier to use the CleverTapAPI from Unreal/C++ 
public class UECleverTapBridge {
    private static String pendingIntentUri = null;
    private static boolean areIntentNotificationsEnabled = false;

    public static void onNewIntent(Intent intent) {
        android.net.Uri intentData = intent.getData();
        if (intentData == null) {
            return;
        }
        String uri = intentData.toString();
        if (areIntentNotificationsEnabled) {
            nativeOnOpenUrl(uri);
        } else {
            pendingIntentUri = uri;
        }
    }

    public static void enableIntentNotifications() {
        areIntentNotificationsEnabled = true;
        if (pendingIntentUri != null) {
            nativeOnOpenUrl(pendingIntentUri);
            pendingIntentUri = null;
        }
    }

    // Implemented on the Unreal side
    private static native void nativeOnOpenUrl(String uri);

    // Create a notification channel using the appropriate overload & defaults.
    // Needs to happen during onCreate() to be reliable.
    public static void createNotificationChannel(Context context, String channelId,
            CharSequence name, String description, int importance, String groupId,
            boolean showBadge, String sound) {

        // default the name to the id if none provided
        if (name == null || name.isEmpty()) {
            name = channelId;
        }

        if (groupId != null && !groupId.isEmpty()) {
            if (sound != null && !sound.isEmpty()) {
                // create channel assigned to group with sound
                CleverTapAPI.createNotificationChannel(context, channelId, name, description, importance, groupId,
                        showBadge, sound);
            } else {
                // create channel assigned to group
                CleverTapAPI.createNotificationChannel(context, channelId, name, description, importance, groupId,
                        showBadge);
            }
        } else {
            if (sound != null && !sound.isEmpty()) {
                // create channel with sound and no group
                CleverTapAPI.createNotificationChannel(context, channelId, name, description, importance, showBadge,
                        sound);
            } else {
                // create channel with no group
                CleverTapAPI.createNotificationChannel(context, channelId, name, description, importance, showBadge);
            }
        }
    }

    // Find the first channel with this channelID, ignoring groupId!
    // Returns null if no such channel found.
    //
    // This function is required because
    // NotificationManager.getNotificationChannel(context,channelID)
    // doesn't work for grouped channels!
    public static NotificationChannel getNotificationChannel(Context context, String channelId) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            // incompatible build; no channel support
            return null;
        }
        NotificationManager nm = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        for (NotificationChannel channel : nm.getNotificationChannels()) {
            if (channel.getId().equals(channelId)) {
                // found it
                return channel;
            }
        }
        // no such channel
        return null;
    }

    // Update the name and description for an existing android notification channel
    // Returns true on success, false if no such channel or incompatible API.
    public static boolean localizeNotificationChannel(Context context, String channelId, String name,
            String description) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            // incompatible build; no channel support
            return false;
        }
        NotificationChannel existing = getNotificationChannel(context, channelId);
        if (existing == null) {
            // no such channel
            return false;
        }
        existing.setName(name);
        existing.setDescription(description);
        // Re-register to apply name/description changes
        NotificationManager nm = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        nm.createNotificationChannel(existing);
        return true;
    }

    public static boolean localizeNotificationChannelGroup(Context context, String groupId, String name) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            // incompatible build; no channel support
            return false;
        }
        NotificationManager nm = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        NotificationChannelGroup group = nm.getNotificationChannelGroup(groupId);
        if (group == null) {
            // no such group
            return false;
        }
        // re-create with the new name; there's no way to update in place
        CleverTapAPI.createNotificationChannelGroup(context, groupId, name);
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

    public static Map<String, Object> jsonObjectToMap(JSONObject json) throws JSONException {
        Map<String, Object> map = new HashMap<>();
        Iterator<String> keys = json.keys();
        while (keys.hasNext()) {
            String key = keys.next();
            Object value = json.get(key);
            // todo flatten nested dictionaries
            map.put(key, value);
        }
        return map;
    }

}
