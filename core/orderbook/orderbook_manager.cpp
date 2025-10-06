#include "filesystem"

#include "orderbook/orderbook_manager.hpp"
#include "parser/msg_types.hpp"
#include "common/json_parser.hpp"
#include "common/thread_utils.hpp"

OrderbookManager::OrderbookManager() : running_{ true }
{
    std::filesystem::path config_path = std::getenv("TRADING_ENGINE_HOME");
    config_path /= "config.json";
    if (config_path.empty())
        throw std::runtime_error{ "config.json file not found" };
    auto tickers = parse_tickers_from_json(config_path.string());

    auto& msg_queues = MessageQueues<ExchangeMessage>::get_instance();
    msg_queues.add_queues(*tickers);

    for (const auto& ticker : *tickers) {
        orderbooks_.emplace(ticker, L2::OrderBook{});

        auto orderbook_worker = [this, ticker, &msg_queues] {

            while (running_.load()) {

                ExchangeMessage msg{};
                while (msg_queues[ticker]->try_pop(msg)) {

                    switch (msg.type_index_) {
                        case 3: {
                            auto& add_order = std::get<3>(msg.payload_);
                            orderbooks_.at(ticker).process_order(Action::Add, add_order.price, add_order.num_shares, add_order.side);
                            break;
                        }

                        // all other cases...
                    }
                }
            }
        };

        auto available_cores = get_all_cores();
        auto core = available_cores.pick_one(); // this will decrement from global pool of available cores that threads can choose from to set affinity on?

        auto* orderbook_thread = common::create_and_pin_thread()




    }
}