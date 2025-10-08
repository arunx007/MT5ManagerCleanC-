//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| KYC country config                                               |
//+------------------------------------------------------------------+
class IMTConKYCCountry
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConKYCCountry* country)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- country code
   virtual LPCWSTR   CountryCode(void) const=0;
   virtual MTAPIRES  CountryCode(LPCWSTR code)=0;
  };
//+------------------------------------------------------------------+
//| KYC group config                                                 |
//+------------------------------------------------------------------+
class IMTConKYCGroup
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConKYCGroup* group)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- phone code
   virtual LPCWSTR   Group(void) const=0;
   virtual MTAPIRES  Group(LPCWSTR group)=0;
  };
//+------------------------------------------------------------------+
//| KYC config                                                       |
//+------------------------------------------------------------------+
class IMTConKYC
  {
public:
   //--- allowed flags
   enum EnFlags
     {
      FLAG_NONE               =0, // none
      FLAG_ENABLED            =1, // config is enabled
      FLAG_DEFAULT            =2, // config is default
      //--- flags borders
      FLAG_FIRST              =FLAG_ENABLED,
      FLAG_ALL                =FLAG_ENABLED|FLAG_DEFAULT
     };
   //--- providers
   enum EnProviderType
     {
      //---- SMS
      PROVIDER_KYC_SUMSUB     =0,
      PROVIDER_KYC_WORLD_CHECK=1,
      PROVIDER_KYC_ESPEAR     =2,
      //--- ranges
      PROVIDER_KYC_FIRST     =PROVIDER_KYC_SUMSUB,
      PROVIDER_KYC_LAST      =PROVIDER_KYC_ESPEAR,
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConKYC* config)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- name
   virtual LPCWSTR   Name(void) const=0;
   virtual MTAPIRES  Name(LPCWSTR name)=0;
   //--- provider type
   virtual uint32_t  ProviderType(void) const=0;
   virtual MTAPIRES  ProviderType(const uint32_t provider)=0;
   //--- provider address
   virtual LPCWSTR   ProviderAddress(void) const=0;
   virtual MTAPIRES  ProviderAddress(LPCWSTR address)=0;
   //--- provider login
   virtual LPCWSTR   ProviderLogin(void) const=0;
   virtual MTAPIRES  ProviderLogin(LPCWSTR login)=0;
   //--- provider password
   virtual LPCWSTR   ProviderPassword(void) const=0;
   virtual MTAPIRES  ProviderPassword(LPCWSTR password)=0;
   //--- provider token
   virtual LPCWSTR   ProviderToken(void) const=0;
   virtual MTAPIRES  ProviderToken(LPCWSTR token)=0;
   //--- EnFlags
   virtual uint64_t    Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint64_t flags)=0;
   //--- country settings
   virtual MTAPIRES  CountryAdd(IMTConKYCCountry* country)=0;
   virtual MTAPIRES  CountryUpdate(const uint32_t pos,const IMTConKYCCountry* country)=0;
   virtual MTAPIRES  CountryDelete(const uint32_t pos)=0;
   virtual MTAPIRES  CountryClear(void)=0;
   virtual MTAPIRES  CountryShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  CountryTotal(void) const=0;
   virtual MTAPIRES  CountryNext(const uint32_t pos,IMTConKYCCountry* country) const=0;
   //--- group settings
   virtual MTAPIRES  GroupAdd(IMTConKYCGroup* group)=0;
   virtual MTAPIRES  GroupUpdate(const uint32_t pos,const IMTConKYCGroup* group)=0;
   virtual MTAPIRES  GroupDelete(const uint32_t pos)=0;
   virtual MTAPIRES  GroupClear(void)=0;
   virtual MTAPIRES  GroupShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  GroupTotal(void) const=0;
   virtual MTAPIRES  GroupNext(const uint32_t pos,IMTConKYCGroup* group) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConKYC(void) {}
  };
//+------------------------------------------------------------------+
//| KYC config events notification interface                         |
//+------------------------------------------------------------------+
class IMTConKYCSink
  {
public:
   virtual void      OnKYCAdd(const IMTConKYC*    /*config*/) {  }
   virtual void      OnKYCUpdate(const IMTConKYC* /*config*/) {  }
   virtual void      OnKYCDelete(const IMTConKYC* /*config*/) {  }
   virtual void      OnKYCSync(void)                          {  }
  };
//+------------------------------------------------------------------+
