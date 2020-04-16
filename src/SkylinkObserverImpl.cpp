// Copyright 2020 Temasys Communications

// This class's header
#include "SkylinkObserverImpl.h"

// Includes from this project
#include "MediaObserverImpl.h"
#include "temasys/VideoMedia.h"
#include "temasys/AudioMedia.h"

// Includes from other non-standard projects

// STD Includes
#include <assert.h>
#include <memory>


namespace Temasys {
/////////////////////////////////////////////////////////////////////////
///
/// VideoRenderer
///
/////////////////////////////////////////////////////////////////////////

VideoRenderer::VideoRenderer(const std::string& windowName)
: argb_frame_(0,0) {
  window_ = SDL_CreateWindow( windowName.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
  assert(window_);
}


VideoRenderer::~VideoRenderer() {
  SDL_DestroyWindow(window_);  
}

void VideoRenderer::onFrame(const I420Frame& frame) {
  std::string strTmp = std::string("I420 frame with: ") + std::to_string(frame.width) + std::string(", height: ") + std::to_string(frame.height);
  std::string strTmp2 = std::string("stride y, u, v, a:") + std::to_string(frame.strideY) + ", " + std::to_string(frame.strideU) + ", " + std::to_string(frame.strideV) + ", " + std::to_string(frame.strideA);

  if (!window_) return;

  if (frame.width != argb_frame_.width || frame.height != argb_frame_.height) {
    argb_frame_.resize(frame.width, frame.height);
    SDL_SetWindowSize(window_, frame.width, frame.height);
  }

  VideoMediaSink::toARGB(frame, argb_frame_);
  
  SDL_Surface *screen  = SDL_GetWindowSurface(window_);
  SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(argb_frame_.data,
                                                  argb_frame_.width,
                                                  argb_frame_.height,
                                                  32,
                                                  argb_frame_.width*4,
                                                  0, 0, 0, 0
  );


  // Put the image in the window
  if ( SDL_BlitSurface( surface, NULL, screen, NULL ) != 0) {
    std::cout << SDL_GetError() << std::endl << std::flush;
    SDL_FreeSurface(surface); // Free the memory
    return;
  }
  SDL_UpdateWindowSurface(window_); // Refresh the window
  SDL_FreeSurface(surface); // Free the memory
}

/////////////////////////////////////////////////////////////////////////
///
/// AudioRenderer
///
/////////////////////////////////////////////////////////////////////////

AudioRenderer::AudioRenderer(int samplingFrequency, int numChannels)
{
  PaError err = Pa_Initialize();
  if( err != paNoError)
  {
    std::cout << "pa initialize error: " << Pa_GetErrorText(err) << std::endl;
    assert(false);
  }
  
  outputParameters.device = Pa_GetDefaultOutputDevice();
  outputInfo = Pa_GetDeviceInfo(outputParameters.device);
  outputParameters.channelCount = numChannels;
  outputParameters.sampleFormat = PA_SAMPLE_TYPE;
  outputParameters.suggestedLatency = outputInfo->defaultHighOutputLatency; // TODO(sp): play with this to check the audio latency 
  outputParameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(&stream,
                      NULL,
                      &outputParameters,
                      samplingFrequency,
                      FRAMES_PER_BUFFER,
                      paNoFlag,
                      NULL,
                      NULL );
  if( err != paNoError)
  {
    std::cout << "pa open stream error: " << Pa_GetErrorText(err) << std::endl;
    assert(false);
  }

  err = Pa_StartStream(stream);
  if( err != paNoError)
  {
    std::cout << "pa start stream error: " << Pa_GetErrorText(err) << std::endl;
    assert(false);
  }

}

AudioRenderer::~AudioRenderer()
{
  err = Pa_StopStream(stream);
  if( err != paNoError)
  {
    std::cout << "pa stop stream error: " << Pa_GetErrorText(err) << std::endl;
    assert(false);
  }
  Pa_Terminate();
}

void AudioRenderer::onAudioData(const void* audioData,
                          int bitsPerSample,
                          int sampleRate,
                          size_t numberOfChannels,
                          size_t numberOfFrames)
{

  err = Pa_WriteStream( stream, audioData, numberOfFrames );
  if( err != paNoError)
    std::cout << "pa write stream error: " << Pa_GetErrorText(err) << std::endl;

}


/////////////////////////////////////////////////////////////////////////
///
/// SkylinkObserverImpl
///
/////////////////////////////////////////////////////////////////////////

SkylinkObserverImpl::SkylinkObserverImpl()
{
  mediaObserver_ = std::make_shared<MediaObserverImpl>();
}

SkylinkObserverImpl::~SkylinkObserverImpl() {
  for (auto peerMedia : video_media_) {
    for (auto mediaItem : peerMedia.second) {
      mediaItem.second->removeSink(video_sinks_[mediaItem.first]);
    }
  }

  for (auto peerMedia : audio_media_) {
    for (auto mediaItem : peerMedia.second) {
      mediaItem.second->removeSink(audio_sinks_[mediaItem.first]);
    }
  }

  video_sinks_.clear();
  audio_sinks_.clear();
  video_media_.clear();
  audio_media_.clear();
  mediaObserver_ = NULL;
}

void SkylinkObserverImpl::onRemotePeer(const std::string& peerId, const std::string& userData) {
  std::cout << "Remote peer joined: " << peerId << ", userData: " << userData << std::endl << std::flush;

  // TODO: create medai entry here
}

void SkylinkObserverImpl::onRemotePeerLeft(const std::string& peerId) {
  std::cout << "Remote peer left: " << peerId << std::endl << std::flush;

  // Remove sinks from media
  //for (auto [mediaId,media] : video_media_[peerId]) {
  for (auto mediaItem : video_media_[peerId]) {
    mediaItem.second->removeSink(video_sinks_[mediaItem.first]);
    video_sinks_.erase(mediaItem.first);
  }
  //for (auto& [mediaId,media] : audio_media_[peerId]) {
  for (auto mediaItem : audio_media_[peerId]) {
    mediaItem.second->removeSink(audio_sinks_[mediaItem.first]);
    audio_sinks_.erase(mediaItem.first);
  }

  //clear media 
  video_media_.erase(peerId);
  audio_media_.erase(peerId);
}

void SkylinkObserverImpl::onRemoteUserDataUpdated(const std::string& peerId, const std::string& userData) {
  std::cout << "Remote peer user data updated: " << peerId << ", userData: " << userData << std::endl << std::flush;
}

void SkylinkObserverImpl::onInRoom(const std::string& peerId, const RoomMetadata& roomMetadata) {
  self_peer_id_ = peerId;
  std::cout << "Successfully joined room. My peerId is: " << self_peer_id_ << " RoomType is: " << (roomMetadata.roomType == Temasys::RT_MCU ? "MCU" : "P2P") << std::endl << std::flush;
}

void SkylinkObserverImpl::onRemoteVideoMedia(const std::string& peerId, VideoMedia_Ptr media)
{
  std::cout << "Remote video media received. PeerID: " << peerId 
            << " mediaId: "  << media->getId() 
            << " streamId: " << media->getStreamId() 
            << std::endl << std::flush;

  // Store the media
  auto peerMap = video_media_.find(peerId);
  if (peerMap == video_media_.end()) {
    video_media_[peerId] = std::map<std::string, VideoMedia_Ptr>();
    peerMap = video_media_.find(peerId);
  }
  peerMap->second[media->getId()] = media;

  //add MediaObserver
  media->registerMediaObserver(mediaObserver_);

  // Create window for this media
  std::string windowName = peerId +"-"+ media->getId();
  auto renderer = std::make_shared<VideoRenderer>(windowName);
  media->addSink(renderer);
  video_sinks_[media->getId()] = renderer;
}

void SkylinkObserverImpl::onRemoteAudioMedia(const std::string& peerId, AudioMedia_Ptr media) {
  std::cout << "Remote audio media received. PeerID: " << peerId 
            << " mediaId: "  << media->getId() 
            << " streamId: " << media->getStreamId() 
            << std::endl << std::flush;

  // Store the media
  auto peerMap = audio_media_.find(peerId);
  if (peerMap == audio_media_.end()) {
    audio_media_[peerId] = std::map<std::string, AudioMedia_Ptr>();
    peerMap = audio_media_.find(peerId);
  }
  peerMap->second[media->getId()] = media;

  //add MediaObserver
  media->registerMediaObserver(mediaObserver_);

  // Create renderer for audio
  auto audioRenderer = std::make_shared<AudioRenderer>(48000, 1); // TODO: I'm not sure that we can blindly set this to 48kHz, and I'm not sure that we should have multiple AudioRenderers either
  media->addSink(audioRenderer);
  audio_sinks_[media->getId()] = audioRenderer;
}

void SkylinkObserverImpl::onMessage(const std::string& clientId, const Message& message) {
  std::cout << (message.isPrivate ? "Private" : "Public") << " message"
            << " from peerId:" << message.senderId
            << ", timestamp:" << message.timestamp
            << ", data:" << message.data 
            << std::endl << std::flush;
}

void SkylinkObserverImpl::onRecordingEvent(const Temasys::RecordingEvent& event) {
  std::string type;
  switch(event.type) {
    case REC_ON:      type = "ON";    break;
    case REC_OFF:     type = "OFF";   break;
    case REC_ERROR:   type = "ERROR"; break;
  }

  std::cout << "Recording event."
            << "RecordingId: " << event.recordingId
            << ", type: " << type
            << (event.type == Temasys::REC_ERROR ? ", error: " + event.error : "")
            << std::endl << std::flush;
}

void SkylinkObserverImpl::onSkylinkError(const Error& error) {
  std::cout << "Received an error from Skylink. Code: " << error.code << ", desc: " << error.desc << std::endl << std::flush;
}

}
