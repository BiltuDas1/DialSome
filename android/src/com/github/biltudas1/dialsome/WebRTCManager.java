package com.github.biltudas1.dialsome;

import android.content.Context;
import android.media.AudioAttributes;
import android.media.AudioFocusRequest;
import android.media.AudioManager;

import org.webrtc.*;

import java.util.Collections;

public class WebRTCManager {

    private PeerConnectionFactory factory;
    private PeerConnection peerConnection;

    public void init(Context context) {

        // Modern audio focus API
        AudioManager am =
            (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);

        AudioAttributes attrs = new AudioAttributes.Builder()
            .setUsage(AudioAttributes.USAGE_VOICE_COMMUNICATION)
            .setContentType(AudioAttributes.CONTENT_TYPE_SPEECH)
            .build();

        AudioFocusRequest afr = new AudioFocusRequest.Builder(
            AudioManager.AUDIOFOCUS_GAIN_TRANSIENT_EXCLUSIVE)
            .setAudioAttributes(attrs)
            .build();

        am.requestAudioFocus(afr);

        // ---- WebRTC global init ----
        PeerConnectionFactory.initialize(
            PeerConnectionFactory.InitializationOptions
                .builder(context)
                .createInitializationOptions()
        );

        factory = PeerConnectionFactory.builder()
            .createPeerConnectionFactory();
    }

    public void createPeerConnection() {

        PeerConnection.RTCConfiguration config =
            new PeerConnection.RTCConfiguration(
                Collections.singletonList(
                    PeerConnection.IceServer.builder(
                        "stun:stun.l.google.com:19302"
                    ).createIceServer()
                )
            );

        peerConnection =
            factory.createPeerConnection(config, new PeerConnection.Observer() {

                @Override
                public void onSignalingChange(
                        PeerConnection.SignalingState signalingState) {}

                @Override
                public void onIceConnectionChange(
                        PeerConnection.IceConnectionState iceConnectionState) {}

                @Override
                public void onIceConnectionReceivingChange(boolean b) {}

                @Override
                public void onIceGatheringChange(
                        PeerConnection.IceGatheringState iceGatheringState) {}

                @Override
                public void onIceCandidate(IceCandidate iceCandidate) {}

                @Override
                public void onIceCandidatesRemoved(
                        IceCandidate[] iceCandidates) {}

                @Override
                public void onAddStream(MediaStream mediaStream) {}

                @Override
                public void onRemoveStream(MediaStream mediaStream) {}

                @Override
                public void onDataChannel(DataChannel dataChannel) {}

                @Override
                public void onRenegotiationNeeded() {}

                @Override
                public void onTrack(RtpTransceiver transceiver) {}
            });

        // This enables microphone + speaker automatically
        peerConnection.addTransceiver(
            MediaStreamTrack.MediaType.MEDIA_TYPE_AUDIO
        );
    }

    public void createOffer() {
        peerConnection.createOffer(new SdpObserver() {
            @Override
            public void onCreateSuccess(SessionDescription sdp) {
                peerConnection.setLocalDescription(this, sdp);
            }

            @Override public void onSetSuccess() {}
            @Override public void onCreateFailure(String error) {}
            @Override public void onSetFailure(String error) {}
        }, new MediaConstraints());
    }
}
