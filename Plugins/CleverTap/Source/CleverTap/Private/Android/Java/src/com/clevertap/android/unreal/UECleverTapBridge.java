package com.clevertap.android.unreal;

import android.app.NotificationChannel;
import android.app.NotificationChannelGroup;
import android.app.NotificationManager;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import androidx.core.content.FileProvider;
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

    // Per-instance variable map: keyed by CleverTapAPI so multiple SDK instances stay isolated.
    private static ConcurrentHashMap<CleverTapAPI, ConcurrentHashMap<String, Var<?>>> sVariablesByInstance =
            new ConcurrentHashMap<>();

    // Per-instance guard: ensures VariablesChangedCallback is registered only once per instance.
    private static ConcurrentHashMap<CleverTapAPI, Boolean> sVariablesChangedCallbackRegistered =
            new ConcurrentHashMap<>();

    // Tracks the latest nativeInstancePtr per CleverTapAPI so the VariablesChangedCallback
    // always fires with the current pointer even if fetchVariables() is called multiple times.
    private static ConcurrentHashMap<CleverTapAPI, Long> sNativePtrByInstance =
            new ConcurrentHashMap<>();

    private static ConcurrentHashMap<String, Var<?>> getOrCreateVars(CleverTapAPI ct) {
        return sVariablesByInstance.computeIfAbsent(ct, k -> new ConcurrentHashMap<>());
    }

    public static void defineStringVariable(CleverTapAPI ct, String name, String defaultValue) {
        getOrCreateVars(ct).put(name, ct.defineVariable(name, defaultValue));
    }

    public static void defineIntVariable(CleverTapAPI ct, String name, int defaultValue) {
        getOrCreateVars(ct).put(name, ct.defineVariable(name, defaultValue));
    }

    public static void defineInt64Variable(CleverTapAPI ct, String name, long defaultValue) {
        getOrCreateVars(ct).put(name, ct.defineVariable(name, defaultValue));
    }

    public static void defineFloatVariable(CleverTapAPI ct, String name, double defaultValue) {
        getOrCreateVars(ct).put(name, ct.defineVariable(name, defaultValue));
    }

    public static void defineDoubleVariable(CleverTapAPI ct, String name, double defaultValue) {
        getOrCreateVars(ct).put(name, ct.defineVariable(name, defaultValue));
    }

    public static void defineBoolVariable(CleverTapAPI ct, String name, boolean defaultValue) {
        getOrCreateVars(ct).put(name, ct.defineVariable(name, defaultValue));
    }

    public static void defineStringMapVariable(CleverTapAPI ct, String name, Map<String, String> defaultValue) {
        // Cast to Map<String, Object> for the SDK
        Map<String, Object> objMap = new HashMap<>(defaultValue);
        getOrCreateVars(ct).put(name, ct.defineVariable(name, objMap));
    }

    public static void defineFileVariable(CleverTapAPI ct, String name) {
        getOrCreateVars(ct).put(name, ct.defineFileVariable(name));
    }

    public static void fetchVariables(CleverTapAPI ct, long nativeInstancePtr) {
        // Always update the pointer so the VariablesChangedCallback (registered once) fires
        // with the current native instance even if fetchVariables() is called again later.
        sNativePtrByInstance.put(ct, nativeInstancePtr);
        ct.fetchVariables(new FetchVariablesCallback() {
            @Override
            public void onVariablesFetched(boolean success) {
                nativeOnVariablesFetched(nativeInstancePtr, success);
            }
        });
        // Register the VariablesChangedCallback only once per instance — addVariablesChangedCallback
        // is additive, so calling this on every fetch would cause duplicate OnVariablesChanged events.
        if (sVariablesChangedCallbackRegistered.putIfAbsent(ct, Boolean.TRUE) == null) {
            ct.addVariablesChangedCallback(new VariablesChangedCallback() {
                @Override
                public void variablesChanged() {
                    Long ptr = sNativePtrByInstance.get(ct);
                    if (ptr != null) nativeOnVariablesChanged(ptr);
                }
            });
        }
    }

    public static String getStringVariable(CleverTapAPI ct, String name, String defaultValue) {
        Var<?> v = getOrCreateVars(ct).get(name);
        if (v == null) return defaultValue;
        Object val = v.value();
        return (val instanceof String) ? (String) val : defaultValue;
    }

    public static int getIntVariable(CleverTapAPI ct, String name, int defaultValue) {
        Var<?> v = getOrCreateVars(ct).get(name);
        if (v == null) return defaultValue;
        Object val = v.value();
        return (val instanceof Number) ? ((Number) val).intValue() : defaultValue;
    }

    public static long getInt64Variable(CleverTapAPI ct, String name, long defaultValue) {
        Var<?> v = getOrCreateVars(ct).get(name);
        if (v == null) return defaultValue;
        Object val = v.value();
        return (val instanceof Number) ? ((Number) val).longValue() : defaultValue;
    }

    public static double getFloatVariable(CleverTapAPI ct, String name, double defaultValue) {
        Var<?> v = getOrCreateVars(ct).get(name);
        if (v == null) return defaultValue;
        Object val = v.value();
        return (val instanceof Number) ? ((Number) val).doubleValue() : defaultValue;
    }

    public static double getDoubleVariable(CleverTapAPI ct, String name, double defaultValue) {
        Var<?> v = getOrCreateVars(ct).get(name);
        if (v == null) return defaultValue;
        Object val = v.value();
        return (val instanceof Number) ? ((Number) val).doubleValue() : defaultValue;
    }

    public static boolean getBoolVariable(CleverTapAPI ct, String name, boolean defaultValue) {
        Var<?> v = getOrCreateVars(ct).get(name);
        if (v == null) return defaultValue;
        Object val = v.value();
        return (val instanceof Boolean) ? (Boolean) val : defaultValue;
    }

    // Returns null when variable not found so the C++ caller falls back to its DefaultValue.
    @SuppressWarnings("unchecked")
    public static Map<String, String> getStringMapVariable(CleverTapAPI ct, String name) {
        Var<?> v = getOrCreateVars(ct).get(name);
        if (v == null) return null;
        Object val = v.value();
        if (!(val instanceof Map)) return null;
        Map<?, ?> raw = (Map<?, ?>) val;
        Map<String, String> result = new HashMap<>();
        for (Map.Entry<?, ?> entry : raw.entrySet()) {
            String k = entry.getKey() != null ? entry.getKey().toString() : "";
            String sv = entry.getValue() != null ? entry.getValue().toString() : "";
            result.put(k, sv);
        }
        return result;
    }

    // Returns null when variable not found so the C++ caller falls back to empty string.
    public static String getFileVariablePath(CleverTapAPI ct, String name) {
        Var<?> v = getOrCreateVars(ct).get(name);
        if (v == null) return null;
        Object val = v.value();
        return (val instanceof String) ? (String) val : null;
    }

    // Returns a JSON array string of active A/B test variant dictionaries.
    // Mirrors Unity's CleverTapUnityPlugin.getVariants().
    public static String getVariants(CleverTapAPI ct) {
        if (ct == null) return "[]";
        try {
            List<Map<String, Object>> variants = ct.variants();
            if (variants == null) return "[]";
            JSONArray array = new JSONArray();
            for (Map<String, Object> map : variants) {
                JSONObject obj = new JSONObject();
                for (Map.Entry<String, Object> entry : map.entrySet()) {
                    obj.put(entry.getKey(), entry.getValue());
                }
                array.put(obj);
            }
            return array.toString();
        } catch (Exception e) {
            return "[]";
        }
    }

    /**
     * Opens a local file using the device's default viewer app (e.g. gallery for images).
     * Uses FileProvider on Android 7+ so the receiving app can access the private-storage path.
     */
    public static void openFile(Context context, String filePath) {
        try {
            java.io.File file = new java.io.File(filePath);
            if (!file.exists()) {
                android.util.Log.e("CleverTap_UE", "openFile: file does not exist: " + filePath);
                return;
            }
            Uri uri;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
                // Resolve symlink (/data/user/0/ → /data/data/) so FileProvider path matching works
                java.io.File canonicalFile = file.getCanonicalFile();
                // Verify the file is within the app's private storage — reject path traversal attempts
                java.io.File appDataDir = context.getDataDir().getCanonicalFile();
                if (!canonicalFile.getPath().startsWith(appDataDir.getPath())) {
                    android.util.Log.e("CleverTap_UE", "openFile: path is outside app data dir, rejected: " + canonicalFile.getPath());
                    return;
                }
                uri = FileProvider.getUriForFile(context,
                    context.getPackageName() + ".clevertap.fileprovider", canonicalFile);
            } else {
                uri = Uri.fromFile(file);
            }
            // Detect MIME type from file content (CleverTap files have no extension)
            String mimeType = null;
            try {
                java.io.InputStream is = new java.io.BufferedInputStream(new java.io.FileInputStream(file));
                mimeType = java.net.URLConnection.guessContentTypeFromStream(is);
                is.close();
            } catch (Exception ignored) {}
            if (mimeType == null) {
                String ext = android.webkit.MimeTypeMap.getFileExtensionFromUrl(filePath);
                if (ext != null && !ext.isEmpty()) {
                    mimeType = android.webkit.MimeTypeMap.getSingleton().getMimeTypeFromExtension(ext);
                }
            }
            if (mimeType == null) mimeType = "image/*";
            Intent intent = new Intent(Intent.ACTION_VIEW);
            intent.setDataAndType(uri, mimeType);
            intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            context.startActivity(intent);
        } catch (Exception e) {
            android.util.Log.e("CleverTap_UE", "openFile: failed for " + filePath, e);
        }
    }

    // These are implemented on the C++ side
    private static native void nativeOnVariablesFetched(long nativeInstancePtr, boolean success);
    private static native void nativeOnVariablesChanged(long nativeInstancePtr);

}
