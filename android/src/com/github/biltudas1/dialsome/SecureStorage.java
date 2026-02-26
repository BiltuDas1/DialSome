package com.github.biltudas1.dialsome;

import android.content.Context;
import androidx.datastore.preferences.core.MutablePreferences;
import androidx.datastore.preferences.core.Preferences;
import androidx.datastore.preferences.core.PreferencesKeys;
import androidx.datastore.preferences.rxjava3.RxPreferenceDataStoreBuilder;
import androidx.datastore.rxjava3.RxDataStore;

import com.google.crypto.tink.Aead;
import com.google.crypto.tink.KeyTemplates;
import com.google.crypto.tink.aead.AeadConfig;
import com.google.crypto.tink.integration.android.AndroidKeysetManager;

import java.nio.charset.StandardCharsets;
import java.util.Base64;
import io.reactivex.rxjava3.core.Single;

public class SecureStorage {
    private static final String KEYSET_NAME = "dialsome_master_keyset";
    private static final String PREF_FILE_NAME = "dialsome_tink_prefs";
    private static final String MASTER_KEY_URI = "android-keystore://dialsome_master_key";
    
    private final RxDataStore<Preferences> dataStore;
    private Aead aead;

    public SecureStorage(Context context) {
        // Initialize Dialsome DataStore
        dataStore = new RxPreferenceDataStoreBuilder(context, "dialsome_secure_prefs").build();
        
        try {
            AeadConfig.register();
            // This builds the hardware-backed bridge
            aead = new AndroidKeysetManager.Builder()
                    .withSharedPref(context, KEYSET_NAME, PREF_FILE_NAME)
                    .withKeyTemplate(KeyTemplates.get("AES256_GCM"))
                    .withMasterKeyUri(MASTER_KEY_URI)
                    .build()
                    .getKeysetHandle()
                    .getPrimitive(Aead.class);
        } catch (Exception e) {
            android.util.Log.e("Dialsome_Auth", "Failed to initialize Tink: " + e.getMessage());
        }
    }

    /**
     * Encrypts and saves data. 
     * Uses blockingGet() to ensure C++ waits until the write is finished.
     */
    public void saveData(String keyName, String plainText) {
        try {
            byte[] encrypted = aead.encrypt(plainText.getBytes(StandardCharsets.UTF_8), null);
            String base64Encrypted = Base64.getEncoder().encodeToString(encrypted);
            
            Preferences.Key<String> key = PreferencesKeys.stringKey(keyName);
            dataStore.updateDataAsync(prefsIn -> {
                MutablePreferences mutablePreferences = prefsIn.toMutablePreferences();
                mutablePreferences.set(key, base64Encrypted);
                return Single.just(mutablePreferences);
            }).blockingGet(); // Ensures data is written before JNI returns
            
        } catch (Exception e) {
            android.util.Log.e("Dialsome_Auth", "Save Error: " + e.getMessage());
        }
    }

    /**
     * Retrieves and decrypts data. 
     * Blocks until DataStore provides the first emission.
     */
    public String getData(String keyName) {
        try {
            Preferences.Key<String> key = PreferencesKeys.stringKey(keyName);
            Preferences prefs = dataStore.data().blockingFirst();
            String base64Encrypted = prefs.get(key);
            
            if (base64Encrypted == null || base64Encrypted.isEmpty()) return "";
            
            byte[] encrypted = Base64.getDecoder().decode(base64Encrypted);
            byte[] decrypted = aead.decrypt(encrypted, null);
            return new String(decrypted, StandardCharsets.UTF_8);
            
        } catch (Exception e) {
            android.util.Log.e("Dialsome_Auth", "Retrieve Error: " + e.getMessage());
            return "";
        }
    }

    /**
     * Wipes a specific key or all data. Use this for Logging out of Dialsome.
     */
    public void clearData() {
        dataStore.updateDataAsync(prefsIn -> {
            MutablePreferences mutablePreferences = prefsIn.toMutablePreferences();
            mutablePreferences.clear();
            return Single.just(mutablePreferences);
        }).blockingGet();
    }

    /**
     * Checks if a key name exist
     */
    public boolean hasKey(String keyName) {
        try {
            Preferences.Key<String> key = PreferencesKeys.stringKey(keyName);
            Preferences prefs = dataStore.data().blockingFirst();
            
            // contains() returns true if the key exists in the current Preferences
            return prefs.contains(key);
        } catch (Exception e) {
            return false;
        }
    }
}
