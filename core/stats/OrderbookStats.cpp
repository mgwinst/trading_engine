#include <ranges>
#include <algorithm>

#include "stats/OrderbookStats.hpp"
#include <numeric>

namespace views = std::ranges::views;
namespace rng = std::ranges;

std::vector<std::pair<Price, Volume>> OrderbookStats::bid_prices(std::optional<std::size_t> count) const noexcept
{
    return get_prices(orderbook_.bid_levels_, count);
}

std::vector<std::pair<Price, Volume>> OrderbookStats::ask_prices(std::optional<std::size_t> count) const noexcept
{
    return get_prices(orderbook_.ask_levels_, count);
}

template <typename T>
std::vector<std::pair<Price, Volume>> OrderbookStats::get_prices(T &levels, std::optional<std::size_t> count) const noexcept
{
    if (count) {
        auto prices = views::reverse(orderbook_.ask_levels_) | views::take(*count);
        return rng::to<std::vector<std::pair<Price, Volume>>>(prices);
    } else {
        auto prices = views::reverse(orderbook_.ask_levels_);
        return rng::to<std::vector<std::pair<Price, Volume>>>(prices);
   }
}

// throw on no bids/asks? return only price or volume also?
std::pair<Price, Price> OrderbookStats::best_prices() const noexcept
{
    auto best_bid = orderbook_.bid_levels_.back().first;
    auto best_ask = orderbook_.ask_levels_.back().first;

    return {best_bid, best_ask};
}

Price OrderbookStats::best_bid() const noexcept
{
    return orderbook_.bid_levels_.back().first;
}

Price OrderbookStats::best_ask() const noexcept
{
    return orderbook_.ask_levels_.back().first;
}

int32_t OrderbookStats::bid_depth(std::optional<std::size_t> count) const noexcept
{
    return get_market_depth(orderbook_.ask_levels_, count);
}

int32_t OrderbookStats::ask_depth(std::optional<std::size_t> count) const noexcept
{
    return get_market_depth(orderbook_.ask_levels_, count);
}

template <typename T>
int32_t OrderbookStats::get_market_depth(T& levels, std::optional<std::size_t> count) const noexcept
{
    if (count) {
        auto v = views::reverse(levels)
               | views::take(*count)
               | views::transform([](const auto& elem) { return elem.second; });
        return rng::fold_right(v, 0, std::plus());
    } else {
        auto v = views::reverse(levels)
               | views::transform([](const auto& elem) { return elem.second; });
        return rng::fold_right(v, 0, std::plus());
    }
}

Price OrderbookStats::mid_price() const noexcept
{
    return (best_bid() + best_ask()) / 2;
}

int32_t OrderbookStats::absolute_spread() const noexcept
{
    return best_ask() - best_bid();
}

double OrderbookStats::relative_spread() const noexcept
{
    return absolute_spread() / mid_price();
}

double OrderbookStats::imbalance(std::optional<std::size_t> count) const noexcept
{
    auto bid_volume = bid_depth(*count);
    auto ask_volume = ask_depth(*count);

    return (bid_volume - ask_volume) / (bid_volume + ask_volume);
}



