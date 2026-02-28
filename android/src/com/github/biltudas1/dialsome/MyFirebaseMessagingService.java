package com.github.biltudas1.dialsome;

import android.util.Log;
import com.google.firebase.messaging.FirebaseMessagingService;
import com.google.firebase.messaging.RemoteMessage;
import java.util.Map;

public class MyFirebaseMessagingService extends FirebaseMessagingService {
    private static final String TAG = "DialSomeFCM";

    // Native method to notify the C++ Backend that a message arrived
    public native void onFCMMessageReceived(String type, String sdp, String email);
    private native void notifyNewToken(String token);

    /**
     * Called when a new token is generated.
     */
    @Override
    public void onNewToken(String token) {
        super.onNewToken(token);
        notifyNewToken(token);
        // In a real scenario, you'd send this to your /users/update-fcm endpoint
    }

    /**
     * Called when a message is received from the FastAPI backend.
     */
    @Override
    public void onMessageReceived(RemoteMessage remoteMessage) {
        super.onMessageReceived(remoteMessage);

        // Check if message contains a data payload (your FastAPI sends 'data')
        if (remoteMessage.getData().size() > 0) {
            Map<String, String> data = remoteMessage.getData();
            String type = data.get("type"); // "offer" or "answer"
            String sdp = data.get("sdp");
            String callerEmail = data.get("caller_email");

            Log.d(TAG, "FCM Signal Received: " + type + " from " + callerEmail);

            // 1. Show a toast using your existing utility
            AndroidUtils.showToast(null, "Call signal (" + type + ") from: " + callerEmail);

            // 2. Notify the C++ layer via JNI
            // Note: You must define this native method in your backend.cpp
            onFCMMessageReceived(type, sdp, callerEmail);
        }
    }
}