#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <iostream>
#include <fstream>
#include <json/json.h>

class Config {
    public:
        Config();
        void set_user(const std::string& username, std::string phrase, std::map<float, float> S2_dict);
        void save();
        void get_user_list(std::vector<std::string>& list);
        bool get_M_S2(std::string user, std::map<float, float>& M_S2);
        bool is_user_exist(std::string user);
        bool get_S2(std::string user, std::vector<float>& S2);
        int get_phrase_len(std::string user);
        bool check_credentials(std::string user, std::string phrase);
    
    private:
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errs;
};

#endif