#pragma once

#include <cstddef>
#include <cstdint>

struct SystemEventMessage
{
    uint8_t type; // 'S'
    uint16_t stock_locate; // Always 0
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint8_t event_code; // 'O', 'S', 'Q', 'M', 'E', 'C'
} __attribute__((packed));

// important for mapping equities for the trading day
struct StockDirectoryMessage
{
    uint8_t type; // 'R'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint8_t symbol[8];
    uint8_t market_category; // 'Q', 'G', 'S', 'N', 'A', 'P', 'Z', 'V', or ' '
    uint8_t financial_status_indicator; // 'D', 'E', 'Q', 'S', 'G', 'H', 'J', 'K', 'C', 'N', or ' '
    uint32_t round_lot_size;
    uint8_t round_lots_only; // 'Y' or 'N'
    uint8_t issue_classification;
    uint8_t issue_sub_type[2];
    uint8_t authenticity; // 'P' or 'T'
    uint8_t short_sale_threshold_indicator; // 'Y', 'N', or ' '
    uint8_t ipo_flag; // 'Y', 'N', or ' '
    uint8_t luld_reference_price_tier; // '1', '2', or ' '
    uint8_t etp_flag; // 'Y', 'N', or ' '
    uint32_t etp_leverage_factor;
    uint8_t inverse_indicator; // 'Y' or 'N'
} __attribute__((packed));

struct StockTradingActionMessage
{
    uint8_t type; // 'H'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint8_t symbol[8];
    uint8_t trading_state; // 'H', 'P', 'Q', 'T'
    uint8_t reserved;
    uint8_t reason[4];
} __attribute__((packed));

struct RegSHORestrictionMessage
{
    uint8_t type; // 'Y'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint8_t symbol[8];
    uint8_t reg_sho_action; // '0', '1', '2'
} __attribute__((packed));

struct MarketParticipantPositionMessage
{
    uint8_t type; // 'L'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint8_t mpid[4];
    uint8_t symbol[8];
    uint8_t primary_market_maker; // 'Y' or 'N'
    uint8_t market_maker_mode; // 'N', 'P', 'S', 'R', 'L'
    uint8_t market_participant_state; // 'A', 'E', 'W', 'S', 'D'
} __attribute__((packed));

struct MWCBDeclineLevelMessage
{
    uint8_t type; // 'V'
    uint16_t stock_locate; // always 0
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t level_1;
    uint64_t level_2;
    uint64_t level_3;
} __attribute__((packed));

struct MWCBStatusMessage
{
    uint8_t type; // 'W'
    uint16_t stock_locate; // always 0
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint8_t breached_level; // '1', '2', '3'
} __attribute__((packed));

struct QuotingPeriodUpdateMessage
{
    uint8_t type; // 'K'
    uint16_t stock_locate; // Always 0
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint8_t symbol[8];
    uint32_t ipo_quotation_release_time; // seconds since midnight
    uint8_t ipo_quotation_release_qualifier; // 'A' or 'C'
    uint32_t ipo_price;
} __attribute__((packed));

struct LULDAuctionCollarMessage
{
    uint8_t type; // 'J'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint8_t symbol[8];
    uint32_t auction_collar_reference_price;
    uint32_t upper_auction_collar_price;
    uint32_t lower_auction_collar_price;
    uint32_t auction_collar_extension;
} __attribute__((packed));

struct OperationalHaltMessage
{
    uint8_t type; // 'h'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint8_t symbol[8];
    uint8_t market_code; // 'Q', 'B', 'X'
    uint8_t operational_halt_action; // 'H' or 'T'
} __attribute__((packed));

struct AddOrderMessage
{
    uint8_t type; // 'A'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
    uint8_t side;
    uint32_t num_shares;
    uint8_t symbol[8];
    uint32_t price;
} __attribute__((packed));

struct AddOrderMPIDMessage
{
    uint8_t type; // 'F'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
    uint8_t side;
    uint32_t num_shares;
    uint8_t symbol[8];
    uint32_t price;
    uint8_t mpid_attribute[4];
} __attribute__((packed));

struct OrderExecutedMessage
{
    uint8_t type; // 'E'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
    uint32_t num_executed_shares;
    uint64_t match_number;
} __attribute__((packed));

struct OrderExecutedWithPriceMessage
{
    uint8_t type; // 'C'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
    uint32_t executed_shares;
    uint64_t match_number;
    uint8_t printable; // 'Y' or 'N'
    uint32_t execution_price;
} __attribute__((packed));

struct OrderCancelMessage
{
    uint8_t type; // 'X'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
    uint32_t num_cancelled_shares;
} __attribute__((packed));

struct OrderDeleteMessage
{
    uint8_t type; // 'D'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
} __attribute__((packed));

struct OrderReplaceMessage
{
    uint8_t type; // 'U'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t original_reference_number;
    uint64_t new_reference_number;
    uint32_t num_shares;
    uint32_t price;
} __attribute__((packed));

struct NonCrossTradeMessage
{
    uint8_t type; // 'P'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
    uint8_t side; // 'B' or 'S'
    uint32_t num_shares;
    uint8_t symbol[8];
    uint32_t price;
    uint64_t match_number;
} __attribute__((packed));

struct CrossTradeMessage
{
    uint8_t type; // 'Q'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t num_shares;
    uint8_t symbol[8];
    uint32_t cross_price;
    uint64_t match_number;
    uint8_t cross_type; // 'O', 'C', 'H'
} __attribute__((packed));

struct BrokenTradeMessage
{
    uint8_t type; // 'B'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t match_number;
} __attribute__((packed));

struct NOIIMessage
{
    uint8_t type; // 'I'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t num_paired_shares;
    uint64_t num_imbalanced_shares;
    uint8_t imbalance_direction;
    uint8_t symbol[8];
    uint32_t far_price;
    uint32_t near_price;
    uint32_t current_reference_price;
    uint8_t cross_type;
    uint8_t price_variation_indicator;
} __attribute__((packed));

struct RPPIMessage
{
    uint8_t type; // 'N'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint8_t symbol[8];
    uint8_t interest_flag;
} __attribute__((packed));

struct DirectListingWithCapitalRaisePriceDiscoveryMessage
{
    uint8_t type; // 'O'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint8_t symbol[8];
    uint8_t open_eligibility_status;
    uint32_t min_allowable_price;
    uint32_t max_allowable_price;
    uint32_t near_execution_price;
    uint64_t near_execution_time;
    uint32_t lower_price_range_collar;
    uint32_t upper_price_range_collar;
} __attribute__((packed));

using MessageVariant = std::variant<
    std::monostate,
    SystemEventMessage,
    StockDirectoryMessage,
    AddOrderMessage,
    OrderCancelMessage,
    OrderDeleteMessage,
    OrderReplaceMessage
>;

alignas(8) struct ExchangeMessage
{
    std::size_t type_index;
    MessageVariant payload;

    constexpr ExchangeMessage(std::size_t idx, MessageVariant&& p) noexcept :
        type_index{ idx }, payload{ std::move(p) } {}
};