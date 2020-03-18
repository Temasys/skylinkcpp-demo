// Copyright 2020 Temasys Communications Pte Ltd

// Includes from this project
#include "SkylinkObserverImpl.h"

// Includes from Temasys
#include "temasys/Skylink.h"
#include "temasys/AudioMedia.h"
#include "temasys/VideoMedia.h"
#include "temasys/Logs.h"
#include "temasys/Helpers.h"

// Includes from other non-standard projects
#include <SDL.h>

// STD Includes
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>  

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc.h>
#include <objc/objc-runtime.h>
#endif

using namespace Temasys;

bool stopProgram = false;

void signalHandler(int signal)
{
  if (signal == SIGINT)
  {
    std::cout << "Interrupt signal received." << std::endl;
    stopProgram = true;
  }
  else if (signal == SIGTERM)
  {
    std::cout << "Termination signal received." << std::endl;
    stopProgram = true;
  }
  else
  {
      std::cout << "Unhandled signal received. Skipping..." << std::endl;
  }
}

LogLevel StringToLogLevel(std::string level) {
       if (level == "TRACE")  return Temasys::LogLevel::SL_TRACE;
  else if (level == "INFO")   return Temasys::LogLevel::SL_INFO;
  else if (level == "WARN")   return Temasys::LogLevel::SL_WARN;
  else if (level == "ERROR")  return Temasys::LogLevel::SL_ERROR;
  else if (level == "NONE")   return Temasys::LogLevel::SL_NONE;

  return Temasys::LogLevel::SL_NONE; // Disable logs by default
}

std::string LogLevelToString(LogLevel level) {
       if (level == Temasys::LogLevel::SL_TRACE)   return "TRACE";
  else if (level == Temasys::LogLevel::SL_INFO)    return "INFO";
  else if (level == Temasys::LogLevel::SL_WARN)    return "WARN";
  else if (level == Temasys::LogLevel::SL_ERROR)   return "ERROR";
  else if (level == Temasys::LogLevel::SL_NONE)    return "NONE";

  return "Bad log level";
}

