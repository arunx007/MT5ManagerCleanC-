//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include <in6addr.h>
//+------------------------------------------------------------------+
//| Geo record interface                                             |
//+------------------------------------------------------------------+
class IMTGeo
  {
public:
   //--- connection types
   enum EnGeoRequestFlags
     {
      GEO_REQUEST_FLAGS_CONTINENT   =0x00000001,
      GEO_REQUEST_FLAGS_COUNTRY     =0x00000002,
      GEO_REQUEST_FLAGS_CITY        =0x00000004,
      GEO_REQUEST_FLAGS_REGION      =0x00000008,
      GEO_REQUEST_FLAGS_PROVINCE    =0x00000010,
      GEO_REQUEST_FLAGS_ASN         =0x00000020,
      GEO_REQUEST_FLAGS_ASNNAME     =0x00000040,
      GEO_REQUEST_FLAGS_ISP         =0x00000080,
      GEO_REQUEST_FLAGS_ORGANIZATION=0x00000100,
      GEO_REQUEST_FLAGS_COORDINATES =0x00000200,
      GEO_REQUEST_FLAGS_DETAILS     =0x00000400,
      //--- enumeration borders
      GEO_REQUEST_FLAGS_NONE        =0x00000000,
      GEO_REQUEST_FLAGS_ADDRESS     =GEO_REQUEST_FLAGS_COUNTRY|GEO_REQUEST_FLAGS_CITY|GEO_REQUEST_FLAGS_REGION|GEO_REQUEST_FLAGS_PROVINCE,
      GEO_REQUEST_FLAGS_ALL         =GEO_REQUEST_FLAGS_CONTINENT|GEO_REQUEST_FLAGS_COUNTRY|GEO_REQUEST_FLAGS_CITY|GEO_REQUEST_FLAGS_REGION|GEO_REQUEST_FLAGS_PROVINCE|GEO_REQUEST_FLAGS_ASN|
      GEO_REQUEST_FLAGS_ASNNAME|GEO_REQUEST_FLAGS_ISP|GEO_REQUEST_FLAGS_ORGANIZATION|GEO_REQUEST_FLAGS_COORDINATES|GEO_REQUEST_FLAGS_DETAILS
     };
   //--- connection types
   enum EnGeoRecordDetails
     {
      GEO_DETAILS_PROXY_TOR         =0x00000001,   // TOR output nodes
      GEO_DETAILS_PROXY_HTTP        =0x00000002,   // different http/https proxies
      GEO_DETAILS_PROXY_VPN         =0x00000004,   // VPN servers
      GEO_DETAILS_DATACENTER        =0x00000008,   // address belongs to the data center
      GEO_DETAILS_ATTACK_MAIL       =0x00000010,   // noticed in an attack on mail systems or just a spammer
      GEO_DETAILS_ATTACK_SSH        =0x00000020,   // noticed in an attack on SSH servers, password brute force
      GEO_DETAILS_ATTACK_WEB        =0x00000040,   // noticed attacking websites
      GEO_DETAILS_ATTACK_APP        =0x00000080,   // noticed in an attack on applications (for example, on MetaTrader)
      GEO_DETAILS_BOT_BOTNET        =0x00000100,   // the botnet is clean
      GEO_DETAILS_BOT_SEARCH_ENGINE =0x00000200,   // legal search engine bot
      GEO_DETAILS_BLACKLIST         =0x00000400,   // blacklist
      GEO_DETAILS_WEBDRIVER         =0x00000800,   // webdriver/selenium/chrome headless
      GEO_DETAILS_BOT_ANY           =0x00001000,   // any bot
      GEO_DETAILS_BOT_FAKE          =0x00002000,   // fake crawlers
      //--- enumeration borders     
      GEO_DETAILS_NONE              =0x00000000,
      GEO_DETAILS_ALL               =GEO_DETAILS_PROXY_TOR |GEO_DETAILS_PROXY_HTTP|GEO_DETAILS_PROXY_VPN|GEO_DETAILS_DATACENTER|GEO_DETAILS_ATTACK_MAIL|GEO_DETAILS_ATTACK_SSH|
      GEO_DETAILS_ATTACK_WEB|GEO_DETAILS_ATTACK_APP|GEO_DETAILS_BOT_BOTNET|GEO_DETAILS_BOT_SEARCH_ENGINE|GEO_DETAILS_BLACKLIST|GEO_DETAILS_WEBDRIVER|GEO_DETAILS_BOT_ANY|GEO_DETAILS_BOT_FAKE
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTGeo* record)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- IPv4
   virtual ULONG     IPv4From(void) const=0;
   virtual ULONG     IPv4To(void) const=0;
   //--- IPv6
   virtual IN6_ADDR  IPv6From(void) const=0;
   virtual IN6_ADDR  IPv6To(void) const=0;
   //--- Geo
   virtual LPCWSTR   Continent(void) const=0;
   virtual LPCWSTR   Country(void) const=0;
   virtual LPCWSTR   City(void) const=0;
   virtual LPCWSTR   Region(void) const=0;
   virtual LPCWSTR   Province(void) const=0;
   //--- ASN
   virtual uint64_t  ASN(void) const=0;
   virtual LPCWSTR   ASNOrganization(void) const=0;
   //--- ISP
   virtual LPCWSTR   ISP(void) const=0;
   virtual LPCWSTR   ISPOrganization(void) const=0;
   //--- Geo coordinates
   virtual double    Latitude(void) const=0;
   virtual double    Longitude(void) const=0;
   //--- EnGeoDetails
   virtual uint64_t  Details(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTGeo(void) {}
  };
//+------------------------------------------------------------------+
