//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "NFATradeRecord.h"
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CNFATradeRecord::CNFATradeRecord(void) : m_trade_id(0),m_verb(L' '),
                                         m_bid_price(0.0),m_ask_price(0.0),m_quantity(0),m_remaining_qty(0),
                                         m_fill_price(0.0),m_commission(0.0),m_digits(5),m_digits_currency(2)
  {
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CNFATradeRecord::~CNFATradeRecord(void)
  {
  }
//+------------------------------------------------------------------+
//| Check record was filled                                          |
//+------------------------------------------------------------------+
bool CNFATradeRecord::Empty(void) const
  {
   return(m_order_id.Empty());
  }
//+------------------------------------------------------------------+
//| Fill record from IMTDeal data                                    |
//+------------------------------------------------------------------+
void CNFATradeRecord::Fill(LPCWSTR order_id,const IMTDeal *deal)
  {
   if(!deal || !deal->Deal())
      return;
//--- Set verb and check deal action
   switch(deal->Action())
     {
      case IMTDeal::DEAL_BUY:
         m_verb=L'B';
         break;
      case IMTDeal::DEAL_SELL:
         m_verb=L'S';
         break;
      default:
         return;
     }
//---
   m_order_id.Assign(order_id);
   m_product_code.Assign(deal->Symbol());
   m_trade_id  =deal->Deal();
   m_quantity  =uint64_t(SMTMath::VolumeExtToSize(deal->VolumeExt(),deal->ContractSize()));
   m_fill_price=deal->Price();
   m_commission=-deal->Commission();
//--- digits
   m_digits         =deal->Digits();
   m_digits_currency=deal->DigitsCurrency();
  }
//+------------------------------------------------------------------+
//| Reset quantities for partial filling                             |
//+------------------------------------------------------------------+
void CNFATradeRecord::SetPartial(const IMTOrder *order,const IMTDeal *deal)
  {
   if(order && deal)
     {
      m_quantity     =uint64_t(SMTMath::VolumeExtToSize(deal->VolumeExt(),        deal->ContractSize()));
      m_remaining_qty=uint64_t(SMTMath::VolumeExtToSize(order->VolumeCurrentExt(),order->ContractSize()));
     }
  }
//+------------------------------------------------------------------+
//| Format record to string                                          |
//+------------------------------------------------------------------+
LPCWSTR CNFATradeRecord::Print(CMTStr &str,tm &ttm_batch,tm &ttm_stamp,const PluginContext &context) const
  {
   CMTStr64 sbatch_date,stradematch,sremaining_qty,sbid,sask,sfill_price,scommission;
//---
   if(m_remaining_qty)
      sremaining_qty.Format(L"%I64u",m_remaining_qty);
//---
   SMTFormat::FormatDouble(sbid       ,m_bid_price ,m_digits);
   SMTFormat::FormatDouble(sask       ,m_ask_price ,m_digits);
   SMTFormat::FormatDouble(sfill_price,m_fill_price,m_digits);
   SMTFormat::FormatDouble(scommission,m_commission,m_digits_currency);
//--- format
   str.Format(L"%s|"     // BATCH_DATE
               "%I64u|"  // TRADE_ID
               "%s|"     // ORDER_ID
               "%s||"    // FDM_ID [MKT_SEG_ID]
               "%s|F|"   // TIME_TRADEMATCH|PRODUCT_CAT=F
               "%s|||||" // PRODUCT_CODE [CONTRACT_YEAR|CONTRACT_MONTH|CONTRACT_DAY|STRIKE|OPTION_TYPE]
               "%c|"     // VERB
               "%s|"     // BID_PRICE
               "%s|"     // ASK_PRICE
               "%I64u|"  // QUANTITY
               "%s|"     // REMAINING_QTY
               "%s||"    // FILL_PRICE [CONTRA_FILL_PRICE]
               "%s|||"   // SERVER_ID [IMPLIED_VOLATILITY|IB_REBATE]
               "%s",     // COMMISSION
               SPluginHelpers::FormatBatchDate(sbatch_date,ttm_batch),
               m_trade_id,
               m_order_id.Str(),
               context.fdm_id.Str(),
               SPluginHelpers::FormatStampDatetime(stradematch,ttm_stamp),
               m_product_code.Str(),
               m_verb,
               sbid.Str(),
               sask.Str(),
               m_quantity,
               sremaining_qty.Str(),
               sfill_price.Str(),
               context.server_id.Str(),
               scommission.Str());
//---
   return str.Str();
  }
//+------------------------------------------------------------------+
//| Setup current market prices from request                         |
//+------------------------------------------------------------------+
void CNFATradeRecord::SetBidAsk(const IMTRequest *request)
  {
   if(request)
     {
      m_bid_price=request->ResultMarketBid();
      m_ask_price=request->ResultMarketAsk();
     }
  }
//+------------------------------------------------------------------+
//| Setup current market prices from tick                            |
//+------------------------------------------------------------------+
void CNFATradeRecord::SetBidAsk(const MTTickShort &tick)
  {
   m_bid_price=tick.bid;
   m_ask_price=tick.ask;
  }
//+------------------------------------------------------------------+
