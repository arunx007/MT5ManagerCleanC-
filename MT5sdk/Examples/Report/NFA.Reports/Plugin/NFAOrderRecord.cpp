//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "NFAOrderRecord.h"
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CNFAOrderRecord::CNFAOrderRecord(void) : m_trans_type(TRANS_UNDEFINED),m_order_type(ORDER_MARKET),
                                         m_verb(L' '),m_bid_price(0.0),m_ask_price(0.0),m_limit_price(0.0),m_stop_price(0.0),
                                         m_duration(DURATION_FOK),m_expiry_date(0),m_origin(ORIGIN_SYSTEM),m_quantity(0),m_remaining_qty(0),m_custacc_id(0),
                                         m_digits(5)
  {
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CNFAOrderRecord::~CNFAOrderRecord(void)
  {
  }
//+------------------------------------------------------------------+
//| Check order record was filled                                    |
//+------------------------------------------------------------------+
bool CNFAOrderRecord::Empty(void) const
  {
   return(m_trans_type==TRANS_UNDEFINED);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
LPCWSTR CNFAOrderRecord::OrderID(void) const
  {
   return(m_order_id.Str());
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
uint32_t CNFAOrderRecord::Origin(void) const
  {
   return(m_origin);
  }
//+------------------------------------------------------------------+
//| Format record to string                                          |
//+------------------------------------------------------------------+
LPCWSTR CNFAOrderRecord::Print(CMTStr &str,const tm &ttm_batch,const tm &ttm_stamp,const PluginContext &context) const
  {
   CMTStr64 sbatch_date,sdatetime,sremaining_qty,sprice_limit,sprice_stop,sexpr,sbid,sask;
//--- optional fields
   if(m_trans_type==TRANS_PARTIAL_FILL)
      sremaining_qty.Format(L"%I64u",m_remaining_qty);
   if(m_order_type==ORDER_LIMIT || m_order_type==ORDER_STOP_LIMIT)
      SMTFormat::FormatDouble(sprice_limit,m_limit_price,m_digits);
   if(m_order_type==ORDER_STOP || m_order_type==ORDER_STOP_LIMIT)
      SMTFormat::FormatDouble(sprice_stop,m_stop_price,m_digits);
//--- bid/ask
   SMTFormat::FormatDouble(sbid,m_bid_price,m_digits);
   SMTFormat::FormatDouble(sask,m_ask_price,m_digits);
//--- format
   str.Format(L"%s|"             // BATCH_DATE
               "%s|"             // TRANSACTION_DATETIME (stamp)
               "%s|"             // ORDER_ID
               "%s||"            // FDM_ID [MKT_SEG_ID]
               "%s|F||||||"      // PRODUCT_CODE|PRODUCT_CAT=F [CONTRACT_YEAR|CONTRACT_MONTH|CONTRACT_DAY|STRIKE|OPTION_TYPE]
               "%s|"             // TRANSACTION_TYPE
               "%s|"             // ORDER_TYPE
               "%c|"             // VERB
               "%s|"             // BID_PRICE
               "%s|"             // ASK_PRICE
               "%I64u|"          // QUANTITY
               "%s|"             // REMAINING_QTY
               "%s|"             // PRICE
               "%s||||"          // STOP_PRICE [STOP_PRODUCT_CODE|TRAIL_AMT|LIMIT_OFFSET]
               "%s|"             // DURATION
               "%s|"             // EXPIRY_DATE
               "%s||"            // ORDER_ORIGIN [MANAGER_ID]
               "%I64u|"          // CUSTACCT_ID
               "%s||||",         // SERVER_ID [CUST_GROUP|LINKED_ORDER_ID|LINK_REASON|OPEN_CLOSE]
               SPluginHelpers::FormatBatchDate(sbatch_date,ttm_batch),
               SPluginHelpers::FormatStampDatetime(sdatetime,ttm_stamp),
               m_order_id.Str(),
               context.fdm_id.Str(),
               m_product_code.Str(),
               PrintTransType(),
               PrintOrderType(),
               m_verb,
               sbid.Str(),
               sask.Str(),
               m_quantity,
               sremaining_qty.Str(),
               sprice_limit.Str(),
               sprice_stop.Str(),
               (m_order_type==ORDER_MARKET) ? L"" : PrintDuration(),
               PrintExpiration(sexpr,context.server_timezone),
               PrintOrigin(),
               m_custacc_id,
               context.server_id.Str());
//---
   return str.Str();
  }
//+------------------------------------------------------------------+
//| Filling by IMTOrder data, for market orders                      |
//+------------------------------------------------------------------+
void CNFAOrderRecord::FillMarket(const uint32_t trans_type,const bool instant,const IMTOrder *order,const uint32_t origin)
  {
   if(!order || !order->Order())
      return;
//--- check type, fill m_verb
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         m_verb=L'B';
         break;
      case IMTOrder::OP_SELL:
         m_verb=L'S';
         break;
      default:
         return;
     }
//--- fill
   m_order_id.Format(L"%I64u",order->Order());
   m_product_code.Assign(order->Symbol());
   m_trans_type      =trans_type;
   m_order_type      =instant ? ORDER_LIMIT : ORDER_MARKET;
   m_limit_price     =order->PriceOrder();
   m_duration        =(order->TypeFill()==IMTOrder::ORDER_FILL_FOK) ? DURATION_FOK : DURATION_IOC;
   m_expiry_date     =order->TimeExpiration();
   m_origin          =ReasonToOrigin(origin,order->Reason());
   m_quantity        =uint64_t(SMTMath::VolumeExtToSize(order->VolumeInitialExt(),order->ContractSize()));
   m_custacc_id      =order->Login();
   m_digits          =order->Digits();
  }
//+------------------------------------------------------------------+
//| Filling by IMTOrder data, for pending orders                     |
//+------------------------------------------------------------------+
void CNFAOrderRecord::FillPending(const uint32_t trans_type,const IMTOrder *order,const EnOrigin origin)
  {
   if(!order || !order->Order())
      return;
//--- check type, fill m_verb, m_order_type and prices
   switch(order->Type())
     {
      case IMTOrder::OP_BUY_LIMIT:
         m_verb       =L'B';
         m_order_type =ORDER_LIMIT;
         m_limit_price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT:
         m_verb       =L'S';
         m_order_type =ORDER_LIMIT;
         m_limit_price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP:
         m_verb      =L'B';
         m_order_type=ORDER_STOP;
         m_stop_price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP:
         m_verb      =L'S';
         m_order_type=ORDER_STOP;
         m_stop_price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT:
         m_verb       =L'B';
         m_order_type =ORDER_STOP_LIMIT;
         m_stop_price =order->PriceOrder();
         m_limit_price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         m_verb       =L'S';
         m_order_type =ORDER_STOP_LIMIT;
         m_stop_price =order->PriceOrder();
         m_limit_price=order->PriceTrigger();
         break;
      default:
         return;
     }
//--- set duration
   switch(order->TypeTime())
     {
      case IMTOrder::ORDER_TIME_DAY:
         m_duration=DURATION_GFD;
         break;
      case IMTOrder::ORDER_TIME_SPECIFIED:
      case IMTOrder::ORDER_TIME_SPECIFIED_DAY:
         m_duration=DURATION_GTD;
         break;
      default:
         m_duration=DURATION_GTC;
         break;
     }
//--- fill
   m_order_id.Format(L"%I64u",order->Order());
   m_product_code.Assign(order->Symbol());
   m_trans_type      =trans_type;
   m_expiry_date     =order->TimeExpiration();
   m_origin          =ReasonToOrigin(origin,order->Reason());
   m_quantity        =uint64_t(SMTMath::VolumeExtToSize(order->VolumeInitialExt(),order->ContractSize()));
   m_custacc_id      =order->Login();
   m_digits          =order->Digits();
  }
//+------------------------------------------------------------------+
//| Fill with IMTPosition, for STOP LOSS                             |
//+------------------------------------------------------------------+
void CNFAOrderRecord::FillStopLoss(const uint32_t trans_type,const uint64_t id,const double price,const uint64_t volume_ext,const double contract_size,const IMTPosition *position,const uint32_t origin)
  {
   if(!position || !position->Position())
      return;
//--- fill
   m_order_id.Format(L"%I64u_SL_%I64u",position->Position(),id);
   m_product_code.Assign(position->Symbol());
   m_trans_type      =trans_type;
   m_order_type      =ORDER_STOP;
   m_verb            =(position->Action()==IMTPosition::POSITION_BUY) ? L'S' : L'B';
   m_stop_price      =price;
   m_duration        =DURATION_GTC;
   m_origin          =origin;
   m_quantity        =uint64_t(SMTMath::VolumeExtToSize(volume_ext,contract_size));
   m_custacc_id      =position->Login();
   m_digits          =position->Digits();
  }
//+------------------------------------------------------------------+
//| Fill with IMTPosition, for TAKE PROFIT                           |
//+------------------------------------------------------------------+
void CNFAOrderRecord::FillTakeProfit(const uint32_t trans_type,const uint64_t id,const double price,const uint64_t volume_ext,const double contract_size,const IMTPosition *position,const uint32_t origin)
  {
   if(!position || !position->Position())
      return;
//--- fill
   m_order_id.Format(L"%I64u_TP_%I64u",position->Position(),id);
   m_product_code.Assign(position->Symbol());
   m_trans_type      =trans_type;
   m_order_type      =ORDER_LIMIT;
   m_verb            =(position->Action()==IMTPosition::POSITION_BUY) ? L'S' : L'B';
   m_limit_price      =price;
   m_duration        =DURATION_GTC;
   m_origin          =origin;
   m_quantity        =uint64_t(SMTMath::VolumeExtToSize(volume_ext,contract_size));
   m_custacc_id      =position->Login();
   m_digits          =position->Digits();
  }
//+------------------------------------------------------------------+
//| Reset quantities for partial filling                             |
//+------------------------------------------------------------------+
void CNFAOrderRecord::SetTransType(const uint32_t trans_type)
  {
   m_trans_type=trans_type;
  }
//+------------------------------------------------------------------+
//| Reset quantities for partial filling                             |
//+------------------------------------------------------------------+
void CNFAOrderRecord::SetPartial(const IMTOrder *order,const IMTDeal *deal)
  {
   if(order && deal)
     {
      m_quantity     =uint64_t(SMTMath::VolumeExtToSize(deal->VolumeExt()        ,deal->ContractSize()));
      m_remaining_qty=uint64_t(SMTMath::VolumeExtToSize(order->VolumeCurrentExt(),order->ContractSize()));
     }
  }
//+------------------------------------------------------------------+
//| Reset order type                                                 |
//+------------------------------------------------------------------+
void CNFAOrderRecord::SetOrderType(const EnOrderType order_type)
  {
   m_order_type=order_type;
  }
//+------------------------------------------------------------------+
//| Setup current market prices from request                         |
//+------------------------------------------------------------------+
void CNFAOrderRecord::SetBidAsk(const IMTRequest *request)
  {
   if(request)
     {
      m_bid_price=request->ResultMarketBid();
      m_ask_price=request->ResultMarketAsk();
     }
  }
//+------------------------------------------------------------------+
//| Setup current market prices from request                         |
//+------------------------------------------------------------------+
void CNFAOrderRecord::SetBidAsk(const MTTickShort &tick)
  {
   m_bid_price=tick.bid;
   m_ask_price=tick.ask;
  }
//+------------------------------------------------------------------+
//| Convert order/position reason to NFA origin                      |
//+------------------------------------------------------------------+
uint32_t CNFAOrderRecord::ReasonToOrigin(const uint32_t origin,const uint32_t reason) const
  {
//--- is auto?
   if(origin==ORIGIN_AUTO)
     {
      switch(reason)
        {
         case IMTOrder::ORDER_REASON_CLIENT:
         case IMTOrder::ORDER_REASON_EXPERT:
         case IMTOrder::ORDER_REASON_MOBILE:
         case IMTOrder::ORDER_REASON_WEB:
            return(ORIGIN_CUST);
         case IMTOrder::ORDER_REASON_DEALER:
            return(ORIGIN_DLR);
        }
     }
   else
      return(origin);
//--- default
   return(ORIGIN_SYSTEM);
  }
//+------------------------------------------------------------------+
//| Convert to string                                                |
//+------------------------------------------------------------------+
LPCWSTR CNFAOrderRecord::PrintTransType(void) const
  {
   switch(m_trans_type)
     {
      case TRANS_ORDER_ADDED    : return(L"ORDER ADDED");
      case TRANS_ORDER_MODIFY   : return(L"ORDER MODIFY");
      case TRANS_ORDER_FILL     : return(L"ORDER FILL");
      case TRANS_PARTIAL_FILL   : return(L"PARTIAL FILL");
      case TRANS_ORDER_CANCELLED: return(L"ORDER CANCELLED");
      case TRANS_ORDER_DELETED  : return(L"ORDER DELETED");
      case TRANS_DLR_OFFSET     : return(L"DLR OFFSET");
      case TRANS_STOP_ADJUST    : return(L"STOP ADJUST");
      default:
         break;
     }
   return(L"");
  }
//+------------------------------------------------------------------+
//| Convert to string                                                |
//+------------------------------------------------------------------+
LPCWSTR CNFAOrderRecord::PrintOrderType(void) const
  {
   switch(m_order_type)
     {
      case ORDER_MARKET    : return(L"MARKET");
      case ORDER_STOP      : return(L"STOP");
      case ORDER_LIMIT     : return(L"LIMIT");
      case ORDER_STOP_LIMIT: return(L"STOP LIMIT");
      default:
         break;
     }
   return(L"");
  }
//+------------------------------------------------------------------+
//| Convert to string                                                |
//+------------------------------------------------------------------+
LPCWSTR CNFAOrderRecord::PrintDuration(void) const
  {
   switch(m_duration)
     {
      case DURATION_GTC: return(L"GTC");
      case DURATION_GTD: return(L"GTD");
      case DURATION_GFD: return(L"GFD");
      case DURATION_FOK: return(L"FOK");
      case DURATION_IOC: return(L"IOC");
      default:
         break;
     }
   return(L"");
  }
//+------------------------------------------------------------------+
//| Convert to string                                                |
//+------------------------------------------------------------------+
LPCWSTR CNFAOrderRecord::PrintOrigin(void) const
  {
   switch(m_origin)
     {
      case ORIGIN_CUST: return(L"CUST");
      case ORIGIN_DLR : return(L"DLR");
     }
   return(L"SYSTEM");
  }
//+------------------------------------------------------------------+
//| Convert to string                                                |
//+------------------------------------------------------------------+
LPCWSTR CNFAOrderRecord::PrintExpiration(CMTStr &str,int tz_minutes) const
  {
//--- expire date exist?
   if(m_expiry_date && m_duration==DURATION_GTD)
     {
      tm ttm={};
      _gmtime64_s(&ttm,&m_expiry_date);
      SPluginHelpers::TimeToEST(ttm,tz_minutes);
      return SPluginHelpers::FormatStampDatetime(str,ttm);
     }
   return(L"");
  }
//+------------------------------------------------------------------+
