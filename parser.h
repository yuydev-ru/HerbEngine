#ifndef ENGINE_PARSER_H
#define ENGINE_PARSER_H

#include <nlohmann/json.hpp>
#include <unordered_map>
#include <fstream>
#include <iostream>

struct Parser
{
    typedef nlohmann::json backend;
    typedef nlohmann::json::iterator backend_iterator;
    backend data;
    backend_iterator iterator;

    explicit Parser(backend &data) : data(data) {}

    explicit Parser(const std::string &path)
    {
        std::ifstream ifs(path);
        this->data = nlohmann::json::parse(ifs);
        ifs.close();
    }

    template <class T>
    explicit Parser(const std::string &path, std::initializer_list<T> list )
    {
        Parser parser(path);

        auto it = list.begin();
        this->data = parser.data[*it];

        for (it = list.begin()+1; it != list.end(); ++it)
        {
            this->data = this->data[*it];
        }
    }

    template <class T> T
    parseElement(const std::string &key)
    {
        return this->data.find(key)->get<T>();
    }

    template<class T> T
    parseElement(const std::string &key, const std::string &subkey)
    {
        return this->data.find(key)->find(subkey)->get<T>();
    }

    template<class T> T
    parseObjectElement(int index, const std::string &key)
    {
        return this->data[index].at(key);
    }

    template<class T> T
    parseObjectElement(int index, int key)
    {
        return this->data[index].at(key);
    }

    template<class T> sf::Vector2 <T>
    parseVector2(const std::string &key)
    {
        auto buf = this->data.find(key)->get<std::vector<T>>();
        return { buf[0], buf[1] };
    }

    unsigned int
    size()
    {
        return this->data.size();
    }
};

#endif