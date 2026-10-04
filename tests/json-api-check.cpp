#include <cstdio>
#include <cstdlib>
#include <string>

#include "json.hpp"
#include "websocket-api-json.hpp"

static int failures = 0;

static void Expect(bool cond, const char* msg)
{
    if (!cond) {
        std::fprintf(stderr, "FAIL: %s\n", msg);
        ++failures;
    }
}

int main()
{
    auto ver = BuildApiVersionJson("0.7.5.1");
    Expect(ver["apiVersion"] == kObsMultiRtmpApiVersion, "apiVersion == 2");
    Expect(ver["pluginVersion"] == "0.7.5.1", "pluginVersion string");

    nlohmann::json requests = nlohmann::json::array();
    nlohmann::json r;
    r["name"] = "get_api_version";
    r["description"] = "test";
    requests.push_back(r);

    auto caps = BuildListCapabilitiesJson("0.7.5.1", requests);
    Expect(caps["apiVersion"] == 2, "list_capabilities apiVersion");
    Expect(caps["pluginVersion"] == "0.7.5.1", "list_capabilities pluginVersion");
    Expect(caps["events"].is_array() && caps["events"].size() == 2, "events has 2");
    Expect(caps["events"][0] == "target_state_changed", "events[0]");
    Expect(caps["events"][1] == "emergency_stop", "events[1]");
    Expect(caps.contains("requests"), "requests present");

    auto state = BuildTargetStateChangedJson("1", "YouTube", "reconnecting", -2, 3);
    Expect(state["id"] == "1", "state id");
    Expect(state["name"] == "YouTube", "state name");
    Expect(state["state"] == "reconnecting", "state value");
    Expect(state["last_error_code"] == -2, "last_error_code");
    Expect(state["reconnect_count"] == 3, "reconnect_count");

    auto ids = nlohmann::json::array({"1", "2"});
    auto estop = BuildEmergencyStopJson("2026-10-05T02:50:00+09:00", ids, 2);
    Expect(estop["time"] == "2026-10-05T02:50:00+09:00", "emergency_stop time");
    Expect(estop["count"] == 2, "emergency_stop count");
    Expect(estop["stopped_ids"].size() == 2, "emergency_stop stopped_ids");

    if (failures) {
        std::fprintf(stderr, "%d check(s) failed\n", failures);
        return 1;
    }
    std::printf("json-api-check: all ok\n");
    return 0;
}