int main(int argc, char * argv[])
{
  //Register the signal handler
  signal( SIGINT, &signalHandler);
  signal( SIGTERM, &signalHandler);

  // DEFAULT SETTINGS
  std::string appKey          = "";
  std::string secret          = "";
  std::string roomName        = "";
  std::string userData        = "{ userName: \"Alice\", job: \"captain\" }";
  LogLevel    skylinkLogLevel = LogLevel::SL_ERROR;
  LogLevel    webrtcLogLevel  = LogLevel::SL_NONE;
  bool        sendAudio       = true;
  bool        sendVideo       = true;

  std::string optionList = "skylinkcpp-demo"
                           " -k {appKey}"
                           " -s {secret}"
                           " -r {roomName}"
                           " [-a {sendAudio:true/false}]"
                           " [-v {sendVideo:true/false}]"
                           " [-l {skylinLogLevel}]"
                           " [-w {webrtcLogLevel}]"
                           "\n"
                           "Log levels are: TRACE, INFO, WARN, ERROR, NONE.";

  // From https://www.geeksforgeeks.org/getopt-function-in-c-to-parse-command-line-arguments/
  // put ':' in the starting of the 
  // string so that program can  
  // distinguish between '?' and ':'  
  int opt; 
  // TODO: support getopt_long
  while((opt = getopt(argc, argv, "k:s:r:a:v:l:w:")) != -1)
  {  
    switch(opt) {  
      case 'k':
        appKey = std::string(optarg);
        std::cout << "App Key from cli options: " << appKey << std::endl << std::flush;
        break;  
      case 's':  
        secret = std::string(optarg);
        std::cout << "Secret from cli options: " << secret << std::endl << std::flush;
        break;  
      case 'r':  
        roomName = std::string(optarg);
        std::cout << "Room name from cli options: " << roomName << std::endl << std::flush;
        break;
      case 'a':  
        sendAudio = std::string(optarg) == "true";
        std::cout << "Send audio from cli options: " << sendAudio << std::endl << std::flush;
        break;
      case 'v':  
        sendVideo = std::string(optarg) == "true";
        std::cout << "Send video from cli options: " << sendVideo << std::endl << std::flush;
        break;
      case 'l':  
        skylinkLogLevel = StringToLogLevel(std::string(optarg));
        std::cout << "Skylink log level from cli options: " << LogLevelToString(skylinkLogLevel) << std::endl << std::flush;
        break;
      case 'w':  
        webrtcLogLevel  = StringToLogLevel(std::string(optarg));
        std::cout << "WebRTC log level from cli options: " << LogLevelToString(webrtcLogLevel) << std::endl << std::flush;
        break;
      default:
        std::cout << "Unsupported argument(s)" << std::endl
                  << "Usage : " << optionList  << std::endl << std::flush;
        exit(1);
        break;
    }  
  }

  if (appKey.empty() || secret.empty() || roomName.empty()) {
    std::cout << "Missing arguments. Usage:" << std::endl << '\t' << optionList << std::flush;
    exit(2);
  }

  // Print SDK version
  std::cout << "Skylink SDK Version: " << Skylink::SDK_VERSION() << std::endl << std::flush;

  // Set log levels
  Temasys::SetLogLevel(skylinkLogLevel, webrtcLogLevel);

  // SDL is used for video rendering
  SDL_Init(SDL_INIT_EVERYTHING);

  // Init skylink
  auto skylink = Skylink::Create(appKey, secret);
  skylink->setUserData(userData);

  // Register observer
  std::shared_ptr<SkylinkObserverImpl> observer = std::make_shared<SkylinkObserverImpl>();
  skylink->registerObserver(observer);

  // Media factory
  MediaFactory_Ptr  mediaFactory  = skylink->getMediaFactory();

  // Print available devices
  std::cout << "Audio devices available:" << std::endl << std::flush;
  auto audioCapturers = mediaFactory->getLocalAudioCapturers();
  for (const auto& device : audioCapturers) {
    std::cout << device.id << " " 
              << device.label << " " 
              << (device.kind == MTYPE_AUDIO ? "audio" : "video")
              << std::endl << std::flush;
  }
  std::cout << "Video devices available:" << std::endl << std::flush;
  auto videoCapturers = mediaFactory->getLocalVideoCapturers();
  for (const auto& device : videoCapturers) {
    std::cout << device.id << " " 
              << device.label << " " 
              << (device.kind == MTYPE_AUDIO ? "audio" : "video")
              << std::endl << std::flush;
  }

  // Video constraints
  MediaConstraints videoConstraints;
  videoConstraints.maxWidth = 640;
  videoConstraints.maxFramerate = 20;

  // Capture audio & video
  AudioMedia_Ptr microphone;
  VideoMedia_Ptr camera;
  if (sendAudio) {
    if (audioCapturers.empty()) {
      std::cout << "Audio enabled, but no audio capture devices available. Exiting." << std::endl << std::flush;
      exit(11);
    }
    std::cout << "Starting local microphone" << std::endl << std::flush;
                 mediaFactory->setAudioInputDevice(audioCapturers.front());
    microphone = mediaFactory->createMicrophoneMedia();
    microphone->registerMediaObserver(observer->mediaObserver_);
  }
  if (sendVideo) {
    if (videoCapturers.empty()) {
      std::cout << "Video enabled, but no video capture devices available. Exiting." << std::endl << std::flush;
      exit(12);
    }
    std::cout << "Starting local camera" << std::endl << std::flush;
    camera = mediaFactory->createCameraMedia(videoCapturers.front(), videoConstraints);
    camera->registerMediaObserver(observer->mediaObserver_);
  }

  // Render local video
  std::shared_ptr<VideoRenderer> renderer;
  if (camera) {
    std::cout << "Starting local camera rendering" << std::endl << std::flush;
    renderer = std::make_shared<VideoRenderer>("local camera");
    camera->addSink(renderer);
  }

  // Send media
  std::cout << "Sending local media (if any)" << std::endl << std::flush;
  auto streamID = Temasys::CreateRandomUuid();
  if (microphone) skylink->sendMedia(microphone,  streamID);
  if (camera)     skylink->sendMedia(camera,      streamID);

  // Join room
  std::cout << "Joining room" << std::endl << std::flush;
  skylink->joinRoom(roomName);

  ////////////////////////////////////////////////////////////
  // Checks the setUserData API
  // sleep(10);
  // std::cout << "Setting new local userData." << std::endl << std::flush;
  // skylink->setUserData("newUserDataIsASimpleString");

  ////////////////////////////////////////////////////////////
  // Mute and unmute audio media
  // sleep(10);
  // std::cout << "Muting local audio." << std::endl << std::flush;
  // microphone->mute();
  // sleep(10);
  // std::cout << "Unmuting local audio." << std::endl << std::flush;
  // microphone->unmute();

  ////////////////////////////////////////////////////////////
  // Mute and unmute video media
  // sleep(10);
  // std::cout << "Muting local video." << std::endl << std::flush;
  // camera->stop();
  // sleep(10);
  // std::cout << "Unmuting local video." << std::endl << std::flush;
  // camera->unmute();

  ////////////////////////////////////////////////////////////
  // Stop sending and restart sending audio
  // sleep(10);
  // std::cout << "Stop sending local audio." << std::endl << std::flush;
  // skylink->stopSendingMedia(microphone);
  // sleep(10);
  // std::cout << "Start sending local audio again." << std::endl << std::flush;
  // skylink->sendMedia(microphone, streamID);

  ////////////////////////////////////////////////////////////
  // Stop sending and restart sending video
  // sleep(10);
  // std::cout << "Stop sending local video." << std::endl << std::flush;
  // skylink->stopSendingMedia(camera);
  // sleep(10);
  // std::cout << "Start sending local video again." << std::endl << std::flush;
  // skylink->sendMedia(camera, streamID);

  ////////////////////////////////////////////////////////////
  // Checks the sending message API
  // std::cout << "Type a message to send. (rendering will be blocked until you do..." << std::endl << std::flush;
  // std::string message;
  // std::cin >> message;
  // std::cout << "Sending message" << std::endl << std::flush;
  // skylink->sendMessage(message/*, "replace me with a target to send a private message"*/);

  ////////////////////////////////////////////////////////////
  // Recording
  // std::cout << "Starting recording for 60sec" << std::endl << std::flush;
  // skylink->startRecording();
  // sleep(15);
  // std::cout << "Stopping recording" << std::endl << std::flush;
  // skylink->stopRecording();

  ////////////////////////////////////////////////////////////
  // Checks leave and re-join room
  // sleep(10);
  // std::cout << "Leaving room." << std::endl << std::flush;
  // skylink->leaveRoom();
  // sleep(10);
  // std::cout << "Joining room again." << std::endl << std::flush;
  // skylink->joinRoom(roomName);

  ////////////////////////////////////////////////////////////
  // SDL loop. Mandartory for rendering via SDL
  // See https://jira.temasys.com.sg/browse/ESC-86
  while (!stopProgram) { // running will be put to FALSE when the user type /quit
    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT) {
        break; // Break the loop, exit the program
      }
    }
    SDL_Delay(20);
  }

  //cleanup
  skylink->leaveRoom();

  SDL_Quit();

  // ORDER MATTERS
  // Skylink MUST be released last, as it hold thread that some destructors will run on
  if (camera && renderer) camera->removeSink(renderer);
  skylink->unregisterObserver(observer);
  microphone  = NULL;
  camera      = NULL;
  renderer    = NULL;
  observer    = NULL;
  skylink     = NULL;

  std::cout << "Exiting SampleApp." << std::endl;
  return 0;
}
