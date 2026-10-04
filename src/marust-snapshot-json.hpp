#pragma once

#include <string>

#include "json.hpp"

// Pure JSON builders for marust get_snapshot (no OBS/Qt). Validated by
// tests/json-api-check.cpp.

inline nlohmann::json BuildSnapshotTargetJson(const std::string& id,
                                              const std::string& name,
                                              bool enabled,
                                              const std::string& state,
                                              int reconnects,
                                              const nlohmann::json& lastError,
                                              const std::string& serviceType,
                                              const std::string& server)
{
    nlohmann::json service;
    service["type"] = serviceType;
    service["server"] = server;

    nlohmann::json t;
    t["id"] = id;
    t["name"] = name;
    t["enabled"] = enabled;
    t["state"] = state;
    t["reconnects"] = reconnects;
    t["lastError"] = lastError;
    t["service"] = service;
    return t;
}

inline nlohmann::json BuildSnapshotJson(int apiVersion,
                                        const char* pluginVersion,
                                        const std::string& observedAtUtc,
                                        bool headless,
                                        const nlohmann::json& targets)
{
    nlohmann::json j;
    j["apiVersion"] = apiVersion;
    j["pluginVersion"] = pluginVersion ? pluginVersion : "";
    j["observedAt"] = observedAtUtc;
    j["headless"] = headless;
    j["targets"] = targets;
    return j;
}