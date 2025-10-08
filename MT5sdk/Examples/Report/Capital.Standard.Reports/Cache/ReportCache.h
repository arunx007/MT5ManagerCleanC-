//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\ReportParameter.h"
//+------------------------------------------------------------------+
//| Parameters                                                       |
//+------------------------------------------------------------------+
#define DEFAULT_DATA_LATENCY_MAX          L"300"                            // default: maximum data latency in seconds
#define PARAMETER_DATA_LATENCY_MAX        L"Cache Rebuild Period, sec"
//+------------------------------------------------------------------+
//| Forward declarations                                             |
//+------------------------------------------------------------------+
class CReportTopBase;
class CReportDictionary;
//+------------------------------------------------------------------+
//| Report cache base class                                          |
//+------------------------------------------------------------------+
class CReportCache
  {
protected:
   //--- cache context class
   class CReportCacheContext
     {
   protected:
      enum constants
        {
         KEY_TIME_TO_LIVE=SECONDS_IN_WEEK       // cache key time to live
        };

   public:
      MTAPIRES          m_res;                  // status result code
      IMTReportAPI     &m_api;                  // report api
      IMTReportCache   *m_cache;                // report cache interface
      IMTReportCacheValue *m_value;             // report cache value interface

   public:
                        CReportCacheContext(IMTReportAPI &api,LPCWSTR name,uint32_t version,int64_t key_time_to_live=KEY_TIME_TO_LIVE);
      virtual          ~CReportCacheContext(void);
      //--- status result code
      MTAPIRES          Status(void) const { return(m_res); }
      //--- detach interface
      template<class I>
      I&                Detach(I *&intf);
     };
   //--- template class receiver
   template<class Receiver,class T>
   class CReportCacheReceiver
     {
   private:
      Receiver         &m_receiver;             // receiver object reference
      MTAPIRES          (Receiver::*m_method)(const uint64_t,const T&);   // receiver object method pointer

   public:
                        CReportCacheReceiver(Receiver &receiver,MTAPIRES(Receiver::*method)(const uint64_t,const T&)) : m_receiver(receiver),m_method(method) {}
      //--- data read
      MTAPIRES          DataRead(const uint64_t key,const void *data) { return(data ? (m_receiver.*m_method)(key,*(const T*)data) : MT_RET_ERROR); }
     };

protected:
   IMTReportAPI     &m_api;                     // report api reference
   IMTReportCache   &m_cache;                   // report cache interface reference
   IMTReportCacheValue &m_value;                // report cache value interface reference

public:
   //--- release object
   void              Release(void);
   //--- read begin/end
   MTAPIRES          ReadBegin(void)                                                                     { return(LogError(m_cache.ReadBegin(),L"read begin")); }
   MTAPIRES          ReadEnd(void)                                                                       { return(LogError(m_cache.ReadEnd(),L"read end")); }
   template<class Receiver,class T>
   MTAPIRES          Read(const IMTReportCacheKeySet &keys,Receiver &receiver,MTAPIRES(Receiver::*method)(const uint64_t,const T&));
   template<class Receiver,class T>
   MTAPIRES          ReadAll(Receiver &receiver,MTAPIRES(Receiver::*method)(const uint64_t,const T&));
   //--- read dictionary
   MTAPIRES          ReadDictionaryString(const uint32_t dictionary_id,const uint32_t pos,LPCWSTR &string) const { return(m_cache.ReadDictionaryString(dictionary_id,pos,string)); }
   MTAPIRES          ReadDictionaryPos(const uint32_t dictionary_id,LPCWSTR string,uint32_t &pos) const          { return(m_cache.ReadDictionaryPos(dictionary_id,string,pos)); }
   //--- read top names
   MTAPIRES          ReadTopNames(const uint32_t dictionary_id,const CReportTopBase &top,CReportDictionary &names) const;
   //--- report generation time ceil
   static int64_t    Now(IMTReportAPI &api,CReportParameter &params);

protected:
   explicit          CReportCache(CReportCacheContext &context);
   virtual          ~CReportCache(void);
   //--- cache write begin/end with report generation stopped check
   MTAPIRES          WriteBegin(void)                                                                    { return(IsStopped() ? MT_RET_ERR_CANCEL : m_cache.WriteBegin()); }
   MTAPIRES          WriteEnd(const bool apply)                                                          { return(CheckStopped(m_cache.WriteEnd(apply))); }
   //--- check report generation stopped
   bool              IsStopped(void) const                                                               { return(m_api.IsStopped()); }
   MTAPIRES          CheckStopped(const MTAPIRES res)                                                    { return(res==MT_RET_OK && IsStopped() ? MT_RET_ERR_CANCEL : res); }
   //--- read data
   MTAPIRES          ReadData(const uint64_t key,const void *&data,const uint32_t data_size) const;
   MTAPIRES          ReadDataFirst(const IMTReportCacheKeySet &keys,uint64_t &key,const void *&data,const uint32_t data_size) const;
   MTAPIRES          ReadDataNext(uint64_t &key,const void *&data,const uint32_t data_size) const;
   template<class Receiver>
   MTAPIRES          Read(const IMTReportCacheKeySet *keys,const uint32_t size,Receiver &receiver);
   //--- get data from cache value
   MTAPIRES          DataFromValue(const void *&data,const uint32_t data_size) const;
   //--- success logging
   template<typename... Args>
   MTAPIRES          LogOkTime(LPCWSTR msg,const int64_t from,const int64_t to,Args... args) const;
   //--- error logging
   template<typename... Args>
   MTAPIRES          LogError(const MTAPIRES res,LPCWSTR msg,Args... args) const;
   template<typename... Args>
   MTAPIRES          LogErrorTime(const MTAPIRES res,LPCWSTR msg,const int64_t from,const int64_t to,Args... args) const;
   //--- error log type
   static uint32_t   ErrorLogType(const MTAPIRES res)                                                    { return(res==MT_RET_ERR_CANCEL ? MTLogOK : MTLogAtt); }
  };
