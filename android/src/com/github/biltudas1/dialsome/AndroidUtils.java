package com.github.biltudas1.dialsome;

import android.app.Activity;
import android.widget.Toast;

public class AndroidUtils {
    /**
     * Shows a native Android Toast message.
     * This must be called from the UI thread.
     */
    public static void showToast(Activity activity, String message) {
        if (activity == null) return;
        
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(activity, message, Toast.LENGTH_SHORT).show();
            }
        });
    }
}