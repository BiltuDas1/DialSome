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

    // Native callbacks to C++
    public native void onLocalIceCandidate(String sdp, String sdpMid, int sdpMLineIndex);
    public native void onLocalSdp(String sdp, String type);

    public void init(Context context) {
        PeerConnectionFactory.initialize(
            PeerConnectionFactory.InitializationOptions.builder(context).createInitializationOptions()
        );
        factory = PeerConnectionFactory.builder().createPeerConnectionFactory();
    }

    public void createPeerConnection() {
        PeerConnection.RTCConfiguration config = new PeerConnection.RTCConfiguration(
            Collections.singletonList(PeerConnection.IceServer.builder("stun:stun.l.google.com:19302").createIceServer())
        );

        peerConnection = factory.createPeerConnection(config, new PeerConnection.Observer() {
            @Override public void onIceCandidate(IceCandidate iceCandidate) {
                // Tell C++ we found a candidate
                onLocalIceCandidate(iceCandidate.sdp, iceCandidate.sdpMid, iceCandidate.sdpMLineIndex);
            }
            // ... other overrides empty ...
            @Override public void onSignalingChange(PeerConnection.SignalingState s) {}
            @Override public void onIceConnectionChange(PeerConnection.IceConnectionState s) {}
            @Override public void onIceConnectionReceivingChange(boolean b) {}
            @Override public void onIceGatheringChange(PeerConnection.IceGatheringState s) {}
            @Override public void onIceCandidatesRemoved(IceCandidate[] i) {}
            @Override public void onAddStream(MediaStream m) {}
            @Override public void onRemoveStream(MediaStream m) {}
            @Override public void onDataChannel(DataChannel d) {}
            @Override public void onRenegotiationNeeded() {}
            @Override public void onTrack(RtpTransceiver t) {}
        });

        peerConnection.addTransceiver(MediaStreamTrack.MediaType.MEDIA_TYPE_AUDIO);
    }

    public void createOffer() {
        peerConnection.createOffer(new SimpleSdpObserver() {
            @Override public void onCreateSuccess(SessionDescription sdp) {
                peerConnection.setLocalDescription(new SimpleSdpObserver(), sdp);
                onLocalSdp(sdp.description, sdp.type.canonicalForm());
            }
        }, new MediaConstraints());
    }

    public void handleRemoteSdp(String sdp, String type) {
        SessionDescription remoteSdp = new SessionDescription(
            SessionDescription.Type.fromCanonicalForm(type), sdp);

        peerConnection.setRemoteDescription(new SimpleSdpObserver() {
            @Override public void onSetSuccess() {
                if (remoteSdp.type == SessionDescription.Type.OFFER) {
                    createAnswer();
                }
            }
        }, remoteSdp);
    }

    private void createAnswer() {
        peerConnection.createAnswer(new SimpleSdpObserver() {
            @Override public void onCreateSuccess(SessionDescription sdp) {
                peerConnection.setLocalDescription(new SimpleSdpObserver(), sdp);
                onLocalSdp(sdp.description, sdp.type.canonicalForm());
            }
        }, new MediaConstraints());
    }

    public void addRemoteIceCandidate(String sdp, String sdpMid, int sdpMLineIndex) {
        peerConnection.addIceCandidate(new IceCandidate(sdpMid, sdpMLineIndex, sdp));
    }

    // Helper to keep code clean
    private class SimpleSdpObserver implements SdpObserver {
        @Override public void onCreateSuccess(SessionDescription s) {}
        @Override public void onSetSuccess() {}
        @Override public void onCreateFailure(String s) {}
        @Override public void onSetFailure(String s) {}
    }
}
