#pragma once

#include <string>

#include "json.hpp"

// Pure JSON builders for the obs-websocket vendor API. Kept free of OBS/Qt
// so a small build-time check can validate the shapes documented in
// WEBSOCKET_API.md without linking against OBS.

inline constexpr int kObsMultiRtmpApiVersion = 2;

inline nlohmann::json BuildApiVersionJson(const char* pluginVersion)
{
    nlohmann::json j;
    j["apiVersion"] = kObsMultiRtmpApiVersion;
    j["pluginVersion"] = pluginVersion ? pluginVersion : "";
    return j;
}

inline nlohmann::json BuildListCapabilitiesJson(const char* pluginVersion,
                                                const nlohmann::json& requests)
{
    nlohmann::json j = BuildApiVersionJson(pluginVersion);
    j["vendor"] = "obs-multi-rtmp";
    j["docs"] = "https://github.com/orangeqoon/obs-multi-rtmp/blob/master/WEBSOCKET_API.md";
    j["events"] = nlohmann::json::array({"target_state_changed", "emergency_stop"});
    j["requests"] = requests;
    return j;
}

inline nlohmann::json BuildTargetStateChangedJson(const std::string& id,
                                                  const std::string& name,
                                                  const std::string& state,
                                                  int lastErrorCode,
                                                  int reconnectCount)
{
    nlohmann::json j;
    j["id"] = id;
    j["name"] = name;
    j["state"] = state;
    j["last_error_code"] = lastErrorCode;
    j["reconnect_count"] = reconnectCount;
    return j;
}

inline nlohmann::json BuildEmergencyStopJson(const std::string& timeIso,
                                             const nlohmann::json& stoppedIds,
                                             int count)
{
    nlohmann::json j;
    j["time"] = timeIso;
    j["stopped_ids"] = stoppedIds;
    j["count"] = count;
    return j;
}