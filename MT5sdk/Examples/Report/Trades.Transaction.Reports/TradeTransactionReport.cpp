//+------------------------------------------------------------------+
//|                          MetaTrader 5 Trades.Transaction.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "TradeTransactionReport.h"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
MTReportInfo CTradeTransactionReport::s_info=
  {
   100,
   MTReportAPIVersion,
   0,
   L"Trade Transactions",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_NONE,
   MTReportInfo::TYPE_TABLE,
   L"Trades",
                    // params
     { { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM          },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_TO            },
      { MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS,  L"*" },
      { MTReportParam::TYPE_SYMBOLS,MTAPI_PARAM_SYMBOLS, L"*" },
     },4,           // params_total
     {              // configs
      { MTReportParam::TYPE_STRING, L"Base Directory"         ,DEFAULT_BASE_DIRECTORY },
      { MTReportParam::TYPE_BOOL,   L"Show group owner"       ,DEFAULT_SHOW_OWNER     },
      { MTReportParam::TYPE_BOOL,   L"Show daily transactions",DEFAULT_SHOW_DAILY     },
      { MTReportParam::TYPE_BOOL,   L"Show reason"            ,DEFAULT_SHOW_REASON    },
      { MTReportParam::TYPE_BOOL,   L"Show retcode"           ,DEFAULT_SHOW_RETCODE   },
      { MTReportParam::TYPE_BOOL,   L"Show rejected"          ,DEFAULT_SHOW_REJECTED  },
     },
   6            // configs_total
  };
