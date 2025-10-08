//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "TradeReport.h"
#include "..\Tools\TextReader.h"
//+------------------------------------------------------------------+
//| Report description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CTradeReport::s_info=
  {
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_ANY,
   L"Trade",
   COPYRIGHT,
   L"MetaTrader 5 Report API plug-in",
   0,
   MTReportInfo::TYPE_TABLE,
   L"NFA",
     {{ MTReportParam::TYPE_DATE,MTAPI_PARAM_FROM },
      { MTReportParam::TYPE_DATE,MTAPI_PARAM_TO   },
     },
   2,   // params_total 
     {{ MTReportParam::TYPE_STRING,L"Base Directory",DEFAULT_BASE_DIRECTORY }
     },
   1    //config_total
  };
//+------------------------------------------------------------------+
//| Report fields description structures                             |
//+------------------------------------------------------------------+
ReportColumn CTradeReport::s_columns[]=
   //id                         ,name                    ,type                             ,width,width_max,offset                                    ,size                                     ,digits_column,flags
  {{ COLUMN_ID                  ,L"ID"                   ,IMTDatasetColumn::TYPE_INT64     ,    1,        0,offsetof(TableRecord,id                  ),0                                         ,0            ,0  },
   { COLUMN_BATCH_DATE          ,L"BATCH_DATE"           ,IMTDatasetColumn::TYPE_DATE      ,    1,        0,offsetof(TableRecord,batch_date          ),0                                         ,0            ,0  },
   { COLUMN_TRADE_ID            ,L"TRADE_ID"             ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,trade_id            ),MtFieldSize(TableRecord,trade_id         ),0            ,0  },
   { COLUMN_ORDER_ID            ,L"ORDER_ID"             ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,order_id            ),MtFieldSize(TableRecord,order_id         ),0            ,0  },
   { COLUMN_FDM_ID              ,L"FDM_ID"               ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,fdm_id              ),MtFieldSize(TableRecord,fdm_id           ),0            ,0  },
   { COLUMN_MKT_SEG_ID          ,L"MKT_SEG_ID"           ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,mkt_seg_id          ),MtFieldSize(TableRecord,mkt_seg_id       ),0            ,0  },
   { COLUMN_TIME_TRADEMATCH     ,L"TIME_TRADEMATCH"      ,IMTDatasetColumn::TYPE_DATETIME  ,    1,        0,offsetof(TableRecord,time_tradematch     ),0                                         ,0            ,0  },
   { COLUMN_PRODUCT_CAT         ,L"PRODUCT_CAT"          ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,product_cat         ),MtFieldSize(TableRecord,product_cat      ),0            ,0  },
   { COLUMN_PRODUCT_CODE        ,L"PRODUCT_CODE"         ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,product_code        ),MtFieldSize(TableRecord,product_code     ),0            ,0  },
   { COLUMN_CONTRACT_YEAR       ,L"CONTRACT_YEAR"        ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,contract_year       ),MtFieldSize(TableRecord,contract_year    ),0            ,0  },
   { COLUMN_CONTRACT_MONTH      ,L"CONTRACT_MONTH"       ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,contract_month      ),MtFieldSize(TableRecord,contract_month   ),0            ,0  },
   { COLUMN_CONTRACT_DAY        ,L"CONTRACT_DAY"         ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,contract_day        ),MtFieldSize(TableRecord,contract_day     ),0            ,0  },
   { COLUMN_STRIKE              ,L"STRIKE"               ,IMTDatasetColumn::TYPE_PRICE     ,    1,        0,offsetof(TableRecord,strike              ),0                                         ,6            ,0  },
   { COLUMN_VERB                ,L"VERB"                 ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,verb                ),MtFieldSize(TableRecord,verb             ),0            ,0  },
   { COLUMN_BID_PRICE           ,L"BID_PRICE"            ,IMTDatasetColumn::TYPE_PRICE     ,    1,        0,offsetof(TableRecord,bid_price           ),0                                         ,6            ,0  },
   { COLUMN_ASK_PRICE           ,L"ASK_PRICE"            ,IMTDatasetColumn::TYPE_PRICE     ,    1,        0,offsetof(TableRecord,ask_price           ),0                                         ,6            ,0  },
   { COLUMN_QUANTITY            ,L"QUANTITY"             ,IMTDatasetColumn::TYPE_UINT64    ,    1,        0,offsetof(TableRecord,quantity            ),0                                         ,0            ,0  },
   { COLUMN_REMAINING_QTY       ,L"REMAINING_QTY"        ,IMTDatasetColumn::TYPE_UINT64    ,    1,        0,offsetof(TableRecord,remaining_qty       ),0                                         ,0            ,0  },
   { COLUMN_FILL_PRICE          ,L"FILL_PRICE"           ,IMTDatasetColumn::TYPE_PRICE     ,    1,        0,offsetof(TableRecord,fill_price          ),0                                         ,6            ,0  },
   { COLUMN_CONTRA_FILL_PRICE   ,L"CONTRA_FILL_PRICE"    ,IMTDatasetColumn::TYPE_PRICE     ,    1,        0,offsetof(TableRecord,contra_fill_price   ),0                                         ,6            ,0  },
   { COLUMN_SERVER_ID           ,L"SERVER_ID"            ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,server_id           ),MtFieldSize(TableRecord,server_id        ),0            ,0  },
   { COLUMN_IMPLIED_VOLATILITY  ,L"IMPLIED_VOLATILITY"   ,IMTDatasetColumn::TYPE_DOUBLE    ,    1,        0,offsetof(TableRecord,implied_volatility  ),0                                         ,6            ,0  },
   { COLUMN_IB_REBATE           ,L"IB_REBATE"            ,IMTDatasetColumn::TYPE_DOUBLE    ,    1,        0,offsetof(TableRecord,ib_rebate           ),0                                         ,6            ,0  },
   { COLUMN_COMMISSION          ,L"COMMISSION"            ,IMTDatasetColumn::TYPE_MONEY    ,    1,        0,offsetof(TableRecord,commission          ),0                                         ,6            ,0  },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CTradeReport::CTradeReport(void)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CTradeReport::~CTradeReport(void)
  {
  }
