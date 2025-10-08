//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "LoggerCache.h"
//+------------------------------------------------------------------+
//| Timeout log record                                               |
//+------------------------------------------------------------------+
const wchar_t CLoggerResult::s_timeout_record[]=L"Journal request terminated due timeout";
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CLoggerResult::CLoggerResult(void) :
   m_to(0),m_to_result(0),m_logs(nullptr),m_logs_total(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CLoggerResult::~CLoggerResult(void)
  {
  }
//+------------------------------------------------------------------+
//| Clear contents                                                   |
//+------------------------------------------------------------------+
void CLoggerResult::Clear(IMTReportAPI &api)
  {
//--- reset time
   m_to=0;
   m_to_result=0;
//--- reset record count
   m_logs_total=0;
//--- free records
   if(m_logs)
      api.Free(m_logs);
//--- reset records pointer
   m_logs=nullptr;
  }
//+------------------------------------------------------------------+
//| Logger request                                                   |
//+------------------------------------------------------------------+
MTAPIRES CLoggerResult::Request(IMTReportAPI &api,const int64_t from,const int64_t to,const CMTStr64 &request)
  {
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- clear contents
   Clear(api);
//--- logger request
   MTAPIRES res=api.LoggerRequest(MTLogModeStd,MTLogTypeAll,from,to,request.Str(),m_logs,m_logs_total);
   if(res!=MT_RET_OK)
      return(res);
//--- store time
   m_to=m_to_result=to;
//--- check for records
   if(!m_logs || !m_logs_total)
      return(MT_RET_OK);
//--- check last record for timeout
   const MTLogRecord &last=m_logs[m_logs_total-1];
   if(last.code==MTLogFolder && CMTStr::Find(last.message,s_timeout_record)<0)
      return(MT_RET_OK);
//--- remove last record
   m_logs_total--;
//--- check end date 
   if(last.datetime>to)
      return(MT_RET_OK);
//--- store result end date
   m_to_result=last.datetime;
//--- calculate result end date
   uint32_t days=0;
   for(;!m_to_result && days<m_logs_total;days++)
      m_to_result=m_logs[m_logs_total-days-1].datetime;
   if(!m_to_result)
      m_to_result=from;
   if(days)
      m_to_result+=SECONDS_IN_DAY*(days-1);
//--- check result end date
   if(m_to_result<from)
      return(MT_RET_ERR_TIMEOUT);
//--- check same day as request from
   if(SMTTime::DayBegin(from)==SMTTime::DayBegin(m_to_result))
      return(MT_RET_ERR_TIMEOUT);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CLoggerCache::CLoggerCache(IMTReportAPI *api) :
   m_api(api),m_ctm_from(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CLoggerCache::~CLoggerCache(void)
  {
//--- clear all
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear contents                                                   |
//+------------------------------------------------------------------+
void CLoggerCache::Clear(void)
  {
//--- clear request
   m_request.Clear();
   m_ctm_from=0;
//--- clear result
   ClearResult();
  }
//+------------------------------------------------------------------+
//| Clear results                                                    |
//+------------------------------------------------------------------+
void CLoggerCache::ClearResult(void)
  {
//--- clear results
   if(m_api)
      for(uint32_t i=0;i<m_logs.Total();i++)
         m_logs[i].Clear(*m_api);
//--- clear results array
   m_logs.Clear();
  }
//+------------------------------------------------------------------+
//| Add id to request                                                |
//+------------------------------------------------------------------+
bool CLoggerCache::Add(const uint64_t id,const int64_t ctm)
  {
//--- format id
   CMTStr32 str;
   str.Format(L"#%I64u",id);
//--- add request string
   if(!AddRequest(str))
      return(false);
//--- update request time
   UpdateTime(ctm);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Add ids pair to request                                          |
//+------------------------------------------------------------------+
bool CLoggerCache::AddPair(const uint64_t id1,const int64_t ctm1,const uint64_t id2,const int64_t ctm2)
  {
//--- format second id
   CMTStr32 str_id2;
   str_id2.Format(L"#%I64u",id2);
   bool second_id_exist=false;
//--- search for second id in request
   for(int32_t pos=m_request.Find(str_id2.Str());pos>=0;pos=m_request.Find(str_id2.Str(),pos+str_id2.Len()))
      if(!iswdigit(m_request[pos+str_id2.Len()]))
        {
         second_id_exist=true;
         break;
        }
//--- format first id
   CMTStr64 str;
   str.Format(L"#%I64u",id1);
//--- append second id
   if(!second_id_exist)
     {
      str.Append(L'|');
      str.Append(str_id2);
     }
//--- add request string
   if(!AddRequest(str))
      return(false);
//--- update request time
   UpdateTime(ctm1);
   UpdateTime(ctm2);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Add request string                                               |
//+------------------------------------------------------------------+
bool CLoggerCache::AddRequest(const CMTStr &request)
  {
//--- check space available in request string
   if(m_request.Len()+request.Len()+1>=m_request.Max())
      return(false);
//--- append delimiter
   if(!m_request.Empty())
      m_request.Append(L'|');
//--- append request substring
   m_request.Append(request);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Update request time                                              |
//+------------------------------------------------------------------+
void CLoggerCache::UpdateTime(const int64_t ctm)
  {
//--- update time
   if(!m_ctm_from || (ctm && m_ctm_from>ctm))
      m_ctm_from=ctm;
  }
//+------------------------------------------------------------------+
//| Logger request                                                   |
//+------------------------------------------------------------------+
MTAPIRES CLoggerCache::Request(void)
  {
//--- check api
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- check for stop
   MTAPIRES res=m_api->IsStopped();
   if(res!=MT_RET_OK)
      return(res);
//--- check request
   if(m_request.Empty())
      return(MT_RET_OK);
//--- report time
   int64_t from=m_api->ParamFrom();
   from=std::max(m_ctm_from,from);
   const int64_t to=m_api->ParamTo();
//--- clear results
   ClearResult();
//--- request while results incomplete
   while(from && from<to)
     {
      //--- append new result
      CLoggerResult *result=m_logs.Append();
      if(!result)
         return(MT_RET_ERR_MEM);
      new(result) CLoggerResult();
      //--- logger request
      res=result->Request(*m_api,from,to,m_request);
      if(res==MT_RET_ERR_TIMEOUT)
         return(MT_RET_OK);
      if(res!=MT_RET_OK)
         return(res);
      //--- check result for completenes
      if(result->Complete())
         break;
      //--- check for stop
      res=m_api->IsStopped();
      if(res!=MT_RET_OK)
         return(res);
      //--- update from time
      from=result->ResultTo();
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Log record filter helper class                                   |
//+------------------------------------------------------------------+
class CLoggerCacheFilter
  {
private:
   CMTStr32          m_pattern;  // filter patter

public:
   //--- constructor
   explicit CLoggerCacheFilter(const uint64_t id)
     {
      //--- format id
      if(id)
         m_pattern.Format(L"#%I64u",id);
     }
   //--- filter log record
   bool FilterLog(const MTLogRecord &log) const
     {
      //--- check empty pattern
      if(m_pattern.Empty())
         return(true);
      //--- search pattern in record and check for full number
      LPCWSTR str=log.message;
      int32_t pos;
      while((pos=CMTStr::Find(str,m_pattern.Str()))>=0)
        {
         //--- check for full number
         str+=pos+m_pattern.Len();
         if(!iswdigit(*str))
            return(false);
        }
      //--- not found
      return(true);
     }
  };
//+------------------------------------------------------------------+
//| Write log records to html                                        |
//+------------------------------------------------------------------+
MTAPIRES CLoggerCache::HtmlWriteLog(const uint64_t id1,const uint64_t id2/*=0*/) const
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- filter objects
   CLoggerCacheFilter filter1(id1),filter2(id2);
//--- iterate all results
   CMTStr64 str;
   int64_t last=0;
   MTAPIRES res=MT_RET_OK;
   for(uint32_t i=0;i<m_logs.Total();i++)
     {
      //--- result
      const CLoggerResult &result=m_logs[i];
      //--- iterate all records
      for(uint32_t j=0;const MTLogRecord *log=result.Record(j);j++)
        {
         //--- check for stop
         if((res=m_api->IsStopped())!=MT_RET_OK)
            return(res);
         //--- log record
         if(log->datetime<last)
            continue;
         //--- skip folders
         if(log->code==MTLogFolder)
            continue;
         //--- filter record
         if(filter1.FilterLog(*log) && (!id2 || filter2.FilterLog(*log)))
            continue;
         //--- write string to html
         if((res=m_api->HtmlWrite(L"%s&emsp;",SMTFormat::FormatDateTime(str,log->datetime,true,true)))!=MT_RET_OK)
            return(res);
         if((res=m_api->HtmlWriteSafe(log->message,IMTReportAPI::HTML_SAFE_NONE))!=MT_RET_OK)
            return(res);
         if((res=m_api->HtmlWriteSafe(L"\r\n",IMTReportAPI::HTML_SAFE_NONE))!=MT_RET_OK)
            return(res);
        }
      //--- last record time
      last=result.ResultTo();
     }
//--- check for incomplete log
   if(m_logs.Total() && !m_logs[m_logs.Total()-1].Complete())
     {
      //--- write string to html
      if((res=m_api->HtmlWrite(L"%s&emsp;",SMTFormat::FormatDateTime(str,last,false,false)))!=MT_RET_OK)
         return(res);
      if((res=m_api->HtmlWriteSafe(CLoggerResult::s_timeout_record,IMTReportAPI::HTML_SAFE_NONE))!=MT_RET_OK)
         return(res);
      if((res=m_api->HtmlWriteSafe(L"\r\n",IMTReportAPI::HTML_SAFE_NONE))!=MT_RET_OK)
         return(res);
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CLoggerCacheHolder::CLoggerCacheHolder(IMTReportAPI *api) :
   m_api(api),m_cache(api),m_pos_beg(0),m_pos_end(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CLoggerCacheHolder::~CLoggerCacheHolder(void)
  {
  }
//+------------------------------------------------------------------+
//| Check cache for item index                                       |
//+------------------------------------------------------------------+
bool CLoggerCacheHolder::CheckCache(const uint32_t pos)
  {
//--- check item index in range
   if(pos>=m_pos_beg && pos<m_pos_end)
      return(true);
//--- clear cache
   m_cache.Clear();
//--- update index range
   m_pos_beg=m_pos_end=pos;
//--- cache miss
   return(false);
  }
//+------------------------------------------------------------------+
//| Cache positions array items                                      |
//+------------------------------------------------------------------+
const IMTPosition* CLoggerCacheHolder::CachePositions(const IMTPositionArray &positions,const uint32_t pos,MTAPIRES &res)
  {
//--- check cache
   if(!CheckCache(pos))
     {
      //--- add positions to request
      for(;const IMTPosition *position=positions.Next(m_pos_end);m_pos_end++)
         if(!AddPosition(*position))
            break;
      //--- do request
      if((res=m_cache.Request())!=MT_RET_OK)
         return(nullptr);
     }
//--- get posititon from array
   const IMTPosition *position=positions.Next(pos);
   res=position ? MT_RET_OK : MT_RET_ERR_PARAMS;
//--- return position
   return(position);
  }
//+------------------------------------------------------------------+
//| Cache orders array items                                         |
//+------------------------------------------------------------------+
const IMTOrder* CLoggerCacheHolder::CacheOrders(const IMTOrderArray &orders,const uint32_t pos,MTAPIRES &res)
  {
//--- check cache
   if(!CheckCache(pos))
     {
      //--- add orders to request
      for(;const IMTOrder *order=orders.Next(m_pos_end);m_pos_end++)
         if(!AddOrder(*order))
            break;
      //--- do request
      if((res=m_cache.Request())!=MT_RET_OK)
         return(nullptr);
     }
//--- get order from array
   const IMTOrder *order=orders.Next(pos);
   res=order ? MT_RET_OK : MT_RET_ERR_PARAMS;
//--- return order
   return(order);
  }
//+------------------------------------------------------------------+
//| Cache deals array items                                          |
//+------------------------------------------------------------------+
const IMTDeal* CLoggerCacheHolder::CacheDeals(const IMTDealArray &deals,const uint32_t pos,MTAPIRES &res)
  {
//--- check cache
   if(!CheckCache(pos))
     {
      //--- add deals to request
      for(;const IMTDeal *deal=deals.Next(m_pos_end);m_pos_end++)
         if(!AddDeal(*deal))
            break;
      //--- do request
      if((res=m_cache.Request())!=MT_RET_OK)
         return(nullptr);
     }
//--- get deal from array
   const IMTDeal *deal=deals.Next(pos);
   res=deal ? MT_RET_OK : MT_RET_ERR_PARAMS;
//--- return deal
   return(deal);
  }
//+------------------------------------------------------------------+
//| Add position to request                                          |
//+------------------------------------------------------------------+
bool CLoggerCacheHolder::AddPosition(const IMTPosition &position)
  {
//--- add request
   return(m_cache.Add(position.Position(),position.TimeCreate()));
  }
//+------------------------------------------------------------------+
//| Write position log to html                                       |
//+------------------------------------------------------------------+
MTAPIRES CLoggerCacheHolder::HtmlWriteLogPosition(const IMTPosition &position)
  {
//--- write log to html
   return(m_cache.HtmlWriteLog(position.Position()));
  }
//+------------------------------------------------------------------+
//| Add order to request                                             |
//+------------------------------------------------------------------+
bool CLoggerCacheHolder::AddOrder(const IMTOrder &order)
  {
//--- add request
   return(m_cache.Add(order.Order(),order.TimeSetup()));
  }
//+------------------------------------------------------------------+
//| Write order log to html                                          |
//+------------------------------------------------------------------+
MTAPIRES CLoggerCacheHolder::HtmlWriteLogOrder(const IMTOrder &order)
  {
//--- write log to html
   return(m_cache.HtmlWriteLog(order.Order()));
  }
//+------------------------------------------------------------------+
//| Add deal to request                                              |
//+------------------------------------------------------------------+
bool CLoggerCacheHolder::AddDeal(const IMTDeal &deal)
  {
//--- check modification flags
   if((deal.ModificationFlags()&IMTDeal::MODIFY_FLAGS_POSITION))
     {
      //--- deal params
      int64_t ctm=deal.Time();
      const uint64_t position_id=deal.PositionID();
      if(m_api)
        {
         //--- receive position
         if(IMTPosition *position=m_api->PositionCreate())
           {
            //--- logs request date will be from position creation
            if(m_api->PositionGetByTicket(position_id,position)==MT_RET_OK)
               ctm=position->TimeCreate();
            //--- release position
            position->Release();
           }
        }
      //--- add request
      return(m_cache.AddPair(deal.Deal(),deal.Time(),position_id,ctm));
     }
//--- add request
   return(m_cache.Add(deal.Deal(),deal.Time()));
  }
//+------------------------------------------------------------------+
//| Write deal log to html                                           |
//+------------------------------------------------------------------+
MTAPIRES CLoggerCacheHolder::HtmlWriteLogDeal(const IMTDeal &deal)
  {
//--- check modification flags and write log to html
   if((deal.ModificationFlags()&IMTDeal::MODIFY_FLAGS_POSITION))
      return(m_cache.HtmlWriteLog(deal.Deal(),deal.PositionID()));
//--- write log to html
   return(m_cache.HtmlWriteLog(deal.Deal()));
  }
//+------------------------------------------------------------------+
