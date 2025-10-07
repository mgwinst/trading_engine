#pragma once

#include <nlohmann/json.hpp>
#include <string_view>
#include <optional>
#include <variant>
#include <fstream>
#include <vector>
#include <string>

using json = nlohmann::json;

inline std::optional<std::string> parse_interface_from_json(std::string_view file_path)
{
    auto json_file = std::ifstream(file_path.data());

    json data = json::parse(json_file);

    if (data["network"]["interface"].is_null()) {
        return std::nullopt;
    } else {
        return std::string{ data["network"]["interface"] };
    }
}

inline std::optional<std::vector<std::string>> parse_tickers_from_json(std::string_view file_path)
{
    auto json_file = std::ifstream(file_path.data());

    json data = json::parse(json_file);   

    if (data["engine"]["tickers"].is_null())
        return std::nullopt;
    else {
        std::vector<std::string> tickers;
        for (const auto& ticker : data["engine"]["tickers"])
            tickers.push_back(ticker);
        return tickers;
    }
}