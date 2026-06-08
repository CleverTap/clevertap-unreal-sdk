package com.clevertap.android.unreal;

import android.app.NotificationChannel;
import android.app.NotificationChannelGroup;
import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import com.clevertap.android.sdk.CleverTapAPI;
import com.clevertap.android.sdk.inapp.CTLocalInApp;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import com.clevertap.android.sdk.variables.Var;
import com.clevertap.android.sdk.variables.callbacks.FetchVariablesCallback;
import com.clevertap.android.sdk.variables.callbacks.VariablesChangedCallback;
import java.util.concurrent.ConcurrentHashMap;

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
        if (name == null || name.length() == 0) {
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

    /// Converts a Java JSONObject to a flattened Map<String, Object>.
    //
    // Nested objects are flattened using dot-separated keys (e.g., "foo.bar").
    // Arrays are converted to ArrayList<String> where possible.
    // Preserves native Java types (String, Number, Boolean)
    public static Map<String, Object> jsonObjectToFlatMap(JSONObject json) throws JSONException {
        Map<String, Object> result = new HashMap<>();
        flattenJSONObject("", json, result);
        return result;
    }

    private static void flattenJSONObject(String prefix, JSONObject json, Map<String, Object> result)
            throws JSONException {
        Iterator<String> keys = json.keys();
        while (keys.hasNext()) {
            String key = keys.next();
            Object value = json.get(key);
            String fullKey = prefix.isEmpty() ? key : prefix + "." + key;

            if (value instanceof JSONObject) {
                flattenJSONObject(fullKey, (JSONObject) value, result); // Recurse
            } else if (value instanceof JSONArray) {
                result.put(fullKey, jsonArrayToStringList((JSONArray) value)); // ArrayList<String>
            } else {
                result.put(fullKey, value); // Preserve original type (String, Integer, Boolean, etc.)
            }
        }
    }

    private static ArrayList<String> jsonArrayToStringList(JSONArray array) throws JSONException {
        ArrayList<String> list = new ArrayList<>();
        for (int i = 0; i < array.length(); i++) {
            Object element = array.get(i);
            if (element instanceof String) {
                list.add((String) element);
            } else {
                list.add(String.valueOf(element)); // Safe coercion of scalars like numbers
            }
        }
        return list;
    }

    // ---- Product Experiences (Variables) ----

    // Stores defined variables by name so we can read their values later
    private static ConcurrentHashMap<String, Var<?>> sVariables = new ConcurrentHashMap<>();

    // Ensures the VariablesChangedCallback is only registered once, even if fetchVariables() is called multiple times
    private static volatile boolean sVariablesChangedCallbackRegistered = false;

    public static void defineStringVariable(CleverTapAPI ct, String name, String defaultValue) {
        sVariables.put(name, ct.defineVariable(name, defaultValue));
    }

    public static void defineIntVariable(CleverTapAPI ct, String name, int defaultValue) {
        sVariables.put(name, ct.defineVariable(name, defaultValue));
    }

    public static void defineInt64Variable(CleverTapAPI ct, String name, long defaultValue) {
        sVariables.put(name, ct.defineVariable(name, defaultValue));
    }

    public static void defineFloatVariable(CleverTapAPI ct, String name, double defaultValue) {
        sVariables.put(name, ct.defineVariable(name, defaultValue));
    }

    public static void defineDoubleVariable(CleverTapAPI ct, String name, double defaultValue) {
        sVariables.put(name, ct.defineVariable(name, defaultValue));
    }

    public static void defineBoolVariable(CleverTapAPI ct, String name, boolean defaultValue) {
        sVariables.put(name, ct.defineVariable(name, defaultValue));
    }

    public static void defineStringMapVariable(CleverTapAPI ct, String name, Map<String, String> defaultValue) {
        // Cast to Map<String, Object> for the SDK
        Map<String, Object> objMap = new HashMap<>(defaultValue);
        sVariables.put(name, ct.defineVariable(name, objMap));
    }

    public static void defineFileVariable(CleverTapAPI ct, String name) {
        sVariables.put(name, ct.defineFileVariable(name));
    }

    public static void fetchVariables(CleverTapAPI ct, long nativeInstancePtr) {
        ct.fetchVariables(new FetchVariablesCallback() {
            @Override
            public void onVariablesFetched(boolean success) {
                nativeOnVariablesFetched(nativeInstancePtr, success);
            }
        });
        // Register the VariablesChangedCallback only once — addVariablesChangedCallback is additive,
        // so calling this on every fetch would cause duplicate OnVariablesChanged events.
        if (!sVariablesChangedCallbackRegistered) {
            sVariablesChangedCallbackRegistered = true;
            ct.addVariablesChangedCallback(new VariablesChangedCallback() {
                @Override
                public void variablesChanged() {
                    nativeOnVariablesChanged(nativeInstancePtr);
                }
            });
        }
    }

    public static String getStringVariable(String name, String defaultValue) {
        Var<?> v = sVariables.get(name);
        if (v == null) return defaultValue;
        Object val = v.value();
        return (val instanceof String) ? (String) val : defaultValue;
    }

    public static int getIntVariable(String name, int defaultValue) {
        Var<?> v = sVariables.get(name);
        if (v == null) return defaultValue;
        Object val = v.value();
        return (val instanceof Number) ? ((Number) val).intValue() : defaultValue;
    }

    public static long getInt64Variable(String name, long defaultValue) {
        Var<?> v = sVariables.get(name);
        if (v == null) return defaultValue;
        Object val = v.value();
        return (val instanceof Number) ? ((Number) val).longValue() : defaultValue;
    }

    public static double getFloatVariable(String name, double defaultValue) {
        Var<?> v = sVariables.get(name);
        if (v == null) return defaultValue;
        Object val = v.value();
        return (val instanceof Number) ? ((Number) val).doubleValue() : defaultValue;
    }

    public static double getDoubleVariable(String name, double defaultValue) {
        Var<?> v = sVariables.get(name);
        if (v == null) return defaultValue;
        Object val = v.value();
        return (val instanceof Number) ? ((Number) val).doubleValue() : defaultValue;
    }

    public static boolean getBoolVariable(String name, boolean defaultValue) {
        Var<?> v = sVariables.get(name);
        if (v == null) return defaultValue;
        Object val = v.value();
        return (val instanceof Boolean) ? (Boolean) val : defaultValue;
    }

    @SuppressWarnings("unchecked")
    public static Map<String, String> getStringMapVariable(String name) {
        Var<?> v = sVariables.get(name);
        if (v == null) return new HashMap<>();
        Object val = v.value();
        if (!(val instanceof Map)) return new HashMap<>();
        Map<?, ?> raw = (Map<?, ?>) val;
        Map<String, String> result = new HashMap<>();
        for (Map.Entry<?, ?> entry : raw.entrySet()) {
            String k = entry.getKey() != null ? entry.getKey().toString() : "";
            String sv = entry.getValue() != null ? entry.getValue().toString() : "";
            result.put(k, sv);
        }
        return result;
    }

    public static String getFileVariablePath(String name) {
        Var<?> v = sVariables.get(name);
        if (v == null) return "";
        Object val = v.value();
        return (val instanceof String) ? (String) val : "";
    }

    // These are implemented on the C++ side
    private static native void nativeOnVariablesFetched(long nativeInstancePtr, boolean success);
    private static native void nativeOnVariablesChanged(long nativeInstancePtr);

}
