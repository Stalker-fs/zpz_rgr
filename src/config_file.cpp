#include <iostream>
#include <fstream>
#include <json/json.h>

#include "../include/config_file.h"

Config::Config() {
    std::ifstream fh("delays.json", std::ifstream::binary);
    if (fh.is_open()) {
        if (!Json::parseFromStream(builder, fh, &root, &errs)) {
            std::cerr << "Error JSON: " << errs << std::endl;
        }
        fh.close();
    }
}

void Config::set_user(const std::string& username, std::string phrase, const std::vector<double>& S2_array) {
    Json::Value user;
    Json::Value S2(Json::arrayValue);

    for (const double& val : S2_array) {
        S2.append(val);
    }

    user["S2"] = S2;
    user["phrase"] = phrase;
    root[username] = user;
}

void Config::save() {
    Json::StreamWriterBuilder writer;
    std::string json_str = Json::writeString(writer, root);

    std::ofstream fh("delays.json", std::ios::trunc);
    if (fh.is_open()) {
        fh << json_str;
        fh.close();
    } else {
        std::cerr << "Error opening file for writing!" << std::endl;
    }
}

void Config::get_user_list(std::vector<std::string>& list) {
    for (const auto& username : root.getMemberNames()) {
        list.push_back(username);
    }
}

bool Config::get_S2(std::string user, std::vector<double>& S2) {
    if (!root.isMember(user)) {
        return false;
    }

    for (const auto& val : root[user]["S2"]) {
        S2.push_back(val.asDouble());
    }

    return true;
}

bool Config::is_user_exist(std::string user) {
    return root.isMember(user);
}
