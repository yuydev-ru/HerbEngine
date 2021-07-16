#include <nlohmann/json.hpp>
#include <unordered_map>
#include <fstream>

namespace Parsing {
    // NOTE(vincento): Все json-функции должны вызываться через абстракции, описанные ниже.
    using json = nlohmann::json;

    typedef  nlohmann::basic_json<std::map, std::vector, std::string, bool, int64_t, uint64_t, double,
            std::allocator, nlohmann::adl_serializer, std::vector<std::uint8_t>> configFile;

    typedef nlohmann::json::iterator pIterator;

    static configFile
    parseConfigFile(const std::string &path)
    {
        std::ifstream ifs(path);
        configFile f = json::parse(ifs);
        ifs.close();
        return f;
    }
    static pIterator
    findEntries(configFile &file, const std::string &key)
    {
        return file.find(key);
    }

    template <class T>
    static T
    parseElement(configFile &dict, const std::string &key)
    {
        return dict.find(key)->get<T>();
    }
    static sf::Vector2f
    parseVector2f(configFile &dict, const std::string &key)
    {
        auto buf = dict.find(key)->get<std::vector<float>>();
        return { buf[0], buf[1] };
    }
    template<class T>
    static std::unordered_map<std::string, T>
    parseKeyData(configFile &dict, const std::string &key, const std::string &subkey1, const std::string &subkey2)
    {

    }
}