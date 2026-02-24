package com.github.biltudas1.dialsome;

import android.app.Activity;
import android.content.Context;
import androidx.credentials.ClearCredentialStateRequest;
import androidx.credentials.CredentialManager;
import androidx.credentials.GetCredentialRequest;
import androidx.credentials.GetCredentialResponse;
import com.google.android.libraries.identity.googleid.GetGoogleIdOption;
import com.google.android.libraries.identity.googleid.GoogleIdTokenCredential;
import java.util.concurrent.Executor;
import androidx.core.content.ContextCompat;

public class GoogleLoginManager {
    // Native callback to C++
    public native void onLoginSuccess(String idToken, String email, String displayName);
    public native void onLoginError(String error);

    public void signIn(Context context, String webClientId) {
        Activity activity = (Activity) context;
        CredentialManager credentialManager = CredentialManager.create(context);
        Executor executor = ContextCompat.getMainExecutor(context);

        GetGoogleIdOption googleIdOption = new GetGoogleIdOption.Builder()
                .setFilterByAuthorizedAccounts(false)
                .setServerClientId(webClientId)
                .build();

        GetCredentialRequest request = new GetCredentialRequest.Builder()
                .addCredentialOption(googleIdOption)
                .build();

        credentialManager.getCredentialAsync(context, request, null, executor, new androidx.credentials.CredentialManagerCallback<GetCredentialResponse, androidx.credentials.exceptions.GetCredentialException>() {
            @Override
            public void onResult(GetCredentialResponse result) {
                if (result.getCredential() instanceof GoogleIdTokenCredential) {
                    GoogleIdTokenCredential googleIdTokenCredential = (GoogleIdTokenCredential) result.getCredential();
                    onLoginSuccess(
                        googleIdTokenCredential.getIdToken(),
                        googleIdTokenCredential.getId(),
                        googleIdTokenCredential.getDisplayName()
                    );
                }
            }

            @Override
            public void onError(androidx.credentials.exceptions.GetCredentialException e) {
                onLoginError(e.getMessage());
            }
        });
    }
}