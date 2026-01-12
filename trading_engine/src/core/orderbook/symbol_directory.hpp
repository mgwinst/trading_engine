#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

#include "../common/json_parser.hpp"

using StockLocate = uint16_t;
using Symbol = uint64_t;

class SymbolDirectory
{
public:
    static SymbolDirectory& instance() noexcept {
        static SymbolDirectory symbol_dir{};
        return symbol_dir;
    }

    inline auto index(const StockLocate loc) const noexcept
    {
        return locate_to_index[loc];
    }
    
    inline void on_stock_directory_msg(const StockLocate loc, const uint8_t* symbol_bytes) noexcept
    {
        Symbol s = read_symbol(symbol_bytes);

        for (auto i = 0uz; i < NUM_SYMBOLS; i++) {
            if (s == watched_symbols[i]) {
                locate_to_index[loc] = i;
                return;
            }
        }
    }

private:
    static constexpr std::size_t NUM_SYMBOLS{ 3 };   

    SymbolDirectory()
    {
        std::ranges::fill(locate_to_index, -1);
        load_watched_symbols();
    }

    void load_watched_symbols() noexcept
    {
        auto symbols = parse_tickers_from_json("../../config.json");

        for (auto i = 0uz; i < symbols->size(); i++)
            watched_symbols[i] = pack((*symbols)[i]);
    }

    inline Symbol read_symbol(const uint8_t* p) noexcept
    {
        Symbol value;
        std::memcpy(&value, p, sizeof(value));
        return value;
    }

    inline constexpr Symbol pack(std::string_view sv)
    {
        Symbol value = 0;
        for (auto i = 0; i < 8 && sv[i]; i++)
            value |= Symbol(uint8_t(sv[i]) << (i * 8));

        return value;
    }

    alignas(64) std::array<int8_t, 65536> locate_to_index;
    alignas(64) std::array<Symbol, NUM_SYMBOLS> watched_symbols;
};