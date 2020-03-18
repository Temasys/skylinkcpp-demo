// Copyright 2020 Temasys Communications

#ifndef __TEMASYS_SKYLINK_OBSERVER_IMPL__
#define __TEMASYS_SKYLINK_OBSERVER_IMPL__

// Includes from this project
#include "temasys/SkylinkObserver.h"
#include "temasys/MediaObserver.h"

// Includes from other non-standard projects

// STD Includes
#include <iostream>
#include <string>
#include <map>

#include <SDL.h>

#include "portaudio.h"

#define FRAMES_PER_BUFFER  480
#define PA_SAMPLE_TYPE  paInt16

namespace Temasys { // TODO: put sample code outside of namespace

  class VideoRenderer : public VideoMediaSink {
  public:
    VideoRenderer(const std::string& windowName);
    ~VideoRenderer() override;
    void onFrame(const I420Frame& frame) override;

    SDL_Window* window_;
    ARGBFrame argb_frame_;
  };

  class AudioRenderer : public AudioMediaSink {
  public:
    AudioRenderer(int samplingFrequency, int numChannels = 1);
    ~AudioRenderer() override;
    void onAudioData(const void* audioData,
                     int bitsPerSample,
                     int sampleRate,
                     size_t numberOfChannels,
                     size_t numberOfFrames) override;
    
    //NOTE: All pa resources here are managed by the pa library. 
    PaStreamParameters outputParameters;
    const PaDeviceInfo* outputInfo;
    PaStream *stream = NULL;
    PaError err;
  };

  class SkylinkObserverImpl
  : public SkylinkObserver
   {
    public :
    SkylinkObserverImpl();
    ~SkylinkObserverImpl();

    void onInRoom(const std::string& peerId, const RoomMetadata& roomMetadata) override;
    void onRemotePeer(const std::string& peerId, const std::string& userData) override;
    void onRemotePeerLeft(const std::string& peerId) override;
    void onRemoteUserDataUpdated(const std::string& peerId, const std::string& userData) override;
    void onRemoteVideoMedia(const std::string& peerId, VideoMedia_Ptr media) override;
    void onRemoteAudioMedia(const std::string& peerId, AudioMedia_Ptr media) override;
    void onMessage(const std::string& peerId, const Message& message) override;
    void onRecordingEvent(const RecordingEvent& event) override;

    std::string self_peer_id_;

    // peerId: mediaId: media
    std::map<std::string, std::map<std::string, VideoMedia_Ptr>> video_media_;
    std::map<std::string, std::map<std::string, AudioMedia_Ptr>> audio_media_;

    std::map<std::string, std::shared_ptr<VideoRenderer>> video_sinks_; // mediaId: sink
    std::map<std::string, std::shared_ptr<AudioRenderer>> audio_sinks_; // mediaId: sink
    MediaObserver_Ptr mediaObserver_;
  };


}

#endif /* __TEMASYS_SKYLINK_OBSERVER_IMPL__ */
