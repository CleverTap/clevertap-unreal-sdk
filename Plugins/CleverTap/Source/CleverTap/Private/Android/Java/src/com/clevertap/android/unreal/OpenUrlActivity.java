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
            Intent forward = new Intent(this, com.epicgames.ue4.GameActivity.class);
            forward.setAction(Intent.ACTION_VIEW);
            forward.setData(data);
            forward.putExtras(sourceIntent);
            startActivity(forward);
        }

        finish();
    }
}
