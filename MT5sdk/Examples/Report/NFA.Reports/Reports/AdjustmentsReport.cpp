//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "AdjustmentsReport.h"
//+------------------------------------------------------------------+
//| Report description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CAdjustmentsReport::s_info=
  {
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_ANY,
   L"Adjustments",
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
ReportColumn CAdjustmentsReport::s_columns[]=
   //id                         ,name                    ,type                             ,width,width_max,offset                                    ,size                                     ,digits_column,flags
  {{ COLUMN_BATCH_DATE          ,L"BATCH_DATE"           ,IMTDatasetColumn::TYPE_DATE      ,    1,        0,offsetof(TableRecord,batch_date          ),0                                         ,0            ,0  },
   { COLUMN_FDM_ID              ,L"FDM_ID"               ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,fdm_id              ),MtFieldSize(TableRecord,fdm_id           ),0            ,0  },
   { COLUMN_TRADE_ID            ,L"TRADE_ID"             ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,trade_id            ),MtFieldSize(TableRecord,trade_id         ),0            ,0  },
   { COLUMN_TRADE_DATE          ,L"TRADE_DATE"           ,IMTDatasetColumn::TYPE_DATE      ,    1,        0,offsetof(TableRecord,trade_date          ),0                                         ,0            ,0  },
   { COLUMN_ADJUSTMENT_DATETIME ,L"ADJUSTMENT_DATETIME"  ,IMTDatasetColumn::TYPE_DATETIME  ,    1,        0,offsetof(TableRecord,adjustment_datetime ),0                                         ,0            ,0  },
   { COLUMN_ADJUSTMENT_TYPE     ,L"ADJUSTMENT_TYPE"      ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,adjustment_type     ),MtFieldSize(TableRecord,adjustment_type  ),0            ,0  },
   { COLUMN_PRICE               ,L"PRICE"                ,IMTDatasetColumn::TYPE_PRICE     ,    1,        0,offsetof(TableRecord,price               ),0                                         ,6            ,0  },
   { COLUMN_QUANTITY            ,L"QUANTITY"             ,IMTDatasetColumn::TYPE_UINT64    ,    1,        0,offsetof(TableRecord,quantity            ),0                                         ,0            ,0  },
   { COLUMN_CASH_AMOUNT         ,L"CASH_AMOUNT"          ,IMTDatasetColumn::TYPE_MONEY     ,    1,        0,offsetof(TableRecord,cash_amount         ),0                                         ,6            ,0  },
   { COLUMN_NOTE                ,L"NOTE"                 ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,note                ),MtFieldSize(TableRecord,note             ),0            ,0  },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CAdjustmentsReport::CAdjustmentsReport(void)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CAdjustmentsReport::~CAdjustmentsReport(void)
  {
  }
//+------------------------------------------------------------------+
//| Get report descriptor                                            |
//+------------------------------------------------------------------+
MTAPIRES CAdjustmentsReport::Info(MTReportInfo &info)
  {
   info=s_info;
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Remove report class                                              |
//+------------------------------------------------------------------+
void CAdjustmentsReport::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Generate report                                                  |
//+------------------------------------------------------------------+
MTAPIRES CAdjustmentsReport::Generate(const uint32_t type,IMTReportAPI *api)
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
   return(ReportContext::Generate(api,REPORT_ADJUSTMENTS,Fill));
  }
//+------------------------------------------------------------------+
//|  Fill Table record from CSV line                                 |
//+------------------------------------------------------------------+
bool CAdjustmentsReport::Fill(CTextReader &reader,TableRecord &record,const ReportContext&)
  {
//--- fill table record
   reader.Date    (record.batch_date);
   reader.String  (record.fdm_id);
   reader.String  (record.trade_id);
   reader.Date    (record.trade_date);
   reader.DateTime(record.adjustment_datetime);
   reader.String  (record.adjustment_type);
   reader.Double  (record.price);
   reader.Integer (record.quantity);
   reader.Double  (record.cash_amount);
   reader.String  (record.note);
//---
   return(true);
  }
//+------------------------------------------------------------------+
