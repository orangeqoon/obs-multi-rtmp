#pragma once

#include "json.hpp"
#include "obs-websocket-api.h"

// marust-facing vendor requests (get_snapshot). Registered alongside the
// core obs-multi-rtmp vendor requests; see RegisterWebsocketVendor().

void RegisterMarustVendorRequests(obs_websocket_vendor vendor);
void AppendMarustRequestDescriptions(nlohmann::json& requests);