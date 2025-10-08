//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "OrderbookReport.h"
#include "..\Tools\TextReader.h"
//+------------------------------------------------------------------+
//| Report description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo COrderbookReport::s_info=
  {
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_ANY,
   L"Orderbook",
   COPYRIGHT,
   L"MetaTrader 5 Report API plug-in",
   0,
   MTReportInfo::TYPE_TABLE,
   L"NFA",
     {{ MTReportParam::TYPE_DATE   ,MTAPI_PARAM_FROM         },
      { MTReportParam::TYPE_DATE   ,MTAPI_PARAM_TO           },
      { MTReportParam::TYPE_GROUPS ,MTAPI_PARAM_GROUPS ,L"*" },
      { MTReportParam::TYPE_SYMBOLS,MTAPI_PARAM_SYMBOLS,L"*" },
     },
   4,   // params_total 
     {{ MTReportParam::TYPE_STRING,L"Base Directory",DEFAULT_BASE_DIRECTORY }
     },
   1    //config_total
  };
//+------------------------------------------------------------------+
//| Report fields description structures                             |
//+------------------------------------------------------------------+
ReportColumn COrderbookReport::s_columns[]=
   //id                         ,name                    ,type                             ,width,width_max,offset                                    ,size                                     ,digits_column,flags
  {{ COLUMN_ID                  ,L"ID"                   ,IMTDatasetColumn::TYPE_INT64     ,    1,        0,offsetof(TableRecord,id                  ),0                                         ,0            ,0  },
   { COLUMN_BATCH_DATE          ,L"BATCH_DATE"           ,IMTDatasetColumn::TYPE_DATE      ,    1,        0,offsetof(TableRecord,batch_date          ),0                                         ,0            ,0  },
   { COLUMN_TRANSACTION_DATETIME,L"TRANSACTION_DATETIME" ,IMTDatasetColumn::TYPE_DATETIME  ,    1,        0,offsetof(TableRecord,transaction_datetime),0                                         ,0            ,0  },
   { COLUMN_ORDER_ID            ,L"ORDER_ID"             ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,order_id            ),MtFieldSize(TableRecord,order_id         ),0            ,0  },
   { COLUMN_FDM_ID              ,L"FDM_ID"               ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,fdm_id              ),MtFieldSize(TableRecord,fdm_id           ),0            ,0  },
   { COLUMN_MKT_SEG_ID          ,L"MKT_SEG_ID"           ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,mkt_seg_id          ),MtFieldSize(TableRecord,mkt_seg_id       ),0            ,0  },
   { COLUMN_PRODUCT_CODE        ,L"PRODUCT_CODE"         ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,product_code        ),MtFieldSize(TableRecord,product_code     ),0            ,0  },
   { COLUMN_PRODUCT_CAT         ,L"PRODUCT_CAT"          ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,product_cat         ),MtFieldSize(TableRecord,product_cat      ),0            ,0  },
   { COLUMN_CONTRACT_YEAR       ,L"CONTRACT_YEAR"        ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,contract_year       ),MtFieldSize(TableRecord,contract_year    ),0            ,0  },
   { COLUMN_CONTRACT_MONTH      ,L"CONTRACT_MONTH"       ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,contract_month      ),MtFieldSize(TableRecord,contract_month   ),0            ,0  },
   { COLUMN_CONTRACT_DAY        ,L"CONTRACT_DAY"         ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,contract_day        ),MtFieldSize(TableRecord,contract_day     ),0            ,0  },
   { COLUMN_STRIKE              ,L"STRIKE"               ,IMTDatasetColumn::TYPE_PRICE     ,    1,        0,offsetof(TableRecord,strike              ),0                                         ,6            ,0  },
   { COLUMN_OPTION_TYPE         ,L"OPTION_TYPE"          ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,option_type         ),MtFieldSize(TableRecord,option_type      ),0            ,0  },
   { COLUMN_TRANSACTION_TYPE    ,L"TRANSACTION_TYPE"     ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,transaction_type    ),MtFieldSize(TableRecord,transaction_type ),0            ,0  },
   { COLUMN_ORDER_TYPE          ,L"ORDER_TYPE"           ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,order_type          ),MtFieldSize(TableRecord,order_type       ),0            ,0  },
   { COLUMN_VERB                ,L"VERB"                 ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,verb                ),MtFieldSize(TableRecord,verb             ),0            ,0  },
   { COLUMN_BID_PRICE           ,L"BID_PRICE"            ,IMTDatasetColumn::TYPE_PRICE     ,    1,        0,offsetof(TableRecord,bid_price           ),0                                         ,6            ,0  },
   { COLUMN_ASK_PRICE           ,L"ASK_PRICE"            ,IMTDatasetColumn::TYPE_PRICE     ,    1,        0,offsetof(TableRecord,ask_price           ),0                                         ,6            ,0  },
   { COLUMN_QUANTITY            ,L"QUANTITY"             ,IMTDatasetColumn::TYPE_UINT64    ,    1,        0,offsetof(TableRecord,quantity            ),0                                         ,0            ,0  },
   { COLUMN_REMAINING_QTY       ,L"REMAINING_QTY"        ,IMTDatasetColumn::TYPE_UINT64    ,    1,        0,offsetof(TableRecord,remaining_qty       ),0                                         ,0            ,0  },
   { COLUMN_PRICE               ,L"PRICE"                ,IMTDatasetColumn::TYPE_PRICE     ,    1,        0,offsetof(TableRecord,price               ),0                                         ,6            ,0  },
   { COLUMN_STOP_PRICE          ,L"STOP_PRICE"           ,IMTDatasetColumn::TYPE_PRICE     ,    1,        0,offsetof(TableRecord,stop_price          ),0                                         ,6            ,0  },
   { COLUMN_STOP_PRODUCT_CODE   ,L"STOP_PRODUCT_CODE"    ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,stop_product_code   ),MtFieldSize(TableRecord,stop_product_code),0            ,0  },
   { COLUMN_TRAIL_AMT           ,L"TRAIL_AMT"            ,IMTDatasetColumn::TYPE_DOUBLE    ,    1,        0,offsetof(TableRecord,trail_amt           ),0                                         ,6            ,0  },
   { COLUMN_LIMIT_OFFSET        ,L"LIMIT_OFFSET"         ,IMTDatasetColumn::TYPE_DOUBLE    ,    1,        0,offsetof(TableRecord,limit_offset        ),0                                         ,6            ,0  },
   { COLUMN_DURATION            ,L"DURATION"             ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,duration            ),MtFieldSize(TableRecord,duration         ),0            ,0  },
   { COLUMN_EXPIRY_DATE         ,L"EXPIRY_DATE"          ,IMTDatasetColumn::TYPE_DATETIME  ,    1,        0,offsetof(TableRecord,expiry_date         ),0                                         ,0            ,0  },
   { COLUMN_ORDER_ORIGIN        ,L"ORDER_ORIGIN"         ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,order_origin        ),MtFieldSize(TableRecord,order_origin     ),0            ,0  },
   { COLUMN_MANAGER_ID          ,L"MANAGER_ID"           ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,manager_id          ),MtFieldSize(TableRecord,manager_id       ),0            ,0  },
   { COLUMN_CUSTACCT_ID         ,L"CUSTACCT_ID"          ,IMTDatasetColumn::TYPE_USER_LOGIN,    1,        0,offsetof(TableRecord,custacct_id         ),0                                         ,0            ,0  },
   { COLUMN_SERVER_ID           ,L"SERVER_ID"            ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,server_id           ),MtFieldSize(TableRecord,server_id        ),0            ,0  },
   { COLUMN_CUST_GROUP          ,L"CUST_GROUP"           ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,cust_group          ),MtFieldSize(TableRecord,cust_group       ),0            ,0  },
   { COLUMN_LINKED_ORDER_ID     ,L"LINKED_ORDER_ID"      ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,linked_order_id     ),MtFieldSize(TableRecord,linked_order_id  ),0            ,0  },
   { COLUMN_LINK_REASON         ,L"LINK_REASON"          ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,link_reason         ),MtFieldSize(TableRecord,link_reason      ),0            ,0  },
   { COLUMN_OPEN_CLOSE          ,L"OPEN_CLOSE"           ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,open_close          ),MtFieldSize(TableRecord,open_close       ),0            ,0  }
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
COrderbookReport::COrderbookReport(void)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
COrderbookReport::~COrderbookReport(void)
  {
  }
