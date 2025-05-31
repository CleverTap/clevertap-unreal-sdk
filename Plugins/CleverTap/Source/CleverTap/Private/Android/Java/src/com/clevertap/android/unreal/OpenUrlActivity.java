package com.clevertap.android.unreal;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Bundle;

// A simple Activity that forwards to the normal Unreal GameActivity class;
// This simplifies registration of the intent filters in the manifest
public class OpenUrlActivity extends Activity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent sourceIntent = getIntent();
        Uri data = sourceIntent.getData();

        if (data != null) {
            // Forward the intent to GameActivity
            Intent forward = new Intent(this, findGameActivityClass());
            forward.setAction(Intent.ACTION_VIEW);
            forward.setData(data);
            forward.putExtras(sourceIntent);
            startActivity(forward);
        }

        finish();
    }

    @SuppressWarnings("unchecked")
    private static Class<? extends Activity> findGameActivityClass() {
        // UE5 first
        try {
            return (Class<? extends Activity>) Class.forName("com.epicgames.unreal.GameActivity");
        } catch (ClassNotFoundException ignored) {
        }

        // UE4 fallback
        try {
            return (Class<? extends Activity>) Class.forName("com.epicgames.ue4.GameActivity");
        } catch (ClassNotFoundException e) {
            throw new RuntimeException(
                    "Unable to locate Unreal GameActivity", e);
        }
    }

}