//+------------------------------------------------------------------+
//| read data from cache template method                             |
//+------------------------------------------------------------------+
template<class Receiver> MTAPIRES CReportCache::Read(const IMTReportCacheKeySet *keys,const uint32_t size,Receiver &receiver)
  {
//--- begin read data
   MTAPIRES res=ReadBegin();
   if(res!=MT_RET_OK)
      return(LogError(res,L"begin read data"));
//--- get all key set
   IMTReportCacheKeySet *keys_all=nullptr;
   if(!keys)
     {
      //--- create all keys set
      keys_all=m_api.KeySetCreate();
      if(!keys_all)
         return(LogError(MT_RET_ERR_MEM,L"create all keys set"));
      //--- get all keys set
      if((res=m_cache.ReadMissingKeys(nullptr,keys_all))!=MT_RET_OK)
        {
         keys_all->Release();
         return(LogError(res,L"get all keys set"));
        }
      //--- store all keys set pointer
      keys=keys_all;
     }
//--- iterate data
   uint64_t key=0;
   const void *data=nullptr;
   for(res=ReadDataFirst(*keys,key,data,size);res==MT_RET_OK;res=ReadDataNext(key,data,size))
      if(data)
         if((res=receiver.DataRead(key,data))!=MT_RET_OK)
           {
            LogError(res,L"read data by key %I64u",key);
            break;
           }
//--- end read data
   const MTAPIRES res_end=ReadEnd();
//--- release all keys set
   if(keys_all)
      keys_all->Release();
//--- check results
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(res);
//--- return end read result
   return(LogError(res_end,L"end read data"));
  }
//+------------------------------------------------------------------+
//| read data from cache template method                             |
//+------------------------------------------------------------------+
template<class Receiver,class T> MTAPIRES CReportCache::Read(const IMTReportCacheKeySet &keys,Receiver &receiver,MTAPIRES(Receiver::*method)(const uint64_t,const T&))
  {
   CReportCacheReceiver<Receiver,T> receiver_object(receiver,method);
   return(Read(&keys,sizeof(T),receiver_object));
  }
//+------------------------------------------------------------------+
//| read all data from cache template method                         |
//+------------------------------------------------------------------+
template<class Receiver,class T> MTAPIRES CReportCache::ReadAll(Receiver &receiver,MTAPIRES(Receiver::*method)(const uint64_t,const T&))
  {
   CReportCacheReceiver<Receiver,T> receiver_object(receiver,method);
   return(Read(nullptr,sizeof(T),receiver_object));
  }
//+------------------------------------------------------------------+
//| success logging with time                                        |
//+------------------------------------------------------------------+
template<typename... Args> MTAPIRES CReportCache::LogOkTime(LPCWSTR msg,const int64_t from,const int64_t to,Args... args) const
  {
//--- cache name
   const LPCWSTR name=m_cache.Name();
//--- format message
   CMTStr32 sfrom,sto;
   CMTStr256 str;
   str.Format(L"'%s' %s %s-%s",name ? name : L"Unknown Cache",msg ? msg : L"something",
      SMTFormat::FormatDateTime(sfrom,from,true,true),SMTFormat::FormatDateTime(sto,to,true,true));
//--- log message
   return(m_api.LoggerOut(MTLogOK,str.Str(),args...));
  }
//+------------------------------------------------------------------+
//| error logging                                                    |
//+------------------------------------------------------------------+
template<typename... Args> MTAPIRES CReportCache::LogError(const MTAPIRES res,LPCWSTR msg,Args... args) const
  {
//---- check error
   if(res==MT_RET_OK || res==MT_RET_OK_NONE)
      return(res);
//--- cache name
   const LPCWSTR name=m_cache.Name();
//--- format error message
   CMTStr256 str;
   str.Format(L"'%s' %s failed, error: %s (%u)",name ? name : L"Unknown Cache",msg ? msg : L"something",SMTFormat::FormatError(res),res);
//--- log error message
   m_api.LoggerOut(ErrorLogType(res),str.Str(),args...);
//--- return error code
   return(res);
  }
//+------------------------------------------------------------------+
//| error logging with time                                          |
//+------------------------------------------------------------------+
template<typename... Args> MTAPIRES CReportCache::LogErrorTime(const MTAPIRES res,LPCWSTR msg,const int64_t from,const int64_t to,Args... args) const
  {
//---- check error
   if(res==MT_RET_OK || res==MT_RET_OK_NONE)
      return(res);
//--- cache name
   const LPCWSTR name=m_cache.Name();
//--- format error message
   CMTStr32 sfrom,sto;
   CMTStr256 str;
   str.Format(L"'%s' %s %s-%s failed, error: %s (%u)",name ? name : L"Unknown Cache",msg ? msg : L"something",
      SMTFormat::FormatDateTime(sfrom,from,true,true),SMTFormat::FormatDateTime(sto,to,true,true),SMTFormat::FormatError(res),res);
//--- log error message
   m_api.LoggerOut(ErrorLogType(res),str.Str(),args...);
//--- return error code
   return(res);
  }
//+------------------------------------------------------------------+
//| detach interface                                                 |
//+------------------------------------------------------------------+
template<class I> I& CReportCache::CReportCacheContext::Detach(I *&intf)
  {
   I &ref_intf=*intf;
   intf=nullptr;
   return(ref_intf);
  }
//+------------------------------------------------------------------+
