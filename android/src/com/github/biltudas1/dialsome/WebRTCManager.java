package com.github.biltudas1.dialsome;

import android.content.Context;
import android.util.Log;
import org.webrtc.*;
import java.util.Collections;

public class WebRTCManager {
    private static final String TAG = "WebRTCManager";
    private PeerConnectionFactory factory;
    private PeerConnection peerConnection;
    private AudioSource audioSource;
    private AudioTrack localAudioTrack;

    // Native callbacks to C++
    public native void onLocalIceCandidate(String sdp, String sdpMid, int sdpMLineIndex);
    public native void onLocalSdp(String sdp, String type);
    public native void onCallEstablished();

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
                onLocalIceCandidate(iceCandidate.sdp, iceCandidate.sdpMid, iceCandidate.sdpMLineIndex);
            }

            @Override public void onIceConnectionChange(PeerConnection.IceConnectionState newState) {
                Log.d(TAG, "ICE State Change: " + newState);
                if (newState == PeerConnection.IceConnectionState.CONNECTED ||
                    newState == PeerConnection.IceConnectionState.COMPLETED) {
                    onCallEstablished();
                }
            }

            @Override public void onTrack(RtpTransceiver transceiver) {
                Log.d(TAG, "Remote track added: " + transceiver.getReceiver().track().kind());
            }

            @Override public void onSignalingChange(PeerConnection.SignalingState s) { Log.d(TAG, "Signaling: " + s); }
            @Override public void onIceConnectionReceivingChange(boolean b) {}
            @Override public void onIceGatheringChange(PeerConnection.IceGatheringState s) {}
            @Override public void onIceCandidatesRemoved(IceCandidate[] i) {}
            @Override public void onAddStream(MediaStream m) {}
            @Override public void onRemoveStream(MediaStream m) {}
            @Override public void onDataChannel(DataChannel d) {}
            @Override public void onRenegotiationNeeded() {}
        });

        // Initialize local audio capture
        audioSource = factory.createAudioSource(new MediaConstraints());
        localAudioTrack = factory.createAudioTrack("ARDAMSa0", audioSource);
        localAudioTrack.setEnabled(true);

        // Add bidirectional transceiver (SEND_RECV) and attach local track
        RtpTransceiver.RtpTransceiverInit transceiverInit =
            new RtpTransceiver.RtpTransceiverInit(RtpTransceiver.RtpTransceiverDirection.SEND_RECV);

        RtpTransceiver transceiver = peerConnection.addTransceiver(
            MediaStreamTrack.MediaType.MEDIA_TYPE_AUDIO, transceiverInit);
        transceiver.getSender().setTrack(localAudioTrack, true);
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

    private class SimpleSdpObserver implements SdpObserver {
        @Override public void onCreateSuccess(SessionDescription s) {}
        @Override public void onSetSuccess() {}
        @Override public void onCreateFailure(String s) { Log.e(TAG, "SDP Create Failure: " + s); }
        @Override public void onSetFailure(String s) { Log.e(TAG, "SDP Set Failure: " + s); }
    }
}
