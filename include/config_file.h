struct ConfigData {
    std::vector<std::vector<unsigned int>*> delays;
    int max_S;
};

void save_delays(struct ConfigData in_data);
ConfigData load_delays();