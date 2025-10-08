//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "TickReport.h"
#include "..\Tools\TextReader.h"
//+------------------------------------------------------------------+
//| Report description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CTickReport::s_info=
  {
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_ANY,
   L"Tick",
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
ReportColumn CTickReport::s_columns[]=
   //id                         ,name                    ,type                             ,width,width_max,offset                                    ,size                                     ,digits_column,flags
  {{ COLUMN_FDM_ID              ,L"FDM_ID"               ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,fdm_id              ),MtFieldSize(TableRecord,fdm_id           ),0            ,0  },
   { COLUMN_BATCH_DATE          ,L"BATCH_DATE"           ,IMTDatasetColumn::TYPE_DATE      ,    1,        0,offsetof(TableRecord,batch_date          ),0                                         ,0            ,0  },
   { COLUMN_PRODUCT_CODE        ,L"PRODUCT_CODE"         ,IMTDatasetColumn::TYPE_STRING    ,    1,        0,offsetof(TableRecord,product_code        ),MtFieldSize(TableRecord,product_code     ),0            ,0  },
   { COLUMN_QUOTE_DATETIME      ,L"QUOTE_DATETIME"       ,IMTDatasetColumn::TYPE_DATETIME  ,    1,        0,offsetof(TableRecord,quote_datetime      ),0                                         ,0            ,0  },
   { COLUMN_BID_PRICE           ,L"BID_PRICE"            ,IMTDatasetColumn::TYPE_PRICE     ,    1,        0,offsetof(TableRecord,bid_price           ),0                                         ,6            ,0  },
   { COLUMN_ASK_PRICE           ,L"ASK_PRICE"            ,IMTDatasetColumn::TYPE_PRICE     ,    1,        0,offsetof(TableRecord,ask_price           ),0                                         ,6            ,0  },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CTickReport::CTickReport(void)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CTickReport::~CTickReport(void)
  {
  }
//+------------------------------------------------------------------+
//| Get report descriptor                                            |
//+------------------------------------------------------------------+
MTAPIRES CTickReport::Info(MTReportInfo &info)
  {
   info=s_info;
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Remove report class                                              |
//+------------------------------------------------------------------+
void CTickReport::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Generate report                                                  |
//+------------------------------------------------------------------+
MTAPIRES CTickReport::Generate(const uint32_t type,IMTReportAPI *api)
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
   return(ReportContext::Generate(api,REPORT_TICK,Fill));
  }
//+------------------------------------------------------------------+
//|  Fill Table record from CSV line                                 |
//+------------------------------------------------------------------+
bool CTickReport::Fill(CTextReader &reader,TableRecord &record,const ReportContext&)
  {
//--- fill table record
   reader.String  (record.fdm_id        );
   reader.Date    (record.batch_date    );
   reader.String  (record.product_code  );
   reader.DateTime(record.quote_datetime);
   reader.Double  (record.bid_price     );
   reader.Double  (record.ask_price     );
//---
   return(true);
  }
//+------------------------------------------------------------------+
