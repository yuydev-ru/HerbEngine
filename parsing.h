#ifndef ENGINE_PARSING_H
#define ENGINE_PARSING_H

#include <nlohmann/json.hpp>
#include <unordered_map>
#include <fstream>
#include <SFML/Graphics.hpp>

namespace Parsing
{
    typedef nlohmann::json configFile;

    inline Parsing::configFile
    parseConfigFile(const std::string &path) {
        std::ifstream ifs(path);
        configFile f = nlohmann::json::parse(ifs);
        ifs.close();
        return f;
    }

    template<class T> T
    parseElement(Parsing::configFile &dict, const std::string &key) {
        return dict.find(key)->get<T>();
    }

    template<class T> sf::Vector2 <T>
    parseVector2(Parsing::configFile &dict, const std::string &key) {
        auto buf = dict.find(key)->get<std::vector<T>>();
        return {buf[0], buf[1]};
    }

    template<class T> std::unordered_map<std::string, T>
    parseKeyData(Parsing::configFile &dict, const std::string &key, const std::string &subkey1, const std::string &subkey2) {

    }
}

#endif