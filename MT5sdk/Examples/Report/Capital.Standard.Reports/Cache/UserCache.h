//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportCacheKeysTime.h"
#include "..\Tools\DatasetField.h"
#include "..\Tools\CurrencyConverterCache.h"
//+------------------------------------------------------------------+
//| User cache class                                                 |
//+------------------------------------------------------------------+
class CUserCache : public CReportCacheKeysTime
  {
public:
   //--- User structure
   //--- Update version constant below when change this structure!
   #pragma pack(push,1)
   struct UserCache
     {
      uint64_t          client_id;              // client id
      int64_t           registration;           // registration time
      uint32_t          name;                   // name pos in dictionary (valid in full cache mode only, when m_client==false)
      uint32_t          currency;               // currency pos in dictionary
      uint32_t          lead_source;            // lead source pos in dictionary
      uint32_t          lead_campaign;          // lead campaign pos in dictionary
      uint32_t          country;                // country pos in dictionary (valid in full cache mode only, when m_client==false)
     };
   #pragma pack(pop)

private:
   //--- Cache version constant
   //--- Update it when changing structure above!
   static const uint32_t s_cache_version=3;
   //--- User record
   #pragma pack(push,1)
   struct UserRecord
     {
      uint64_t          login;                  // login
      uint64_t          client_id;              // client id
      int64_t           registration;           // registration time
      wchar_t           group[64];              // group
      wchar_t           lead_source[32];        // lead source
      wchar_t           lead_campaign[32];      // lead campaign
      wchar_t           country[32];            // country (used in full cache mode only, when m_client==false)
      wchar_t           name[128];              // name (used in full cache mode only, when m_client==false)
     };
   #pragma pack(pop)

private:
   //--- cache context class
   class CUserCacheContext : public CReportCacheKeysTimeContext
     {
   public:
      IMTReportCacheKeySet *m_logins;           // user logins set

   public:
                        CUserCacheContext(IMTReportAPI &api,const bool client);
      virtual          ~CUserCacheContext(void);
     };

private:
   const bool        m_client;                  // flag determines that cache users with client attached only
   IMTReportCacheKeySet &m_logins;              // user logins set
   //--- static data
   static DatasetField s_fields[];              // request dataset fields descriptions
   static const uint32_t s_request_limit=100000;    // users request limit

public:
   //--- create class object
   static CUserCache* Create(IMTReportAPI &api,CReportParameter &params,CCurrencyConverterCache *currency,const bool client,MTAPIRES &res);
   //--- update cache
   MTAPIRES          Update(const int64_t from,const int64_t to);
   //--- update and filter cache
   MTAPIRES          UpdateFilter(const int64_t from,const int64_t to);
   //--- user logins key set
   const IMTReportCacheKeySet& Logins(void) const                                   { return(m_logins); }
   //--- user read from cache template methods
   template<class Receiver>
   MTAPIRES          ReadAll(Receiver &receiver,MTAPIRES(Receiver::*method)(const uint64_t,const UserCache&)) { return(Read(m_logins,receiver,method)); }
   template<class Receiver>
   MTAPIRES          ReadAll(Receiver &receiver)                                    { return(ReadAll(receiver,&Receiver::UserRead)); }
   //--- read user by login
   MTAPIRES          ReadUser(const uint64_t login,const UserCache *&user)            { return(ReadData(login,(const void*&)user,sizeof(*user))); }
   //--- read user strings
   MTAPIRES          ReadName(const UserCache &user,LPCWSTR &name) const            { return(ReadDictionaryString(offsetof(UserCache,name),user.name,name));             }
   MTAPIRES          ReadCurrency(const UserCache &user,LPCWSTR &currency) const    { return(ReadDictionaryString(offsetof(UserCache,currency),user.currency,currency)); }
   MTAPIRES          ReadLeadSource(const uint32_t pos,LPCWSTR &string) const           { return(ReadDictionaryString(offsetof(UserCache,lead_source),pos,string));          }
   MTAPIRES          ReadLeadCampaign(const uint32_t pos,LPCWSTR &string) const         { return(ReadDictionaryString(offsetof(UserCache,lead_campaign),pos,string));        }
   MTAPIRES          ReadCountry(const uint32_t pos,LPCWSTR &string) const              { return(ReadDictionaryString(offsetof(UserCache,country),pos,string));              }
   //--- read user strings positions
   MTAPIRES          ReadLeadSourcePos(LPCWSTR lead_source,uint32_t &pos) const         { return(ReadDictionaryPos(offsetof(UserCache,lead_source),lead_source,pos));        }
   MTAPIRES          ReadLeadCampaignPos(LPCWSTR lead_campaign,uint32_t &pos) const     { return(ReadDictionaryPos(offsetof(UserCache,lead_campaign),lead_campaign,pos));    }

private:
                     CUserCache(CUserCacheContext &context,const bool client);
   virtual          ~CUserCache(void);
   //--- write users to cache
   virtual MTAPIRES  WriteData(const IMTDataset &users,uint64_t &id_last) override;
   //--- request users from base
   virtual MTAPIRES  SelectData(IMTDataset &users,IMTDatasetRequest &request,int64_t from,int64_t to,uint64_t id_from) override;
   //--- write missing keys
   virtual MTAPIRES  WriteMissingKeys(const IMTReportCacheKeySet &keys) override { return(m_client ? MT_RET_OK : CReportCacheKeysTime::WriteMissingKeys(keys)); }
   //--- prepare users request
   virtual MTAPIRES  PrepareRequest(IMTDatasetRequest &request,const IMTReportCacheKeySet &keys) override;
   //--- fill users request
   MTAPIRES          UserRequest(IMTDatasetRequest &request,int64_t from,int64_t to,uint64_t id_from);
   //--- group currency
   MTAPIRES          GroupCurrency(IMTConGroup &config,const wchar_t (&group)[64],CMTStr32 &currency);
   //--- write user to cache
   MTAPIRES          WriteUser(const UserRecord &user,const CMTStr32 &currency);
   //--- logging
   virtual MTAPIRES  LogSelectError(const MTAPIRES res,const uint32_t keys,const int64_t from,const int64_t to,const uint64_t id_from) override
     {
      return(LogErrorTime(res,L"Accounts selection by %u logins from #%I64u with time",from,to,keys,id_from));
     }
   virtual MTAPIRES  LogWriteError(const MTAPIRES res,const int64_t from,const int64_t to) override
     {
      return(LogErrorTime(res,L"Accounts caching with time",from,to));
     }
   virtual MTAPIRES  LogWriteOk(const int64_t from,const int64_t to,const uint64_t total,const uint32_t keys,const uint64_t time_select,const uint64_t time_write) override
     {
      return(LogOkTime(L"Selected %I64u accounts by %u logins in %u ms, cached in %u ms, time",from,to,total,keys,time_select,time_write));
     }
  };
//+------------------------------------------------------------------+
