#pragma once

#include <fstream>
#include <cstddef>
#include <array>
#include <cstdlib>
#include <nlohmann/json.hpp>

#include "../common/bytes.hpp"
#include "../common/macros.hpp"

inline std::string CONFIG_FILE_PATH = std::string{ std::getenv("TRADING_ENGINE_HOME")} + "src/core/config/config.json";

using json = nlohmann::json;

inline std::string parse_interface_from_json(std::string_view file_path)
{
    auto json_file = std::ifstream(file_path.data());

    json data = json::parse(json_file);

    if (data["network"]["interface"].is_null())
        error_exit("interface not found");

    return std::string{ data["network"]["interface"] };
}

inline std::vector<std::string> parse_symbols_from_json(std::string_view file_path)
{
    auto json_file = std::ifstream(file_path.data());

    json data = json::parse(json_file);   

    if (data["engine"]["tickers"].is_null())
        error_exit("ticker symbols not found");

    std::vector<std::string> tickers;
    for (const auto& ticker : data["engine"]["tickers"])
        tickers.push_back(ticker);
    return tickers;
}

class Config
{
public:
    static Config& instance() noexcept
    {
        static Config c{};
        return c;
    }

    const auto& interface() noexcept
    {
        return interface_;
    }

    const auto& symbols() noexcept
    {
        return symbols_;
    }

private:
    auto init_symbols() noexcept
    {
        auto symbol_strs = parse_symbols_from_json(CONFIG_FILE_PATH);

        auto str_to_bytes = [&](std::string_view sv) -> std::array<std::byte, 8> {
            std::array<std::byte, 8> symbol_bytes;
            symbol_bytes.fill(std::byte{ 0x20 });
            for (auto i = 0uz; i < sv.size(); i++)
                symbol_bytes[i] = std::byte( sv[i] );
            return symbol_bytes;
        };

        std::vector<uint64_t> symbols;
        for (const auto& s : symbol_strs)
            symbols.push_back(loadu<uint64_t>(str_to_bytes(s).data()));

        return symbols;
    }

    Config() noexcept
    {
        interface_ = parse_interface_from_json(CONFIG_FILE_PATH);
        symbols_ = init_symbols();
    }
    
    std::string interface_;
    std::vector<uint64_t> symbols_;
};