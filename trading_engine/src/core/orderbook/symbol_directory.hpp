#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

#include "../config/config.hpp"
#include "../network/message.hpp"

struct SymbolDirectory
{
    static SymbolDirectory& instance() noexcept {
        static SymbolDirectory symbol_dir{};
        return symbol_dir;
    }

    int8_t index(const uint16_t loc) const noexcept
    {
        return locate_to_index[loc];
    }

    void update(const std::byte* p) noexcept
    {
        uint16_t loc = load_be<uint16_t>(p + Offset<StockDirectory>::LOCATE);
        uint64_t symbol = load_le<uint64_t>(p + Offset<StockDirectory>::SYMBOL_STR);

        for (auto i = 0uz; i < NUM_WATCHED_SYMBOLS; i++) {
            if (symbol == watched_symbols[i]) {
                locate_to_index[loc] = i;
                return;
            }
        }
    }

    static constexpr std::size_t NUM_WATCHED_SYMBOLS{ 3 };

    SymbolDirectory()
    {
        std::ranges::fill(locate_to_index, -1);
        load_watched_symbols();
    }

    void load_watched_symbols() noexcept
    {
        const auto& symbols = Config::instance().symbols();
        
        for (auto i = 0uz; i < NUM_WATCHED_SYMBOLS; i++)
            watched_symbols[i] = symbols[i];
    }

    alignas(64) std::array<int8_t, 65536> locate_to_index;
    alignas(64) std::array<uint64_t, NUM_WATCHED_SYMBOLS> watched_symbols;
};