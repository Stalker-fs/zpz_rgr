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

void Config::set_user(const std::string& username, std::string phrase, std::map<double, double> S2_dict) {
    Json::Value user;
    Json::Value data(Json::objectValue);

    for (const auto& [M, S2] : S2_dict) {
        data[std::to_string(M)] = S2;
    }

    user["data"] = data;
    user["phrase"] = phrase;
    root[username] = user;

    // save
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

bool Config::get_M_S2(std::string user, std::map<double, double>& M_S2) {
    if (!root.isMember(user)) {
        return false;
    }

    const Json::Value& data = root[user]["data"];
    
    for (const auto& val : data.getMemberNames()) {
        M_S2[std::stod(val)] = data[val].asDouble();
    }

    return true;
}

bool Config::is_user_exist(std::string user) {
    return root.isMember(user);
}
