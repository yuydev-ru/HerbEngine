#include <nlohmann/json.hpp>
#include <unordered_map>
#include <fstream>

namespace Parsing
{
    typedef nlohmann::json configFile;

    static configFile
    parseConfigFile(const std::string &path)
    {
        std::ifstream ifs(path);
        configFile f = nlohmann::json::parse(ifs);
        ifs.close();
        return f;
    }

    template <class T> static T
    parseElement(configFile &dict, const std::string &key)
    {
        return dict.find(key)->get<T>();
    }

    template<class T> static sf::Vector2<T>
    parseVector2(configFile &dict, const std::string &key)
    {
        auto buf = dict.find(key)->get<std::vector<float>>();
        return { buf[0], buf[1] };
    }

    template<class T> static std::unordered_map<std::string, T>
    parseKeyData(configFile &dict, const std::string &key, const std::string &subkey1, const std::string &subkey2)
    {

    }
}