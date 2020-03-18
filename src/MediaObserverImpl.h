// Copyright 2020 Temasys Communications

#ifndef __TEMASYS_MEDIA_OBSERVER_IMPL__
#define __TEMASYS_MEDIA_OBSERVER_IMPL__

// Includes from this project
#include "temasys/MediaObserver.h"


// Includes from other non-standard projects

// STD Includes
#include <iostream>
#include <string>
#include <map>

namespace Temasys { // TODO: put sample code outside of namespace

  class MediaObserverImpl
  : public MediaObserver
   {
    public :
    void onMediaStateChange(const std::string& mediaID, const MediaState& newMediaState) override;
    void onMediaMetadataChange(const std::string& mediaID, const std::string& newMediaMetadata) override;

  };


}

#endif /* __TEMASYS_MEDIA_OBSERVER_IMPL__ */
