//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportSelect.h"
#include "DatasetField.h"
//+------------------------------------------------------------------+
//| Select history base class                                        |
//+------------------------------------------------------------------+
class CHistorySelectBase : public CReportSelect
  {
   //--- API interfaces
   IMTDatasetField  *m_field_time;           // request time field
   IMTDatasetField  *m_field_login;          // request login field
   //--- request time
   int64_t           m_from;                 // from
   int64_t           m_to;                   // to
   int64_t           m_month_end;            // month end
   bool              m_full;                 // full result flag

public:
   //--- constructor/destructor
                     CHistorySelectBase(LPCWSTR log_prefix);
   virtual          ~CHistorySelectBase(void);
   //--- clear
   virtual void      Clear(void) override;
   //--- request fields
   MTAPIRES          FieldTime(IMTDatasetField *field_time);
   MTAPIRES          FieldLogin(IMTDatasetField *field_login);
   //--- select history by time and logins
   MTAPIRES          Select(int64_t from,int64_t to,const IMTReportCacheKeySet *logins);
   //--- select next part
   IMTDataset*       Next(MTAPIRES &res,uint64_t &login);
   //--- select only first history item by logins
   IMTDataset*       SelectFirst(MTAPIRES &res,const IMTReportCacheKeySet *logins);

protected:
   //--- prepare
   MTAPIRES          Prepare(IMTReportAPI *api,const DatasetField *fields,uint32_t fields_total,uint32_t limit,uint32_t field_login,uint32_t field_time);
   //--- overridable select method
   virtual MTAPIRES  DoSelect(IMTReportAPI &api,IMTDatasetRequest &request,IMTDataset &dataset)=0;
   //--- end of month
   static int64_t    MonthEnd(int64_t ctm,int64_t to);
  };
//+------------------------------------------------------------------+
//| Order history select                                             |
//+------------------------------------------------------------------+
class CHistorySelect : public CHistorySelectBase
  {
public:
                     CHistorySelect(void) : CHistorySelectBase(L"History ") {}
protected:
   //--- select method
   virtual MTAPIRES  DoSelect(IMTReportAPI &api,IMTDatasetRequest &request,IMTDataset &dataset) { return(api.HistorySelect(&request,&dataset)); }
  };
//+------------------------------------------------------------------+
//| Deal history select                                              |
//+------------------------------------------------------------------+
class CDealSelect : public CHistorySelectBase
  {
public:
                     CDealSelect(void) : CHistorySelectBase(L"Deals ") {}

protected:
   //--- select method
   virtual MTAPIRES  DoSelect(IMTReportAPI &api,IMTDatasetRequest &request,IMTDataset &dataset) { return(api.DealSelect(&request,&dataset)); }
  };
//+------------------------------------------------------------------+