//+------------------------------------------------------------------+
//| Get report descriptor                                            |
//+------------------------------------------------------------------+
MTAPIRES CTradeReport::Info(MTReportInfo &info)
  {
   info=s_info;
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Remove report class                                              |
//+------------------------------------------------------------------+
void CTradeReport::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Generate report                                                  |
//+------------------------------------------------------------------+
MTAPIRES CTradeReport::Generate(const uint32_t type,IMTReportAPI *api)
  {
//---
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- There is only table report implemented
   if(type!=MTReportInfo::TYPE_TABLE)
      return(MT_RET_ERR_NOTIMPLEMENT);
//--- prepare columns
   MTAPIRES res;
   if((res=ReportColumn::ReportColumnsAdd(api,s_columns,_countof(s_columns)))!=MT_RET_OK)
      return(res);
//--- generate
   return(ReportContext::Generate(api,REPORT_TRADE,Fill));
  }
//+------------------------------------------------------------------+
//|  Fill Table record from CSV line                                 |
//+------------------------------------------------------------------+
bool CTradeReport::Fill(CTextReader &reader,TableRecord &record,const ReportContext&)
  {
//--- fill table record
   reader.Integer (record.id                );
   reader.Date    (record.batch_date        );
   reader.String  (record.trade_id          );
   reader.String  (record.order_id          );
   reader.String  (record.fdm_id            );
   reader.String  (record.mkt_seg_id        );
   reader.DateTime(record.time_tradematch   );
   reader.String  (record.product_cat       );
   reader.String  (record.product_code      );
   reader.String  (record.contract_year     );
   reader.String  (record.contract_month    );
   reader.String  (record.contract_day      );
   reader.Double  (record.strike            );
   reader.String  (record.verb              );
   reader.Double  (record.bid_price         );
   reader.Double  (record.ask_price         );
   reader.Integer (record.quantity          );
   reader.Integer (record.remaining_qty     );
   reader.Double  (record.fill_price        );
   reader.Double  (record.contra_fill_price );
   reader.String  (record.server_id         );
   reader.Double  (record.implied_volatility);
   reader.Double  (record.ib_rebate         );
   reader.Double  (record.commission        );
//---
   return(true);
  }
//+------------------------------------------------------------------+
