package com.github.biltudas1.dialsome;

import android.app.Activity;
import android.content.Context;
import android.util.Log; // Added for logging
import androidx.credentials.ClearCredentialStateRequest;
import androidx.credentials.CredentialManager;
import androidx.credentials.GetCredentialRequest;
import androidx.credentials.GetCredentialResponse;
import com.google.android.libraries.identity.googleid.GetGoogleIdOption;
import com.google.android.libraries.identity.googleid.GoogleIdTokenCredential;
import java.util.concurrent.Executor;
import androidx.core.content.ContextCompat;

public class GoogleLoginManager {
    private static final String TAG = "DialSomeLogin";

    // Native callback to C++
    public native void onLoginSuccess(String idToken, String email, String displayName, String userID);
    public native void onLoginError(String error);

    public void signIn(Context context, String webClientId, boolean autoLogin) {
        Log.d(TAG, "signIn called with clientID: " + webClientId);
        
        Activity activity = (Activity) context;
        CredentialManager credentialManager = CredentialManager.create(context);
        Executor executor = ContextCompat.getMainExecutor(context);

        GetGoogleIdOption googleIdOption = new GetGoogleIdOption.Builder()
                .setFilterByAuthorizedAccounts(autoLogin)
                .setServerClientId(webClientId)
                .setAutoSelectEnabled(autoLogin)
                .build();

        GetCredentialRequest request = new GetCredentialRequest.Builder()
                .addCredentialOption(googleIdOption)
                .build();

        Log.d(TAG, "Requesting credentials...");

        credentialManager.getCredentialAsync(context, request, null, executor, 
            new androidx.credentials.CredentialManagerCallback<GetCredentialResponse, androidx.credentials.exceptions.GetCredentialException>() {
            @Override
            public void onResult(GetCredentialResponse result) {
                androidx.credentials.Credential credential = result.getCredential();
                Log.d(TAG, "onResult triggered. Type: " + credential.getType());

                if (credential.getType().equals(GoogleIdTokenCredential.TYPE_GOOGLE_ID_TOKEN_CREDENTIAL)) {
                    try {
                        // Use the factory method to parse the data bundle
                        GoogleIdTokenCredential googleIdTokenCredential = 
                            GoogleIdTokenCredential.createFrom(credential.getData());
                        
                        Log.d(TAG, "Success: Extracted GoogleIdTokenCredential");
                        onLoginSuccess(
                            googleIdTokenCredential.getIdToken(),
                            googleIdTokenCredential.getEmail(),
                            googleIdTokenCredential.getDisplayName(),
                            googleIdTokenCredential.getUniqueId()
                        );
                    } catch (Exception e) {
                        Log.e(TAG, "Failed to parse Google ID Token: " + e.getMessage());
                        onLoginError("Parse error: " + e.getMessage());
                    }
                } else {
                    String errorMsg = "Unexpected credential type: " + credential.getType();
                    Log.e(TAG, errorMsg);
                    onLoginError(errorMsg);
                }
            }

            @Override
            public void onError(androidx.credentials.exceptions.GetCredentialException e) {
                Log.e(TAG, "CredentialManager Error: " + e.getMessage());
                onLoginError(e.getMessage());
            }
        });
    }
}