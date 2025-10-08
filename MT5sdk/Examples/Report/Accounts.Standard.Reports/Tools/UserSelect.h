//+------------------------------------------------------------------+
//|                           MetaTrader 5 Accounts.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "DatasetField.h"
#include "ReportLog.h"
//+------------------------------------------------------------------+
//| Select by groups base class                                      |
//+------------------------------------------------------------------+
class CGroupSelectBase
  {
   //--- API interfaces
   IMTReportAPI     *m_api;                  // report api
   IMTDatasetRequest *m_request;             // request
   IMTDatasetField  *m_field_time;           // request time field
   IMTDatasetField  *m_field_login;          // request login field
   IMTDatasetField  *m_field_group;          // request group field
   IMTDataset       *m_dataset;              // dataset
   CReportLog        m_log;                  // logger
   //--- request time
   int64_t           m_from;                 // from
   int64_t           m_to;                   // to
   bool              m_full;                 // full result flag

public:
   //--- constructor/destructor
                     CGroupSelectBase(LPCWSTR log_prefix);
   virtual          ~CGroupSelectBase(void);
   //--- clear
   void              Clear(void);
   //--- select users by group
   MTAPIRES          Select(LPCWSTR group);
   //--- select next part
   IMTDataset*       Next(MTAPIRES &res,uint64_t login);
   //--- select users by logins
   IMTDataset*       SelectUnlimited(MTAPIRES &res,const IMTReportCacheKeySet &logins);

protected:
   //--- prepare
   MTAPIRES          Prepare(IMTReportAPI *api,const DatasetField *fields,uint32_t fields_total,uint32_t limit,uint32_t field_login,uint32_t field_group);
   MTAPIRES          Prepare(IMTReportAPI *api);
   //--- request
   IMTDatasetRequest* Request(void)                            { return(m_request); }
   MTAPIRES          FieldTime(IMTDatasetField *field_time);
   MTAPIRES          FieldLogin(IMTDatasetField *field_login);
   MTAPIRES          FieldGroup(IMTDatasetField *field_group);
   //--- overridable select method
   virtual MTAPIRES  DoSelect(IMTReportAPI &api,IMTDatasetRequest &request,IMTDataset &dataset)=0;
  };
//+------------------------------------------------------------------+
//| User select                                                      |
//+------------------------------------------------------------------+
class CUserSelect : public CGroupSelectBase
  {
public:
                     CUserSelect(void) : CGroupSelectBase(L"Users ") {}

   MTAPIRES          Prepare(IMTReportAPI *api,const DatasetField *fields,const uint32_t fields_total,const uint32_t limit)
     {
      return(CGroupSelectBase::Prepare(api,fields,fields_total,limit,IMTDatasetField::FIELD_USER_LOGIN,IMTDatasetField::FIELD_USER_GROUP));
     }

protected:
   //--- select method
   virtual MTAPIRES  DoSelect(IMTReportAPI &api,IMTDatasetRequest &request,IMTDataset &dataset) { return(api.UserSelect(&request,&dataset)); }
  };
//+------------------------------------------------------------------+
//| Account select                                                   |
//+------------------------------------------------------------------+
class CAccountSelect : public CGroupSelectBase
  {
public:
                     CAccountSelect(void) : CGroupSelectBase(L"Trade accounts ") {}

   MTAPIRES          Prepare(IMTReportAPI *api,const DatasetField *fields,const uint32_t fields_total,const uint32_t limit)
     {
      return(CGroupSelectBase::Prepare(api,fields,fields_total,limit,IMTDatasetField::FIELD_ACCOUNT_LOGIN,IMTDatasetField::FIELD_ACCOUNT_GROUP));
     }

protected:
   //--- select method
   virtual MTAPIRES  DoSelect(IMTReportAPI &api,IMTDatasetRequest &request,IMTDataset &dataset) { return(api.UserAccountSelect(&request,&dataset)); }
  };
//+------------------------------------------------------------------+

