#include "pch.h"

#include <chrono>
#include <ctime>
#include <string>

#include "marust-snapshot.h"
#include "marust-snapshot-json.hpp"
#include "websocket-api-json.hpp"
#include "dock-registry.h"
#include "output-config.h"
#include "obs-multi-rtmp.h"
#include "plugin-support.h"
#include "obs.hpp"
#include "json.hpp"

namespace {

void SetResponseFromJson(obs_data_t* response_data, const nlohmann::json& j)
{
    if (!response_data)
        return;
    auto dumped = j.dump();
    OBSDataAutoRelease parsed = obs_data_create_from_json(dumped.c_str());
    if (parsed)
        obs_data_apply(response_data, parsed);
}

std::string NowUtcIso8601()
{
    using clock = std::chrono::system_clock;
    auto now = clock::now();
    std::time_t tt = clock::to_time_t(now);
    std::tm utc{};
#ifdef _WIN32
    gmtime_s(&utc, &tt);
#else
    gmtime_r(&tt, &utc);
#endif
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &utc);
    return std::string(buf);
}

const char* SnapshotState(PushWidget* t)
{
    if (t->IsReconnecting())
        return "reconnecting";
    if (t->IsConnecting())
        return "connecting";
    if (t->IsRunning())
        return "live";
    // "stopping" is in the schema but PushWidget does not expose an
    // in-between flag without touching core files; map settled failures
    // to "error" and everything else to "stopped".
    if (t->GetLastErrorCode() != 0)
        return "error";
    return "stopped";
}

nlohmann::json LastErrorForCode(int code)
{
    if (code == 0)
        return nullptr;
    switch (code) {
    case -1:
        return obs_module_text("Error.WrongRTMPUrl");
    case -2:
        return obs_module_text("Error.ServerConnect");
    case -3:
        return obs_module_text("Error.ServerHandshake");
    case -4:
        return obs_module_text("Error.ServerRefuse");
    default:
        return obs_module_text("Error.Unknown");
    }
}

std::string ServerFromServiceSettings(const nlohmann::json& settings)
{
    if (!settings.is_object())
        return {};
    auto it = settings.find("server");
    if (it != settings.end() && it->is_string())
        return it->get<std::string>();
    return {};
}

nlohmann::json BuildLiveSnapshot()
{
    auto targets = nlohmann::json::array();
    for (auto* t : GetAllStreamTargets()) {
        // Secrets: only type + server. Never copy key/token/password/secret.
        targets.push_back(BuildSnapshotTargetJson(
            t->GetTargetId(),
            t->GetTargetName(),
            t->GetEnabled(),
            SnapshotState(t),
            t->GetReconnectCount(),
            LastErrorForCode(t->GetLastErrorCode()),
            t->GetProtocol(),
            ServerFromServiceSettings(t->GetServiceSettings())));
    }

    return BuildSnapshotJson(
        kObsMultiRtmpApiVersion,
        PLUGIN_VERSION,
        NowUtcIso8601(),
        GlobalMultiOutputConfig().hideDock,
        targets);
}

void OnGetSnapshot(obs_data_t*, obs_data_t* response_data, void*)
{
    // Intentionally ignore any request body (including reveal_secrets).
    GetGlobalService().RunInUIThreadBlocking([&]() {
        SetResponseFromJson(response_data, BuildLiveSnapshot());
    });
}

struct MarustRequest {
    const char* name;
    const char* description;
    obs_websocket_request_callback_function callback;
};

const MarustRequest kMarustRequests[] = {
    {"get_snapshot",
     "No params (reveal_secrets is ignored/rejected). One-shot reconcile snapshot: apiVersion, pluginVersion, observedAt (UTC), headless, and targets without secrets.",
     &OnGetSnapshot},
};

} // namespace

void RegisterMarustVendorRequests(obs_websocket_vendor vendor)
{
    if (!vendor)
        return;
    for (auto& req : kMarustRequests)
        obs_websocket_vendor_register_request(vendor, req.name, req.callback, nullptr);
}

void AppendMarustRequestDescriptions(nlohmann::json& requests)
{
    for (auto& req : kMarustRequests) {
        nlohmann::json r;
        r["name"] = req.name;
        r["description"] = req.description;
        requests.push_back(r);
    }
}