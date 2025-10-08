//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Protocol version                                                 |
//+------------------------------------------------------------------+
#define EXCHANGE_PROTOCOL_VERSION         3
//+------------------------------------------------------------------+
//| Default parameters                                               |
//+------------------------------------------------------------------+
#define DEFAULT_EXCHANGE_ADDRESS          L"127.0.0.1"            // default server address
#define DEFAULT_EXCHANGE_PORT             16838                   // default server port
//+------------------------------------------------------------------+
//| Message parameters                                               |
//+------------------------------------------------------------------+
#define MSG_SEPARATOR                     '\n'                    // messages separator
#define MSG_SEPARATOR_TAG                 '\1'                    // message tags separator
#define MSG_SEPARATOR_TAGVALUE            '='                     // tag name and tag value separator
#define MSG_MAX_SIZE                      10*1024                 // maximum size of message
//+------------------------------------------------------------------+
//| Message types                                                    |
//+------------------------------------------------------------------+
#define MSG_TYPE_UNKNOWN                  0                       // unknown type of message
#define MSG_TYPE_LOGIN                    1                       // login message
#define MSG_TYPE_LOGOUT                   2                       // logout message
#define MSG_TYPE_SYMBOL                   3                       // message with symbol settings
#define MSG_TYPE_TICK                     4                       // message with tick
#define MSG_TYPE_ORDER                    5                       // message with order
#define MSG_TYPE_HEARTBEAT                6                       // HeartBeat message
#define MSG_TYPE_DEAL                     7                       // message with deal
#define MSG_TYPE_BOOK                     8                       // message with book
#define MSG_TYPE_ACCOUNT_DATA_REQUEST     9                       // message with account data request
#define MSG_TYPE_ACCOUNT_DATA             10                      // message with all data for account
//+------------------------------------------------------------------+
//| Message header tags                                              |
//+------------------------------------------------------------------+
#define MSG_TAG_HEADER_VERSION            "ver="                  // message protocol version
#define MSG_TAG_HEADER_TYPE               "type="                 // message type
//+------------------------------------------------------------------+
//| Login message tags                                               |
//+------------------------------------------------------------------+
#define MSG_TAG_LOGIN_LOGIN               "login="                // login
#define MSG_TAG_LOGIN_PASSWORD            "password="             // password
#define MSG_TAG_LOGIN_RESULT              "res="                  // result
//+------------------------------------------------------------------+
//| Symbols message tags                                             |
//+------------------------------------------------------------------+
#define MSG_TAG_SYMBOL_INDEX              "index="                // symbol index
#define MSG_TAG_SYMBOL_SYMBOL             "symbol="               // symbol name
#define MSG_TAG_SYMBOL_PATH               "path="                 // path
#define MSG_TAG_SYMBOL_DESCRIPTION        "description="          // description
#define MSG_TAG_SYMBOL_PAGE               "page="                 // page
#define MSG_TAG_SYMBOL_CURRENCY_BASE      "currency_base="        // base currency
#define MSG_TAG_SYMBOL_CURRENCY_PROFIT    "currency_profit="      // profit currency
#define MSG_TAG_SYMBOL_CURRENCY_MARGIN    "currency_margin="      // margin currency
#define MSG_TAG_SYMBOL_DIGITS             "digits="               // digits
#define MSG_TAG_SYMBOL_TICK_FLAGS         "tick_flags="           // tick flags
#define MSG_TAG_SYMBOL_CALC_MODE          "calc_mode="            // calc mode
#define MSG_TAG_SYMBOL_EXEC_MODE          "exec_mode="            // exec mode
#define MSG_TAG_SYMBOL_CHART_MODE         "chart_mode="           // chart mode
#define MSG_TAG_SYMBOL_FILL_FLAGS         "fill_flags="           // fill flags
#define MSG_TAG_SYMBOL_EXPIR_FLAGS        "expir_flags="          // expiration flags
#define MSG_TAG_SYMBOL_TICK_VALUE         "tick_value="           // tick value
#define MSG_TAG_SYMBOL_TICK_SIZE          "tick_size="            // tick size
#define MSG_TAG_SYMBOL_CONTRACT_SIZE      "contract_size="        // contract size
#define MSG_TAG_SYMBOL_VOLUME_MIN         "volume_min="           // volume min
#define MSG_TAG_SYMBOL_VOLUME_MAX         "volume_max="           // volume max
#define MSG_TAG_SYMBOL_VOLUME_STEP        "volume_step="          // volume step
#define MSG_TAG_SYMBOL_MARKET_DEPTH       "market_depth="         // market depth
#define MSG_TAG_SYMBOL_MARGIN_FLAGS       "margin_flags="         // margin flags
#define MSG_TAG_SYMBOL_MARGIN_INITIAL     "margin_initial="       // margin initial
#define MSG_TAG_SYMBOL_MARGIN_MAINTENANCE "margin_maintenance="   // margin maintenance
#define MSG_TAG_SYMBOL_MARGIN_LONG        "margin_long="          // long rate
#define MSG_TAG_SYMBOL_MARGIN_SHORT       "margin_short="         // short rate
#define MSG_TAG_SYMBOL_MARGIN_LIMIT       "margin_limit="         // limit orders rate
#define MSG_TAG_SYMBOL_MARGIN_STOP        "margin_stop="          // stop orders rate
#define MSG_TAG_SYMBOL_MARGIN_STOP_LIMIT  "margin_stop_limit="    // stop limit orders rate
#define MSG_TAG_SYMBOL_SETTLEMENT_PRICE   "settlement_price="     // settlement price
#define MSG_TAG_SYMBOL_PRICE_LIMIT_MAX    "price_limit_max="      // price limit max
#define MSG_TAG_SYMBOL_PRICE_LIMIT_MIN    "price_limit_min="      // price limit min
#define MSG_TAG_SYMBOL_TIME_START         "time_start="           // trading start date
#define MSG_TAG_SYMBOL_TIME_EXPIRATION    "time_expiration="      // trading expiration date
#define MSG_TAG_SYMBOL_TRADE_MODE         "trade_mode="           // trade mode
//+------------------------------------------------------------------+
//| Tick message tags                                                |
//+------------------------------------------------------------------+
#define MSG_TAG_TICK_SYMBOL               "symbol="               // symbol
#define MSG_TAG_TICK_BANK                 "bank="                 // price source
#define MSG_TAG_TICK_BID                  "bid="                  // bid price
#define MSG_TAG_TICK_ASK                  "ask="                  // ask price
#define MSG_TAG_TICK_LAST                 "last="                 // last price
#define MSG_TAG_TICK_VOLUME               "volume="               // volume of last deal
#define MSG_TAG_TICK_DATETIME             "datetime="             // datetime
//+------------------------------------------------------------------+
//| Order message tags                                               |
//+------------------------------------------------------------------+
#define MSG_TAG_ORDER_ACTION_TYPE         "order_action="         // order action
#define MSG_TAG_ORDER_STATE               "state="                // order state
#define MSG_TAG_ORDER_MT_ID               "order="                // order ticket
#define MSG_TAG_ORDER_EXCHANGE_ID         "exchange_id="          // order id in external system
#define MSG_TAG_ORDER_CUSTOM_DATA         "custom_data="          // custom data
#define MSG_TAG_ORDER_REQUEST_ID          "request_id="           // request id
#define MSG_TAG_ORDER_SYMBOL              "symbol="               // symbol
#define MSG_TAG_ORDER_LOGIN               "login="                // client's login
#define MSG_TAG_ORDER_TYPE_ORDER          "type_order="           // order type
#define MSG_TAG_ORDER_TYPE_TIME           "type_time="            // order expiration type
#define MSG_TAG_ORDER_ACTION              "action="               // action
#define MSG_TAG_ORDER_PRICE_ORDER         "price_order="          // order price
#define MSG_TAG_ORDER_PRICE_SL            "price_sl="             // Stop Loss level
#define MSG_TAG_ORDER_PRICE_TP            "price_tp="             // Take Profit level
#define MSG_TAG_ORDER_PRICE_TICK_BID      "price_tick_bid="       // symbol bid price in external trading system
#define MSG_TAG_ORDER_PRICE_TICK_ASK      "price_tick_ask="       // symbol ask price in external trading system
#define MSG_TAG_ORDER_VOLUME              "volume="               // order volume
#define MSG_TAG_ORDER_EXPIRATION_TIME     "expiration_time="      // expiration time
#define MSG_TAG_ORDER_RESULT              "result="               // result of message processing
//+------------------------------------------------------------------+
//| Position message tags                                            |
//+------------------------------------------------------------------+
#define MSG_TAG_POSITION_SYMBOL           "pos_symbol="           // symbol
#define MSG_TAG_POSITION_LOGIN            "pos_login="            // client's login
#define MSG_TAG_POSITION_PRICE            "pos_price="            // position opening price
#define MSG_TAG_POSITION_VOLUME           "pos_volume="           // position volume
#define MSG_TAG_POSITION_DIGITS           "pos_digits="           // price digits
//+------------------------------------------------------------------+
//| Account data message tags                                        |
//+------------------------------------------------------------------+
#define MSG_TAG_ACCOUNT_DATA_LOGIN        "acc_login="            // login
#define MSG_TAG_ACCOUNT_DATA_BALANCE      "acc_balance="          // balance
#define MSG_TAG_ACCOUNT_DATA_ORDERS_AMOUNT "acc_orders_amount="   // orders amount
#define MSG_TAG_ACCOUNT_DATA_POSITIONS_AMOUNT "acc_positions_amount="// positions amount
//+------------------------------------------------------------------+
//| Deal message tags                                                |
//+------------------------------------------------------------------+
#define MSG_TAG_DEAL_EXCHANGE_ID          "exchange_id="          // deal id in external system
#define MSG_TAG_DEAL_ORDER                "order="                // order ticket
#define MSG_TAG_DEAL_SYMBOL               "symbol="               // symbol
#define MSG_TAG_DEAL_LOGIN                "login="                // login
#define MSG_TAG_DEAL_TYPE                 "type_deal="            // action
#define MSG_TAG_DEAL_VOLUME               "volume="               // volume
#define MSG_TAG_DEAL_VOLUME_REM           "volume_rem="           // volume remaind
#define MSG_TAG_DEAL_PRICE                "price="                // price
//+------------------------------------------------------------------+
//| Book message tags                                               |
//+------------------------------------------------------------------+
#define MSG_TAG_BOOK_SYMBOL               "symbol="               // symbol
#define MSG_TAG_BOOK_FLAGS                "flags="                // flags
#define MSG_TAG_BOOK_DATETIME             "datetime="             // action
#define MSG_TAG_BOOK_ITEMS_TOTAL          "items_total="          // items total
#define MSG_TAG_BOOK_ITEM_TYPE            "item_type_%u="         // item type
#define MSG_TAG_BOOK_ITEM_PRICE           "item_price_%u="        // item price
#define MSG_TAG_BOOK_ITEM_VOLUME          "item_volume_%u="       // item volume
//+------------------------------------------------------------------+