//+------------------------------------------------------------------+
//| Column descriptions                                              |
//+------------------------------------------------------------------+
ReportColumn CTradeTransactionReport::s_columns[]=
   //--- id,                 name,               type,                                width,width_max, offset,                               size,                                    digits_column,flags
  {{ COLUMN_LOGIN           ,L"Login"           ,IMTDatasetColumn::TYPE_USER_LOGIN    ,4    ,0        ,offsetof(TableRecord,login           ),0                                       ,0            ,0                             },
   { COLUMN_LEVERAGE        ,L"Leverage"        ,IMTDatasetColumn::TYPE_USER_LEVERAGE ,3    ,0        ,offsetof(TableRecord,leverage        ),0                                       ,0            ,0                             },
   { COLUMN_DEALER          ,L"Dealer"          ,IMTDatasetColumn::TYPE_UINT64        ,4    ,0        ,offsetof(TableRecord,dealer          ),0                                       ,0            ,0                             },
   { COLUMN_IP              ,L"IP"              ,IMTDatasetColumn::TYPE_STRING        ,4    ,0        ,offsetof(TableRecord,ip              ),MtFieldSize(TableRecord,ip)             ,0            ,0                             },
   { COLUMN_CURRENCY        ,L"Currency"        ,IMTDatasetColumn::TYPE_STRING        ,3    ,0        ,offsetof(TableRecord,currency        ),MtFieldSize(TableRecord,currency)       ,0            ,0                             },
   { COLUMN_ORDER           ,L"Order"           ,IMTDatasetColumn::TYPE_UINT64        ,4    ,0        ,offsetof(TableRecord,order           ),0                                       ,0            ,0                             },
   { COLUMN_ORDER_ID        ,L"Order ID"        ,IMTDatasetColumn::TYPE_STRING        ,4    ,0        ,offsetof(TableRecord,order_id        ),MtFieldSize(TableRecord,order_id)       ,0            ,0                             },
   { COLUMN_DEAL            ,L"Deal"            ,IMTDatasetColumn::TYPE_UINT64        ,4    ,0        ,offsetof(TableRecord,deal            ),0                                       ,0            ,0                             },
   { COLUMN_DEAL_ID         ,L"Deal ID"         ,IMTDatasetColumn::TYPE_STRING        ,4    ,0        ,offsetof(TableRecord,deal_id         ),MtFieldSize(TableRecord,deal_id)        ,0            ,0                             },
   { COLUMN_POSITION        ,L"Position"        ,IMTDatasetColumn::TYPE_UINT64        ,4    ,0        ,offsetof(TableRecord,position        ),0                                       ,0            ,0                             },
   { COLUMN_POSITION_ID     ,L"Position ID"     ,IMTDatasetColumn::TYPE_STRING        ,4    ,0        ,offsetof(TableRecord,position_id     ),MtFieldSize(TableRecord,position_id)    ,0            ,0                             },
   { COLUMN_ACTION          ,L"Action"          ,IMTDatasetColumn::TYPE_STRING        ,8    ,0        ,offsetof(TableRecord,action          ),MtFieldSize(TableRecord,action)         ,0            ,0                             },
   { COLUMN_TYPE            ,L"Type"            ,IMTDatasetColumn::TYPE_STRING        ,4    ,0        ,offsetof(TableRecord,type            ),MtFieldSize(TableRecord,type)           ,0            ,0                             },
   { COLUMN_ENTRY           ,L"Entry"           ,IMTDatasetColumn::TYPE_STRING        ,4    ,0        ,offsetof(TableRecord,entry           ),MtFieldSize(TableRecord,entry)          ,0            ,0                             },
   { COLUMN_SYMBOL          ,L"Symbol"          ,IMTDatasetColumn::TYPE_STRING        ,4    ,0        ,offsetof(TableRecord,symbol          ),MtFieldSize(TableRecord,symbol)         ,0            ,0                             },
   { COLUMN_POSITION_BY     ,L"Position By"     ,IMTDatasetColumn::TYPE_UINT64        ,4    ,0        ,offsetof(TableRecord,position_by     ),0                                       ,0            ,0                             },
   { COLUMN_LOTS            ,L"Lots"            ,IMTDatasetColumn::TYPE_VOLUME_EXT    ,3    ,0        ,offsetof(TableRecord,lots            ),0                                       ,0            ,0                             },
   { COLUMN_AMOUNT          ,L"Amount"          ,IMTDatasetColumn::TYPE_MONEY         ,3    ,0        ,offsetof(TableRecord,amount          ),0                                       ,COLUMN_CURRENCY_DIGITS,0                             },
   { COLUMN_TIME            ,L"Time"            ,IMTDatasetColumn::TYPE_DATETIME      ,4    ,0        ,offsetof(TableRecord,timestamp       ),0                                       ,0            ,IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_PRICE           ,L"Price"           ,IMTDatasetColumn::TYPE_PRICE         ,3    ,0        ,offsetof(TableRecord,price           ),0                                       ,COLUMN_DIGITS,0                             },
   { COLUMN_BID             ,L"Bid"             ,IMTDatasetColumn::TYPE_STRING        ,3    ,0        ,offsetof(TableRecord,bid             ),MtFieldSize(TableRecord,bid)            ,0            ,IMTDatasetColumn::FLAG_RIGHT   },
   { COLUMN_ASK             ,L"Ask"             ,IMTDatasetColumn::TYPE_STRING        ,3    ,0        ,offsetof(TableRecord,ask             ),MtFieldSize(TableRecord,ask)            ,0            ,IMTDatasetColumn::FLAG_RIGHT   },
   { COLUMN_POSITION_PRICE  ,L"Position Price"  ,IMTDatasetColumn::TYPE_PRICE_POSITION,3    ,0        ,offsetof(TableRecord,price_position  ),0                                       ,COLUMN_DIGITS,0                             },
   { COLUMN_STOPLOSS        ,L"S / L"           ,IMTDatasetColumn::TYPE_DOUBLE        ,3    ,0        ,offsetof(TableRecord,sl              ),0                                       ,COLUMN_DIGITS,0                             },
   { COLUMN_TAKEPROFIT      ,L"T / P"           ,IMTDatasetColumn::TYPE_DOUBLE        ,3    ,0        ,offsetof(TableRecord,tp              ),0                                       ,COLUMN_DIGITS,0                             },
   { COLUMN_MARGIN_RATE     ,L"Margin Rate"     ,IMTDatasetColumn::TYPE_DOUBLE        ,3    ,0        ,offsetof(TableRecord,margin_rate     ),0                                       ,0            ,0                             },
   { COLUMN_MARGIN_AMOUNT   ,L"Margin Amount"   ,IMTDatasetColumn::TYPE_MONEY         ,3    ,0        ,offsetof(TableRecord,margin_amount   ),0                                       ,COLUMN_CURRENCY_DIGITS,0                             },
   { COLUMN_COMMISSION      ,L"Commission"      ,IMTDatasetColumn::TYPE_MONEY         ,3    ,0        ,offsetof(TableRecord,commission      ),0                                       ,COLUMN_CURRENCY_DIGITS,0                             },
   { COLUMN_SWAP            ,L"Swap"            ,IMTDatasetColumn::TYPE_MONEY         ,3    ,0        ,offsetof(TableRecord,swap            ),0                                       ,COLUMN_CURRENCY_DIGITS,0                             },
   { COLUMN_PROFIT          ,L"Profit"          ,IMTDatasetColumn::TYPE_MONEY         ,3    ,0        ,offsetof(TableRecord,profit          ),0                                       ,COLUMN_CURRENCY_DIGITS,0                             },
   { COLUMN_PROFIT_CURRENCY ,L"Profit Currency" ,IMTDatasetColumn::TYPE_STRING        ,3    ,0        ,offsetof(TableRecord,profit_currency ),MtFieldSize(TableRecord,profit_currency),0            ,0                             },
   { COLUMN_PROFIT_RATE     ,L"Profit Rate"     ,IMTDatasetColumn::TYPE_DOUBLE        ,3    ,0        ,offsetof(TableRecord,profit_rate     ),0                                       ,0            ,0                             },
   { COLUMN_PROFIT_RAW      ,L"Raw Profit"      ,IMTDatasetColumn::TYPE_DOUBLE        ,3    ,0        ,offsetof(TableRecord,profit_raw      ),0                                       ,COLUMN_CURRENCY_DIGITS,0                             },
   { COLUMN_AMOUNT_CLOSED   ,L"Amount Closed"   ,IMTDatasetColumn::TYPE_MONEY         ,3    ,0        ,offsetof(TableRecord,amount_closed   ),0                                       ,COLUMN_CURRENCY_DIGITS,0                             },
   { COLUMN_GATEWAY_PRICE   ,L"Gateway Price"   ,IMTDatasetColumn::TYPE_DOUBLE        ,3    ,0        ,offsetof(TableRecord,price_gateway   ),0                                       ,COLUMN_DIGITS,0                             },
   { COLUMN_REASON          ,L"Reason"          ,IMTDatasetColumn::TYPE_STRING        ,3    ,0        ,offsetof(TableRecord,reason          ),MtFieldSize(TableRecord,reason)         ,0            ,IMTDatasetColumn::FLAG_HIDDEN  },
   { COLUMN_RETCODE         ,L"Retcode"         ,IMTDatasetColumn::TYPE_STRING        ,3    ,0        ,offsetof(TableRecord,retcode         ),MtFieldSize(TableRecord,retcode)        ,0            ,IMTDatasetColumn::FLAG_HIDDEN  },
   { COLUMN_GROUP_OWNER     ,L"Group Owner"     ,IMTDatasetColumn::TYPE_STRING        ,3    ,0        ,offsetof(TableRecord,group_owner     ),MtFieldSize(TableRecord,group_owner)    ,0            ,IMTDatasetColumn::FLAG_HIDDEN  },
   { COLUMN_DIGITS          ,L"Digits"          ,IMTDatasetColumn::TYPE_UINT32        ,0    ,0        ,offsetof(TableRecord,digits)          ,0                                       ,0            ,IMTDatasetColumn::FLAG_HIDDEN  },
   { COLUMN_CURRENCY_DIGITS ,L"CurrencyDigits"  ,IMTDatasetColumn::TYPE_UINT32        ,0    ,0        ,offsetof(TableRecord,digits_currency) ,0                                       ,0            ,IMTDatasetColumn::FLAG_HIDDEN  }
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CTradeTransactionReport::CTradeTransactionReport(void) : m_api(NULL),m_show_owner(false),m_show_daily(true),m_show_reason(false),m_show_retcode(false),m_show_rejected(false)
  {
   m_base.Close();
   m_logins.Clear();
   m_symbol_mask.Clear();
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CTradeTransactionReport::~CTradeTransactionReport(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear members                                                    |
//+------------------------------------------------------------------+
void CTradeTransactionReport::Clear(void)
  {
   m_api          =NULL;
   m_show_owner   =false;
   m_show_daily   =false;
   m_show_reason  =false;
   m_show_retcode =false;          // show retcode
   m_show_rejected=false;          // show rejected transactions
//--- clear all
   m_base.Close();
   m_logins.Clear();
   m_symbol_mask.Clear();
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CTradeTransactionReport::Generate(const uint32_t type,IMTReportAPI *api)
  {
   MTAPIRES      res      =MT_RET_ERROR;
   IMTConReport *report   =NULL;
   IMTConParam  *parameter=NULL;
   CMTStrPath    path;
   int32_t           pos      =0;
   uint64_t       *logins   =NULL;
   uint32_t      total    =0;
//--- clear
   Clear();
//--- checks
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- checks
   if(type!=MTReportInfo::TYPE_TABLE)
      return(MT_RET_ERR_NOTIMPLEMENT);
//--- save api pointer
   m_api=api;
//--- get name of plugin
   if((report=m_api->ReportCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- create parameter interface
   if((parameter=m_api->ParamCreate())==NULL)
     {
      report->Release();
      return(MT_RET_ERR_MEM);
     }
//--- get report
   if((res=m_api->ReportCurrent(report))!=MT_RET_OK)
     {
      parameter->Release();
      report->Release();
      return(res);
     }
//--- get show owner parameter
   if(report->ParameterGet(L"Show group owner",parameter)==MT_RET_OK)
      m_show_owner=CMTStr::CompareNoCase(parameter->ValueString(),L"Yes")==0 || parameter->ValueBool();
   else
      m_show_owner=false;
//--- get show daily transactions parameter
   if(report->ParameterGet(L"Show daily transactions",parameter)==MT_RET_OK)
      m_show_daily=CMTStr::CompareNoCase(parameter->ValueString(),L"Yes")==0 || parameter->ValueBool();
   else
      m_show_daily=true;
//--- get show reason parameter
   if(report->ParameterGet(L"Show reason",parameter)==MT_RET_OK)
      m_show_reason=CMTStr::CompareNoCase(parameter->ValueString(),L"Yes")==0 || parameter->ValueBool();
   else
      m_show_reason=false;
//--- get show retcode parameter
   if(report->ParameterGet(L"Show retcode",parameter)==MT_RET_OK)
      m_show_retcode=CMTStr::CompareNoCase(parameter->ValueString(),L"Yes")==0 || parameter->ValueBool();
   else
      m_show_retcode=false;
//--- get show rejected parameter
   if(report->ParameterGet(L"Show rejected",parameter)==MT_RET_OK)
      m_show_rejected=CMTStr::CompareNoCase(parameter->ValueString(),L"Yes")==0 || parameter->ValueBool();
   else
      m_show_rejected=false;
//--- get base directory parameter
   if((res=report->ParameterGet(L"Base Directory",parameter))!=MT_RET_OK)
     {
      parameter->Release();
      report->Release();
      return(res);
     }
//--- get path
   GetModuleFileNameW(NULL,path.Buffer(),path.Max());
   path.Refresh();
   if((pos=path.FindRChar(L'\\'))>0) path.Trim(pos);
   path.Append(L"\\Reports\\Trades.Transaction.Reports\\");
//--- set plugin name
   if(parameter->ValueString())
      path.Append(parameter->ValueString());
   else
      path.Append(DEFAULT_BASE_DIRECTORY);
   path.Append(L"\\");
//--- release interfaces
   report->Release();    report   =NULL;
   parameter->Release(); parameter=NULL;
//--- get symbol mask
   m_symbol_mask.Assign(m_api->ParamSymbols());
   if(m_symbol_mask.Empty())
      return(MT_RET_ERR_PARAMS);
//--- get logins parameter
   if((res=m_api->ParamLogins(logins,total))!=MT_RET_OK)
      return(res);
   if(!total)
      return(MT_RET_ERR_PARAMS);
//--- add to array and sort it
   if(!m_logins.Add(logins,total))
      return(MT_RET_ERR_PARAMS);
   m_logins.Sort(SortLogins);
//--- initialize base reader
   if(!m_base.Initialize(path,nullptr))
      return(MT_RET_ERR_PARAMS);
//--- prepare table
   if((res=TablePrepare())!=MT_RET_OK)
      return(res);
//--- fill the table
   if((res=TableWrite())!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare table columns                                            |
//+------------------------------------------------------------------+
MTAPIRES CTradeTransactionReport::TablePrepare(void)
  {
   IMTDatasetColumn *column;
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create column
   if((column=m_api->TableColumnCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- clear table columns
   m_api->TableColumnClear();
//--- add column description for TableRecord
   for(uint32_t i=0;i<_countof(s_columns);i++)
     {
      MTAPIRES res;
      //--- clear
      column->Clear();
      //--- fill info
      column->ColumnID(s_columns[i].id);
      column->Name(s_columns[i].name);
      column->Type(s_columns[i].type);
      column->Width(s_columns[i].width);
      column->WidthMax(s_columns[i].width_max);
      column->Offset(s_columns[i].offset);
      column->Size(s_columns[i].size);
      if(COLUMN_MARGIN_RATE==s_columns[i].id || COLUMN_PROFIT_RATE==s_columns[i].id)
         column->Digits(RATE_DIGITS);
      else
         column->DigitsColumn(s_columns[i].digits_column);
      //--- reset flags
      column->Flags(s_columns[i].flags);
      //--- if "show owner group" is "Yes" - set column visible
      if(COLUMN_GROUP_OWNER==s_columns[i].id)
         column->Flags(m_show_owner ? 0 : s_columns[i].flags);
      //--- if "show retcode" is "Yes" - set column visible
      if(COLUMN_REASON==s_columns[i].id)
         column->Flags(m_show_reason ? 0 : s_columns[i].flags);
      //--- if "show retcode" is "Yes" - set column visible
      if(COLUMN_RETCODE==s_columns[i].id)
         column->Flags(m_show_retcode ? 0 : s_columns[i].flags);
      //--- add column
      if((res=m_api->TableColumnAdd(column))!=MT_RET_OK)
        {
         m_api->LoggerOut(MTLogErr,L"table column (%s) initialize filed [%u]",s_columns[i].name,res);
         column->Release();
         return(res);
        }
     }
//--- release column
   column->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write to the table                                               |
//+------------------------------------------------------------------+
MTAPIRES CTradeTransactionReport::TableWrite(void)
  {
   MTAPIRES          res      =MT_RET_ERROR;
   TransactionRecord db_record={0};
   TableRecord       tb_record={0};
   IMTConSymbol     *symbol   =NULL;
//--- checks
   if(!m_api || !m_logins.Total())
      return(MT_RET_ERR_PARAMS);
//--- create symbol
   if(!(symbol=m_api->SymbolCreate()))
      return(MT_RET_ERR_MEM);
//--- day by day
   for(int64_t day=m_api->ParamFrom();day<=m_api->ParamTo() && m_api->IsStopped()==MT_RET_OK;day+=SECONDS_IN_DAY)
     {
      //--- open the day
      if(!m_base.OpenRead(day))
         continue;
      //--- record by record
      while(m_base.Next(db_record) && m_api->IsStopped()==MT_RET_OK)
        {
         //--- is it daily record?
         if((db_record.action==TransactionRecord::ACTION_DAILY_POSITION ||
             db_record.action==TransactionRecord::ACTION_DAILY_ORDER) && !m_show_daily)
            continue;
         //--- check rejected
         if(!m_show_rejected)
            if(db_record.retcode!=MT_RET_OK                   &&
               db_record.retcode!=MT_RET_REQUEST_ACCEPTED     &&
               db_record.retcode!=MT_RET_REQUEST_DONE         &&
               db_record.retcode!=MT_RET_REQUEST_DONE_PARTIAL &&
               db_record.retcode!=MT_RET_REQUEST_PLACED)
               continue;
         //--- login filter
         if(m_logins.Search(&db_record.login,SortLogins)==NULL)
            continue;
         //--- symbol filter (deposit transactions haven't symbol)
         if(db_record.action!=TransactionRecord::ACTION_DEPOSIT)
           {
            //--- get symbol
            if((res=m_api->SymbolGetLight(db_record.symbol,symbol))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogAtt,L"get symbol %s failed [%u]",db_record.symbol,res);
               continue;
              }
            //--- check mask
            if(!CMTStr::CheckGroupMask(m_symbol_mask.Str(),symbol->Symbol()) &&
               !CMTStr::CheckGroupMask(m_symbol_mask.Str(),symbol->Path()))
               continue;
            //---
            if(!db_record.digits)
               db_record.digits=symbol->Digits();
           }
         else
           {
            //--- zero amount
            db_record.amount=0;
           }
         if(!db_record.digits_currency)
            db_record.digits_currency=2;
         //--- get table record
         tb_record.Set(db_record);
         //--- push record to the table
         if((res=m_api->TableRowWrite(&tb_record,sizeof(tb_record)))!=MT_RET_OK)
           {
            if(symbol)
               symbol->Release();
            return(res);
           }
        }
     }
//--- clear symbol
   if(symbol)
      symbol->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
LPCWSTR CTradeTransactionReport::PrintAction(const uint32_t action)
  {
//---
   switch(action)
     {
      case TransactionRecord::ACTION_MARKET                   : return(L"market");
      case TransactionRecord::ACTION_MARKET_OUT               : return(L"market");
      case TransactionRecord::ACTION_MARKET_INOUT             : return(L"market");
      case TransactionRecord::ACTION_POSITION_MODIFY          : return(L"position modify");
      case TransactionRecord::ACTION_PENDING                  : return(L"pending");
      case TransactionRecord::ACTION_PENDING_MODIFY           : return(L"pending modify");
      case TransactionRecord::ACTION_PENDING_CANCEL           : return(L"pending cancel");
      case TransactionRecord::ACTION_PENDING_EXPIRATION       : return(L"pending expiration");
      case TransactionRecord::ACTION_PENDING_ACTIVATION       : return(L"pending activation");
      case TransactionRecord::ACTION_SL                       : return(L"stop loss");
      case TransactionRecord::ACTION_TP                       : return(L"take profit");
      case TransactionRecord::ACTION_ROLLOVER                 : return(L"rollover");
      case TransactionRecord::ACTION_DEPOSIT                  : return(L"deposit");
      case TransactionRecord::ACTION_STOPOUT_ORDER            : return(L"order stopout");
      case TransactionRecord::ACTION_STOPOUT_POSITION         : return(L"position stopout");
      case TransactionRecord::ACTION_STOPLIMIT                : return(L"stop limit");
      case TransactionRecord::ACTION_DAILY_POSITION           : return(L"daily position");
      case TransactionRecord::ACTION_DAILY_ORDER              : return(L"daily order");
      case TransactionRecord::ACTION_DEALER_MARKET            : return(L"market by dealer");
      case TransactionRecord::ACTION_DEALER_POS_MODIFY        : return(L"position modify by dealer");
      case TransactionRecord::ACTION_DEALER_PENDING           : return(L"pending by dealer");
      case TransactionRecord::ACTION_DEALER_PENDING_MODIFY    : return(L"pending modify by dealer");
      case TransactionRecord::ACTION_DEALER_PENDING_CANCEL    : return(L"pending cancel by dealer");
      case TransactionRecord::ACTION_DEALER_PENDING_ACTIVATION: return(L"pending activation by dealer");
      case TransactionRecord::ACTION_CLOSE_BY                 : return(L"close by");
      case TransactionRecord::ACTION_DEALER_CLOSE_BY          : return(L"dealer close by");
      case TransactionRecord::ACTION_VMARGIN                  : return(L"variation margin");
     }
//---
   return(L"");
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
LPCWSTR CTradeTransactionReport::PrintType(const uint32_t type)
  {
//---
   switch(type)
     {
      case TransactionRecord::TYPE_BUY                   : return(L"buy");
      case TransactionRecord::TYPE_SELL                  : return(L"sell");
      case TransactionRecord::TYPE_BUY_LIMIT             : return(L"buy limit");
      case TransactionRecord::TYPE_SELL_LIMIT            : return(L"sell limit");
      case TransactionRecord::TYPE_BUY_STOP              : return(L"buy stop");
      case TransactionRecord::TYPE_SELL_STOP             : return(L"sell stop");
      case TransactionRecord::TYPE_BUY_STOP_LIMIT        : return(L"buy stop limit");
      case TransactionRecord::TYPE_SELL_STOP_LIMIT       : return(L"sell stop limit");
      case TransactionRecord::TYPE_DEPOSIT               : return(L"deposit");
      case TransactionRecord::TYPE_CREDIT                : return(L"credit");
      case TransactionRecord::TYPE_CHARGE                : return(L"charge");
      case TransactionRecord::TYPE_CORRECTION            : return(L"correction");
      case TransactionRecord::TYPE_BONUS                 : return(L"bonus");
      case TransactionRecord::TYPE_COMMISSION            : return(L"commission");
      case TransactionRecord::TYPE_COMMISSION_DAILY      : return(L"commission daily");
      case TransactionRecord::TYPE_COMMISSION_MONTHLY    : return(L"commission monthly");
      case TransactionRecord::TYPE_AGENT_DAILY           : return(L"agent daily");
      case TransactionRecord::TYPE_AGENT_MONTHLY         : return(L"agent monthly");
      case TransactionRecord::TYPE_INTERESTRATE          : return(L"interest rate");
      case TransactionRecord::TYPE_BUY_CANCELED          : return(L"buy canceled");
      case TransactionRecord::TYPE_SELL_CANCELED         : return(L"sell canceled");
      case TransactionRecord::TYPE_DIVIDEND              : return(L"dividend");
      case TransactionRecord::TYPE_DIVIDEND_FRANKED      : return(L"dividend franked");
      case TransactionRecord::TYPE_TAX                   : return(L"tax");
      case TransactionRecord::TYPE_AGENT                 : return(L"agent");
      case TransactionRecord::TYPE_CLOSE_BY              : return(L"close by");
      case TransactionRecord::TYPE_SO_COMPENSATION       : return(L"stop-out compensation");
      case TransactionRecord::TYPE_SO_COMPENSATION_CREDIT: return(L"stop-out compensation credit");
     }
//---
   return(L"");
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
LPCWSTR CTradeTransactionReport::PrintEntry(const uint32_t entry)
  {
//---
   switch(entry)
     {
      case TransactionRecord::ENTRY_IN     : return(L"in");
      case TransactionRecord::ENTRY_OUT    : return(L"out");
      case TransactionRecord::ENTRY_INOUT  : return(L"in-out");
      case TransactionRecord::ENTRY_OUT_BY : return(L"out-by");

      default:
         return(L"");
     }
//---
   return(L"");
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
LPCWSTR CTradeTransactionReport::PrintReason(const uint32_t reason)
  {
//---
   switch(reason)
     {
      case IMTOrder::ORDER_REASON_CLIENT            : return(L"Client");
      case IMTOrder::ORDER_REASON_EXPERT            : return(L"Expert");
      case IMTOrder::ORDER_REASON_DEALER            : return(L"Dealer");
      case IMTOrder::ORDER_REASON_SL                : return(L"Stop loss");
      case IMTOrder::ORDER_REASON_TP                : return(L"Take profit");
      case IMTOrder::ORDER_REASON_SO                : return(L"Stop out");
      case IMTOrder::ORDER_REASON_ROLLOVER          : return(L"Rollover");
      case IMTOrder::ORDER_REASON_EXTERNAL_CLIENT   : return(L"External");
      case IMTOrder::ORDER_REASON_VMARGIN           : return(L"Variation margin");
      case IMTOrder::ORDER_REASON_GATEWAY           : return(L"Gateway");
      case IMTOrder::ORDER_REASON_SIGNAL            : return(L"Signal");
      case IMTOrder::ORDER_REASON_SETTLEMENT        : return(L"Settlement");
      case IMTOrder::ORDER_REASON_TRANSFER          : return(L"Transfer");
      case IMTOrder::ORDER_REASON_SYNC              : return(L"Synchronization");
      case IMTOrder::ORDER_REASON_EXTERNAL_SERVICE  : return(L"External Service");
      case IMTOrder::ORDER_REASON_MIGRATION         : return(L"Migration");
      case IMTOrder::ORDER_REASON_MOBILE            : return(L"Mobile");
      case IMTOrder::ORDER_REASON_WEB               : return(L"Web");
     }
//---
   return(L"");
  }
//+------------------------------------------------------------------+
//| Sort logins                                                      |
//+------------------------------------------------------------------+
int32_t CTradeTransactionReport::SortLogins(const void *left,const void *right)
  {
   uint64_t lft=*(uint64_t*)left;
   uint64_t rgh=*(uint64_t*)right;
//--- by logins
   if(lft>rgh) return(-1);
   if(lft<rgh) return(1);
//--- equal
   return(0);
  }
//+------------------------------------------------------------------+