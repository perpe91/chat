#pragma once

#include "time_utils.hpp"

#include <chrono>
#include <json.hpp>
#include <string>

namespace chat {

struct Message {
    std::string username;
    std::chrono::system_clock::time_point timestamp;
    std::string msg;

    bool operator==(const Message& o) const {
        return username == o.username && timestamp == o.timestamp && msg == o.msg;
    }

    bool operator<(const Message& o) const {
        if (timestamp == o.timestamp) {
            if (username == o.username) {
                return msg < o.msg;
            } else {
                return username < o.username;
            }
        }
        return timestamp < o.timestamp;
    }

    nlohmann::json to_json() const {
        nlohmann::json mj;
        mj["user"] = username;
        mj["time"] = time_to_string(timestamp);
        mj["msg"] = msg;
        return mj;
    }

    std::string to_json_string() const {
        return to_string(to_json());
    }

    static Message from_json(const nlohmann::json& mj) {
        Message m;
        m.username = mj["user"];
        m.timestamp = time_from_string(mj["time"]);
        m.msg = mj["msg"];
        return m;
    }

    static Message from_json_string(const std::string& js) {
        nlohmann::json mj = nlohmann::json::parse(js);
        return Message::from_json(mj);
    }
};

}