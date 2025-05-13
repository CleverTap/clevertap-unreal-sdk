package com.clevertap.android.unreal;

import com.clevertap.android.sdk.CleverTapAPI;
import com.clevertap.android.sdk.inapp.CTInAppNotification;
import com.clevertap.android.sdk.InAppNotificationButtonListener;
import com.clevertap.android.sdk.InAppNotificationListener;
import com.clevertap.android.sdk.pushnotification.CTPushNotificationListener;
import com.clevertap.android.sdk.PushPermissionResponseListener;
import java.util.HashMap;
import java.util.Map;

// Listener implementation that redirects notifications to native methods implemented in C++ 
// The C++ methods then (typically) forward the notifcations to the unreal main game thread for dispatch

public class UECleverTapListener
        implements CTPushNotificationListener, PushPermissionResponseListener,
        InAppNotificationButtonListener, InAppNotificationListener {
    private final long nativeInstancePtr;

    public UECleverTapListener(long nativeInstancePtr) {
        this.nativeInstancePtr = nativeInstancePtr;
    }

    @Override
    public void onNotificationClickedPayloadReceived(HashMap<String, Object> notificationPayload) {
        nativeOnNotificationClicked(nativeInstancePtr, notificationPayload);
    }

    private static native void nativeOnNotificationClicked(long nativeInstancePtr, Object notificationPayload);

    @Override
    public void onPushPermissionResponse(boolean granted) {
        nativeOnPushPermissionResponse(nativeInstancePtr, granted);
    }

    private static native void nativeOnPushPermissionResponse(long nativeInstancePtr, boolean granted);

    // Called when an in-app notification is about to be rendered.
    // If you'd like this notification to not be rendered, then return false.
    @Override
    public boolean beforeShow(Map<String, Object> extras) {
        return nativeBeforeShowInAppNotification(nativeInstancePtr, extras);
    }

    private static native boolean nativeBeforeShowInAppNotification(long nativeInstancePtr, Object extras);

    // Called when an in-app notification is rendered.
    @Override
    public void onShow(CTInAppNotification ctInAppNotification) {
        nativeOnShowInAppNotification(nativeInstancePtr, ctInAppNotification);
    }

    private static native void nativeOnShowInAppNotification(long nativeInstancePtr, Object ctInAppNotification);

    // Called when an in-app notification is dismissed
    @Override
    public void onDismissed(Map<String, Object> extras, Map<String, Object> actionExtras) {
        nativeOnInAppNotificationDismissed(nativeInstancePtr, extras, actionExtras);
    }

    private static native void nativeOnInAppNotificationDismissed(long nativeInstancePtr, Object extras,
            Object actionExtras);

    // Called when a Key-Value pair button in an in-app notification is clicked
    @Override
    public void onInAppButtonClick(HashMap<String, String> payload) {
        nativeOnInAppButtonClick(nativeInstancePtr, payload);
    }

    private static native void nativeOnInAppButtonClick(long nativeInstancePtr, Object payload);

}
