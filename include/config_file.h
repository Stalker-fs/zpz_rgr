#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

#include <iostream>
#include <fstream>
#include <json/json.h>

class Config {
    public:
        Config();
        void set_user(const std::string& username, std::string phrase, std::map<double, double> S2_dict);
        void save();
        void get_user_list(std::vector<std::string>& list);
        bool get_M_S2(std::string user, std::map<double, double>& M_S2);
        bool is_user_exist(std::string user);
    
    private:
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errs;
};

#endif