//+------------------------------------------------------------------+
//| Get report descriptor                                            |
//+------------------------------------------------------------------+
MTAPIRES COrderbookReport::Info(MTReportInfo &info)
  {
   info=s_info;
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Remove report class                                              |
//+------------------------------------------------------------------+
void COrderbookReport::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Generate report                                                  |
//+------------------------------------------------------------------+
MTAPIRES COrderbookReport::Generate(const uint32_t type,IMTReportAPI *api)
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
   return(ReportContext::Generate(api,REPORT_ORDERBOOK,Fill));
  }
//+------------------------------------------------------------------+
//|  Fill Table record from CSV line                                 |
//+------------------------------------------------------------------+
bool COrderbookReport::Fill(CTextReader &reader,TableRecord &record,const ReportContext&)
  {
//--- fill table record
   reader.Integer (record.id                  );
   reader.Date    (record.batch_date          );
   reader.DateTime(record.transaction_datetime);
   reader.String  (record.order_id            );
   reader.String  (record.fdm_id              );
   reader.String  (record.mkt_seg_id          );
   reader.String  (record.product_code        );
   reader.String  (record.product_cat         );
   reader.String  (record.contract_year       );
   reader.String  (record.contract_month      );
   reader.String  (record.contract_day        );
   reader.Double  (record.strike              );
   reader.String  (record.option_type         );
   reader.String  (record.transaction_type    );
   reader.String  (record.order_type          );
   reader.String  (record.verb                );
   reader.Double  (record.bid_price           );
   reader.Double  (record.ask_price           );
   reader.Integer (record.quantity            );
   reader.Integer (record.remaining_qty       );
   reader.Double  (record.price               );
   reader.Double  (record.stop_price          );
   reader.String  (record.stop_product_code   );
   reader.Double  (record.trail_amt           );
   reader.Double  (record.limit_offset        );
   reader.String  (record.duration            );
   reader.DateTime(record.expiry_date         );
   reader.String  (record.order_origin        );
   reader.String  (record.manager_id          );
   reader.Integer (record.custacct_id         );
   reader.String  (record.server_id           );
   reader.String  (record.cust_group          );
   reader.String  (record.linked_order_id     );
   reader.String  (record.link_reason         );
   reader.String  (record.open_close          );
//---
   return(true);
  }
//+------------------------------------------------------------------+
