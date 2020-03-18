// Copyright 2020 Temasys Communications

// This class's header
#include "MediaObserverImpl.h"

// Includes from this project

// Includes from other non-standard projects

// STD Includes

namespace Temasys {

void MediaObserverImpl::onMediaStateChange(const std::string& mediaID, const MediaState& newMediaState)
{
  std::cout << "onMediaStateChange. mediaId: " << mediaID << ", new mediaState: " << newMediaState << std::endl;
}

void MediaObserverImpl::onMediaMetadataChange(const std::string& mediaID, const std::string& newMediaMetadata)
{
  std::cout << "onMediaMetadataChange. mediaId: " << mediaID << ", new mediaMetadata: " << newMediaMetadata << std::endl;
}


}
