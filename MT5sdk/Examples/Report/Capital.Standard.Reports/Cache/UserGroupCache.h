//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportCacheTime.h"
#include "..\Tools\DatasetField.h"
#include "..\Tools\CurrencyConverterCache.h"
//+------------------------------------------------------------------+
//| User group cache class                                           |
//+------------------------------------------------------------------+
class CUserGroupCache : public CReportCacheTime
  {
public:
   //--- User group structure
   //--- Update version constant below when change this structure!
   #pragma pack(push,1)
   struct UserGroup
     {
      uint32_t          currency;               // currency pos in dictionary
      uint32_t          group;                  // group pos in dictionary
     };
   #pragma pack(pop)

private:
   //--- Cache version constant
   //--- Update it when changing structure above!
   static const uint32_t s_cache_version=1;
   //--- User record
   #pragma pack(push,1)
   struct UserRecord
     {
      uint64_t          login;                  // login
      wchar_t           group[64];              // group
     };
   #pragma pack(pop)

private:
   //--- static data
   static DatasetField s_fields[];              // request dataset fields descriptions
   static const uint32_t s_request_limit=100*1024;  // users selection request limit

public:
   //--- create class object
   static CUserGroupCache* Create(IMTReportAPI &api,CReportParameter &params,CCurrencyConverterCache &currency,MTAPIRES &res);
   //--- read user by login
   MTAPIRES          ReadUser(const uint64_t login,const UserGroup *&user)         { return(ReadData(login,(const void*&)user,sizeof(*user))); }
   //--- read user strings
   MTAPIRES          ReadCurrency(const UserGroup &user,LPCWSTR &currency) const { return(ReadDictionaryString(offsetof(UserGroup,currency),user.currency,currency)); }
   MTAPIRES          ReadGroup(const uint32_t pos,LPCWSTR &string) const             { return(ReadDictionaryString(offsetof(UserGroup,group),pos,string));                }

private:
   explicit          CUserGroupCache(CReportCacheContext &context);
   virtual          ~CUserGroupCache(void)=default;
   //--- request data from base
   virtual MTAPIRES  SelectData(IMTDataset &deals,int64_t from,int64_t to,uint64_t id_from) override;
   //--- write users to cache
   virtual MTAPIRES  WriteData(const IMTDataset &data,uint64_t &id_last) override;
   //--- fill users request
   MTAPIRES          UserRequest(IMTDatasetRequest &request,int64_t from,int64_t to,uint64_t id_from);
   //--- group currency
   MTAPIRES          GroupCurrency(IMTConGroup &config,const wchar_t (&group)[64],CMTStr32 &currency);
   //--- write user to cache
   MTAPIRES          WriteUser(const UserRecord &user,const CMTStr32 &currency);
   //--- logging
   virtual MTAPIRES  LogSelectError(const MTAPIRES res,const int64_t from,const int64_t to,const uint64_t id_from) override
     {
      return(LogErrorTime(res,L"Users selection from '%I64u' with registration time",from,to,id_from));
     }
   virtual MTAPIRES  LogWriteError(const MTAPIRES res,const int64_t from,const int64_t to) override
     {
      return(LogErrorTime(res,L"Users caching with registration time",from,to));
     }
   virtual MTAPIRES  LogWriteOk(const int64_t from,const int64_t to,const uint64_t total,const uint64_t time_select,const uint64_t time_write) override
     {
      return(LogOkTime(L"Selected %I64u users in %I64u ms, cached in %I64u ms, registration time",from,to,total,time_select,time_write));
     }
  };
//+------------------------------------------------------------------+
