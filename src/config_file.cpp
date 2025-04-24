#include <iostream>
#include <fstream>
#include <json/json.h>

#include "../include/config_file.h"

void save_delays(struct ConfigData in_data) {
    Json::Value root;
    root["max_S"] = in_data.max_S;

    Json::Value data(Json::arrayValue);

    for (std::vector<unsigned int>* x : in_data.delays){
        Json::Value one_arr(Json::arrayValue);
        for (const unsigned int y : *x) {
            one_arr.append(y);
        }
        data.append(one_arr);
    }
    root["data"] = data;

    Json::StreamWriterBuilder writer;
    std::string json_str = Json::writeString(writer, root);

    std::ofstream fh("delays.json");
    fh << json_str;
    fh.close();
}

ConfigData load_delays() {
    std::ifstream fh("delays.json", std::ifstream::binary);

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errs;

    if (!Json::parseFromStream(builder, fh, &root, &errs)) {
        std::cerr << "Error JSON: " << errs << std::endl;
        return {};
    }

    std::vector<std::vector<unsigned int>*> delays;
    // std::vector<unsigned int>* delay_set = nullptr;

    const Json::Value& data = root["data"];

    if (data.isArray()) {
        for (const Json::Value& set : data) {
            if (set.isArray()) {
                std::vector<unsigned int>* delay_set = new std::vector<unsigned int>();
                for (const Json::Value& val : set) {
                    delay_set->push_back(val.asUInt());
                }
                delays.push_back(delay_set);
            }
        }
    }

    ConfigData conf_data;
    conf_data.delays = delays;
    conf_data.max_S = root["max_S"].asInt();
    
    return conf_data;
}
