#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>

#include "../config/config.hpp"
#include "../network/message.hpp"

class SymbolDirectory
{
public:
    static SymbolDirectory& instance() noexcept {
        static SymbolDirectory symbol_dir{};
        return symbol_dir;
    }

    uint64_t operator[](int8_t index) const noexcept
    {
        return watched_symbols[index];
    }

    auto index(const uint16_t loc) const noexcept
    {
        return locate_to_index[loc];
    }

    void update(const Message& msg) noexcept
    {
        for (auto i = 0uz; i < NUM_WATCHED_SYMBOLS; i++) {
            if (msg.symbol == watched_symbols[i]) {
                locate_to_index[msg.locate] = i;
                return;
            }
        }
    }

    const auto& get_watched_symbols() const noexcept
    {
        return watched_symbols;
    }

private:
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