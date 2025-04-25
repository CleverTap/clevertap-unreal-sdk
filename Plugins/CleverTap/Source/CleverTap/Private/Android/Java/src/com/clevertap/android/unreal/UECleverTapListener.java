package com.clevertap.android.unreal;

import com.clevertap.android.sdk.CleverTapAPI;
import com.clevertap.android.sdk.pushnotification.CTPushNotificationListener;
import com.clevertap.android.sdk.PushPermissionResponseListener;
import java.util.HashMap;

// Listener implementation that redirects notifications to native methods implemented in C++ 
// The C++ methods then forward the notifcations to the unreal main game thread for dispatch

public class UECleverTapListener implements PushPermissionResponseListener, CTPushNotificationListener {
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
}
