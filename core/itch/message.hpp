#pragma once

#include <cstddef>
#include <cstdint>

struct SystemEventMessage
{
    char type; // 'S'
    uint16_t stock_locate; // Always 0
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char event_code; // 'O', 'S', 'Q', 'M', 'E', 'C'
} __attribute__((packed));

struct StockDirectoryMessage
{
    char type; // 'R'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char symbol[8];
    char market_category; // 'Q', 'G', 'S', 'N', 'A', 'P', 'Z', 'V', or ' '
    char financial_status_indicator; // 'D', 'E', 'Q', 'S', 'G', 'H', 'J', 'K', 'C', 'N', or ' '
    uint32_t round_lot_size;
    char round_lots_only; // 'Y' or 'N'
    char issue_classification;
    char issue_sub_type[2];
    char authenticity; // 'P' or 'T'
    char short_sale_threshold_indicator; // 'Y', 'N', or ' '
    char ipo_flag; // 'Y', 'N', or ' '
    char luld_reference_price_tier; // '1', '2', or ' '
    char etp_flag; // 'Y', 'N', or ' '
    uint32_t etp_leverage_factor;
    char inverse_indicator; // 'Y' or 'N'
} __attribute__((packed));

struct StockTradingActionMessage
{
    char type; // 'H'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char symbol[8];
    char trading_state; // 'H', 'P', 'Q', 'T'
    char reserved;
    char reason[4];
} __attribute__((packed));

struct RegSHORestrictionMessage
{
    char type; // 'Y'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char symbol[8];
    char reg_sho_action; // '0', '1', '2'
} __attribute__((packed));

struct MarketParticipantPositionMessage
{
    char type; // 'L'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char mpid[4];
    char symbol[8];
    char primary_market_maker; // 'Y' or 'N'
    char market_maker_mode; // 'N', 'P', 'S', 'R', 'L'
    char market_participant_state; // 'A', 'E', 'W', 'S', 'D'
} __attribute__((packed));

struct MWCBDeclineLevelMessage
{
    char type; // 'V'
    uint16_t stock_locate; // always 0
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t level_1;
    uint64_t level_2;
    uint64_t level_3;
} __attribute__((packed));

struct MWCBStatusMessage
{
    char type; // 'W'
    uint16_t stock_locate; // always 0
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char breached_level; // '1', '2', '3'
} __attribute__((packed));

struct QuotingPeriodUpdateMessage
{
    char type; // 'K'
    uint16_t stock_locate; // Always 0
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char symbol[8];
    uint32_t ipo_quotation_release_time; // seconds since midnight
    char ipo_quotation_release_qualifier; // 'A' or 'C'
    uint32_t ipo_price;
} __attribute__((packed));

struct LULDAuctionCollarMessage
{
    char type; // 'J'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char symbol[8];
    uint32_t auction_collar_reference_price;
    uint32_t upper_auction_collar_price;
    uint32_t lower_auction_collar_price;
    uint32_t auction_collar_extension;
} __attribute__((packed));

struct OperationalHaltMessage
{
    char type; // 'h'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char symbol[8];
    char market_code; // 'Q', 'B', 'X'
    char operational_halt_action; // 'H' or 'T'
} __attribute__((packed));

struct AddOrderMessage
{
    char type; // 'A'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
    char side;
    uint32_t num_shares;
    char symbol[8];
    uint32_t price;
} __attribute__((packed));

struct AddOrderMPIDMessage
{
    char type; // 'F'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
    char side;
    uint32_t num_shares;
    char symbol[8];
    uint32_t price;
    char mpid_attribute[4];
} __attribute__((packed));

struct OrderExecutedMessage
{
    char type; // 'E'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
    uint32_t num_executed_shares;
    uint64_t match_number;
} __attribute__((packed));

struct OrderExecutedWithPriceMessage
{
    char type; // 'C'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
    uint32_t executed_shares;
    uint64_t match_number;
    char printable; // 'Y' or 'N'
    uint32_t execution_price;
} __attribute__((packed));

struct OrderCancelMessage
{
    char type; // 'X'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
    uint32_t num_cancelled_shares;
} __attribute__((packed));

struct OrderDeleteMessage
{
    char type; // 'D'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
} __attribute__((packed));

struct OrderReplaceMessage
{
    char type; // 'U'
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
    char type; // 'P'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t reference_number;
    char side; // 'B' or 'S'
    uint32_t num_shares;
    char symbol[8];
    uint32_t price;
    uint64_t match_number;
} __attribute__((packed));

struct CrossTradeMessage
{
    char type; // 'Q'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t num_shares;
    char symbol[8];
    uint32_t cross_price;
    uint64_t match_number;
    char cross_type; // 'O', 'C', 'H'
} __attribute__((packed));

struct BrokenTradeMessage
{
    char type; // 'B'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t match_number;
} __attribute__((packed));

struct NOIIMessage
{
    char type; // 'I'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t num_paired_shares;
    uint64_t num_imbalanced_shares;
    char imbalance_direction;
    char symbol[8];
    uint32_t far_price;
    uint32_t near_price;
    uint32_t current_reference_price;
    char cross_type;
    char price_variation_indicator;
} __attribute__((packed));

struct RPPIMessage
{
    char type; // 'N'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char symbol[8];
    char interest_flag;
} __attribute__((packed));

struct DirectListingWithCapitalRaisePriceDiscoveryMessage
{
    char type; // 'O'
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char symbol[8];
    char open_eligibility_status;
    uint32_t min_allowable_price;
    uint32_t max_allowable_price;
    uint32_t near_execution_price;
    uint64_t near_execution_time;
    uint32_t lower_price_range_collar;
    uint32_t upper_price_range_collar;
} __attribute__((packed));