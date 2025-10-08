//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "PluginContext.h"
#pragma once
//+------------------------------------------------------------------+
//| Class for formatting one Order record                            |
//+------------------------------------------------------------------+
class CNFAOrderRecord
  {
private:
   uint32_t          m_trans_type;
   uint32_t          m_order_type;
   WCHAR             m_verb;
   CMTStr64          m_order_id;
   CMTStr16          m_product_code;
   double            m_bid_price;
   double            m_ask_price;
   double            m_limit_price;
   double            m_stop_price;
   uint32_t          m_duration;
   int64_t           m_expiry_date;
   uint32_t          m_origin;
   uint64_t          m_quantity;
   uint64_t          m_remaining_qty;
   uint64_t          m_custacc_id;
   //--- digits
   uint32_t          m_digits;

public:
   //--- transaction type
   enum EnTransactionType
     {
      TRANS_UNDEFINED      =0,
      TRANS_ORDER_ADDED    =1,
      TRANS_ORDER_MODIFY   =2,
      TRANS_ORDER_FILL     =3,
      TRANS_PARTIAL_FILL   =4,
      TRANS_ORDER_CANCELLED=5,
      TRANS_ORDER_DELETED  =6,
      TRANS_DLR_OFFSET     =7,
      TRANS_STOP_ADJUST    =8
     };
   //--- order type
   enum EnOrderType
     {
      ORDER_MARKET         =0,
      ORDER_STOP           =1,
      ORDER_LIMIT          =2,
      ORDER_STOP_LIMIT     =3
     };
   //--- duration
   enum EnDuration
     {
      DURATION_GTC         =0,
      DURATION_GTD         =1,
      DURATION_GFD         =2,
      DURATION_FOK         =3,
      DURATION_IOC         =4
     };
   //--- origin
   enum EnOrigin
     {
      ORIGIN_AUTO          =0,
      ORIGIN_CUST          =1,
      ORIGIN_DLR           =2,
      ORIGIN_SYSTEM        =3
     };

public:
                     CNFAOrderRecord(void);
                    ~CNFAOrderRecord(void);
   //--- check empty
   bool              Empty(void) const;
   //--- access
   LPCWSTR           OrderID(void) const;
   uint32_t          Origin(void) const;
   //--- format record to string
   LPCWSTR           Print(CMTStr &str,const tm &ttm_batch,const tm &ttm_stamp,const PluginContext &context) const;
   //--- filling data
   void              FillMarket(const uint32_t trans_type,const bool instant,const IMTOrder *order,const uint32_t origin);
   void              FillPending(const uint32_t trans_type,const IMTOrder *order,const EnOrigin origin);
   void              FillStopLoss(const uint32_t trans_type,const uint64_t id,const double price,const uint64_t volume_ext,const double contract_size,const IMTPosition *position,const uint32_t origin);
   void              FillTakeProfit(const uint32_t trans_type,const uint64_t id,const double price,const uint64_t volume_ext,const double contract_size,const IMTPosition *position,const uint32_t origin);
   //--- change some fields
   void              SetTransType(const uint32_t trans_type);
   void              SetPartial(const IMTOrder *order,const IMTDeal *deal);
   void              SetOrderType(const EnOrderType order_type);
   void              SetBidAsk(const IMTRequest *request);
   void              SetBidAsk(const MTTickShort &tick);

private:
   uint32_t          ReasonToOrigin(const uint32_t origin,const uint32_t reason) const;
   //--- return Enums as string constants
   LPCWSTR           PrintTransType(void) const;
   LPCWSTR           PrintOrderType(void) const;
   LPCWSTR           PrintDuration(void) const;
   LPCWSTR           PrintOrigin(void) const;
   LPCWSTR           PrintExpiration(CMTStr &str,const int32_t tz_minutes) const;
  };
//+------------------------------------------------------------------+
