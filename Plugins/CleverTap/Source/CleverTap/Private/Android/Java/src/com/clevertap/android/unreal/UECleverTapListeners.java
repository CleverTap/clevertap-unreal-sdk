package com.clevertap.android.unreal;

import com.clevertap.android.sdk.PushPermissionResponseListener;
import com.clevertap.android.sdk.CleverTapAPI;

// Listener implementations that redirect the notifications to native methods implemented in C++ 
// that dispatch the notifcations on the main unreal game thread

public class UECleverTapListeners {
    public static class PushPermissionListener implements PushPermissionResponseListener {
        private final long nativeInstancePtr;

        public PushPermissionListener(long nativeInstancePtr) {
            this.nativeInstancePtr = nativeInstancePtr;
        }

        @Override
        public void onPushPermissionResponse(boolean granted) {
            nativeOnPushPermissionResponse(nativeInstancePtr, granted);
        }

        private static native void nativeOnPushPermissionResponse(long nativeInstancePtr, boolean granted);
    }
}
