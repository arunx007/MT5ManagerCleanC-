//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "HistorySelect.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CHistorySelectBase::CHistorySelectBase(const LPCWSTR log_prefix) :
   CReportSelect(log_prefix),
   m_field_time(nullptr),m_field_login(nullptr),m_from(0),m_to(0),m_month_end(0),m_full(false)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CHistorySelectBase::~CHistorySelectBase(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CHistorySelectBase::Clear(void)
  {
//--- base call
   CReportSelect::Clear();
//--- request time field interface
   if(m_field_time)
     {
      m_field_time->Release();
      m_field_time=nullptr;
     }
//--- request login field interface
   if(m_field_login)
     {
      m_field_login->Release();
      m_field_login=nullptr;
     }
//--- reset time
   m_from=0;
   m_to=0;
   m_month_end=0;
   m_full=false;
  }
//+------------------------------------------------------------------+
//| Prepare request                                                  |
//+------------------------------------------------------------------+
MTAPIRES CHistorySelectBase::Prepare(IMTReportAPI *api,const DatasetField *fields,const uint32_t fields_total,const uint32_t limit,const uint32_t field_login,const uint32_t field_time)
  {
//--- checks
   if(!api || !fields || !fields_total)
      return(MT_RET_ERR_PARAMS);
//--- prepare account select object
   MTAPIRES res=Initialize(api);
   if(res!=MT_RET_OK)
      return(res);
   if(!m_request)
      return(MT_RET_ERROR);
//--- fill request
   CDatasetRequest composer(fields,fields_total,*m_request);
   while(const uint32_t id=composer.Next(res))
      if(id==field_login)
         res=FieldLogin(composer.FieldAddReference(res));
      else
         if(id==field_time)
            res=FieldTime(composer.FieldAddReference(res));
         else
            res=composer.FieldAdd();
   if(res!=MT_RET_OK)
      return(res);
//--- limit the number of rows of the resulting dataset
   return(m_request->RowLimit(limit));
  }
//+------------------------------------------------------------------+
//| deal request time field                                          |
//+------------------------------------------------------------------+
MTAPIRES CHistorySelectBase::FieldTime(IMTDatasetField *field_time)
  {
//--- checks
   if(!field_time)
      return(MT_RET_ERR_PARAMS);
//--- store field
   m_field_time=field_time;
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| deal request login field                                         |
//+------------------------------------------------------------------+
MTAPIRES CHistorySelectBase::FieldLogin(IMTDatasetField *field_login)
  {
//--- checks
   if(!field_login)
      return(MT_RET_ERR_PARAMS);
//--- store field
   m_field_login=field_login;
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Select deals                                                     |
//+------------------------------------------------------------------+
MTAPIRES CHistorySelectBase::Select(const int64_t from,const int64_t to,const IMTReportCacheKeySet *logins)
  {
//--- checks
   if(from>to || !m_field_time || !m_field_login)
      return(MT_RET_ERR_PARAMS);
//--- initialize time
   m_from=from;
   m_to=to;
   m_month_end=MonthEnd(from,to);
   m_full=false;
//--- update request login set
   if(logins)
     {
      MTAPIRES res=m_field_login->WhereUIntSet(logins->Array(),logins->Total());
      if(res!=MT_RET_OK)
         return(res);
     }
//--- update request time
   return(m_field_time->BetweenInt(m_from,m_month_end));
  }
//+------------------------------------------------------------------+
//| Select next part                                                 |
//+------------------------------------------------------------------+
IMTDataset* CHistorySelectBase::Next(MTAPIRES &res,uint64_t &login)
  {
//--- checks
   if(!m_api || !m_request || !m_field_time || !m_field_login || !m_dataset)
     {
      res=MT_RET_ERR_PARAMS;
      return(nullptr);
     }
//--- check last full request
   if(m_full)
     {
      res=MT_RET_OK;
      //--- reset login
      login=0;
      //--- next month
      m_from=m_month_end+1;
      m_month_end=MonthEnd(m_from,m_to);
      if(m_from>=m_to)
         return(nullptr);
      //--- update request time
      if((res=m_field_time->BetweenInt(m_from,m_month_end))!=MT_RET_OK)
         return(nullptr);
     }
//--- update request login range
   if((res=m_field_login->BetweenUInt(login,UINT64_MAX))!=MT_RET_OK)
      return(nullptr);
//--- select deals
   res=DoSelect(*m_api,*m_request,*m_dataset);
//--- partial result
   m_full=res!=MT_RET_ERR_PARTIAL;
   if(m_full)
     {
      //--- handle error
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
        {
         m_log.ErrorTime(res,L"selection from #%I64u with time",m_from,m_month_end,login);
         return(nullptr);
        }
     }
   else
      res=MT_RET_OK;
//--- return dataset
   return(m_dataset);
  }
//+------------------------------------------------------------------+
//| end of month                                                     |
//+------------------------------------------------------------------+
int64_t CHistorySelectBase::MonthEnd(int64_t ctm,const int64_t to)
  {
   ctm=SMTTime::MonthBegin(SMTTime::MonthBegin(ctm)+SECONDS_IN_MONTH+SECONDS_IN_WEEK)-1;
   return(std::min(ctm,to));
  }
//+------------------------------------------------------------------+
