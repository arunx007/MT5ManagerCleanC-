//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "CustomerReport.h"
#include "..\Tools\TextReader.h"
//+------------------------------------------------------------------+
//| Report description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CCustomerReport::s_info=
  {
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_ANY,
   L"Customer",
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
ReportColumn CCustomerReport::s_columns[]=
   //id                          ,name                ,type                              ,width,width_max,offset                                     ,size                                 ,digits,flags
  {{ COLUMN_BATCH_DATE           ,L"BATCH_DATE"             ,IMTDatasetColumn::TYPE_DATE      ,1     ,0        ,offsetof(TableRecord,batch_date           ),0                                    ,0     ,0  },
   { COLUMN_CUSTACCT_ID          ,L"CUSTACCT_ID"            ,IMTDatasetColumn::TYPE_USER_LOGIN,1     ,0        ,offsetof(TableRecord,custacct_id          ),0                                    ,0     ,0  },
   { COLUMN_FDM_ID               ,L"FDM_ID"                 ,IMTDatasetColumn::TYPE_STRING    ,1     ,0        ,offsetof(TableRecord,fdm_id               ),MtFieldSize(TableRecord,fdm_id)      ,0     ,0  },
   { COLUMN_CUST_NAME            ,L"CUST_NAME"              ,IMTDatasetColumn::TYPE_STRING    ,1     ,0        ,offsetof(TableRecord,cust_name            ),MtFieldSize(TableRecord,cust_name)   ,0     ,0  },
   { COLUMN_CUST_BIRH_DATE       ,L"CUST_BIRTH_DATE"        ,IMTDatasetColumn::TYPE_DATE      ,1     ,0        ,offsetof(TableRecord,cust_birth_date      ),0                                    ,0     ,0  },
   { COLUMN_CUST_TYPE            ,L"CUST_TYPE"              ,IMTDatasetColumn::TYPE_STRING    ,1     ,0        ,offsetof(TableRecord,cust_type            ),MtFieldSize(TableRecord,cust_type)   ,0     ,0  },
   { COLUMN_CUST_GROUP           ,L"CUST_GROUP"             ,IMTDatasetColumn::TYPE_STRING    ,1     ,0        ,offsetof(TableRecord,cust_group           ),MtFieldSize(TableRecord,cust_group)  ,0     ,0  },
   { COLUMN_COUNTRY_TYPE         ,L"COUNTRY_TYPE"           ,IMTDatasetColumn::TYPE_STRING    ,1     ,0        ,offsetof(TableRecord,country_type         ),MtFieldSize(TableRecord,country_type),0     ,0  },
   { COLUMN_NET_LIQUIDATING_VALUE,L"NET_LIQUIDATING_VALUE"  ,IMTDatasetColumn::TYPE_DOUBLE    ,1     ,0        ,offsetof(TableRecord,net_liquidating_value),0                                    ,2     ,0  },
   { CULUMN_ACCT_OPEN_DATE       ,L"ACCT_OPEN_DATE"         ,IMTDatasetColumn::TYPE_DATE      ,1     ,0        ,offsetof(TableRecord,acct_open_date       ),0                                    ,0     ,0  }
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CCustomerReport::CCustomerReport(void)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CCustomerReport::~CCustomerReport(void)
  {
  }
//+------------------------------------------------------------------+
//| Get report descriptor                                            |
//+------------------------------------------------------------------+
MTAPIRES CCustomerReport::Info(MTReportInfo &info)
  {
   info=s_info;
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Remove report class                                              |
//+------------------------------------------------------------------+
void CCustomerReport::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Generate report                                                  |
//+------------------------------------------------------------------+
MTAPIRES CCustomerReport::Generate(const uint32_t type,IMTReportAPI *api)
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
   return(ReportContext::Generate(api,REPORT_CUSTOMER,Fill));
  }
//+------------------------------------------------------------------+
//|  Fill Table record from CSV line                                 |
//+------------------------------------------------------------------+
bool CCustomerReport::Fill(CTextReader &reader,TableRecord &record,const ReportContext&)
  {
//--- fill table record
   reader.Date   (record.batch_date           );
   reader.Integer(record.custacct_id          );
   reader.String (record.fdm_id               );
   reader.String (record.cust_name            );
   reader.Date   (record.cust_birth_date      );
   reader.String (record.cust_type            );
   reader.String (record.cust_group           );
   reader.String (record.country_type         );
   reader.Double (record.net_liquidating_value);
//---
   return(true);
  }
//+------------------------------------------------------------------+
