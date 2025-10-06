#include "orderbook_manager.hpp"

struct ExchangeMessage;

OrderbookManager::OrderbookManager(const std::vector<std::string>& tickers)
{
    auto& msg_queues = MessageQueues<ExchangeMessage>::get_instance();
    msg_queues.add_queues(tickers);

    for (const auto& ticker : tickers) {
        orderbooks.emplace(ticker, L2::OrderBook{});
        
        auto worker = [ticker] {
            // atomic flag? condition variable? How to signal to all threads owned by this object that we are done
            // with the message queue processing loop, otherwise these threads will never join

            while (/* atomic flag or condition var? */) {
                // read message queue
                // send to proper OB
            }
        };
    }
}