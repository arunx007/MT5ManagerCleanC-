//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Messenger country config                                         |
//+------------------------------------------------------------------+
class IMTConMessengerCountry
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConMessengerCountry* country)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- phone code
   virtual LPCWSTR   PhoneCode(void) const=0;
   virtual MTAPIRES  PhoneCode(LPCWSTR code)=0;
   //--- message template
   virtual LPCWSTR   MessageTemplate(void) const=0;
   virtual MTAPIRES  MessageTemplate(LPCWSTR msg_template)=0;
  };
//+------------------------------------------------------------------+
//| Messenger group config                                           |
//+------------------------------------------------------------------+
class IMTConMessengerGroup
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConMessengerGroup* group)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- phone code
   virtual LPCWSTR   Group(void) const=0;
   virtual MTAPIRES  Group(LPCWSTR group)=0;
  };
//+------------------------------------------------------------------+
//| Messenger template config                                        |
//+------------------------------------------------------------------+
class IMTConMessengerTemplate
  {
public:
   //--- template types
   enum EnMessengerTemplateTypes
     {
      TEMPLATE_TYPE_PHONE_CONFIRMATION  =0,
      TEMPLATE_TYPE_DEPOSIT_COMPLETED   =1,
      TEMPLATE_TYPE_DEPOSIT_FAILED      =2,
      TEMPLATE_TYPE_DEPOSIT_PROCESSEED  =3,
      TEMPLATE_TYPE_WITHDRAWAL_COMPLETED=4,
      TEMPLATE_TYPE_WITHDRAWAL_CONFIRMED=5,
      TEMPLATE_TYPE_WITHDRAWAL_FAILED   =6,
      TEMPLATE_TYPE_WITHDRAWAL_VERIFIED =7,
      //--- enumeration borders
      TEMPLATE_TYPE_FIRST               =TEMPLATE_TYPE_PHONE_CONFIRMATION,
      TEMPLATE_TYPE_LAST                =TEMPLATE_TYPE_WITHDRAWAL_VERIFIED
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConMessengerTemplate* tpl)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- template type
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- template id
   virtual LPCWSTR   Id(void) const=0;
   virtual MTAPIRES  Id(LPCWSTR id)=0;
  };
//+------------------------------------------------------------------+
//| Messenger config                                                 |
//+------------------------------------------------------------------+
class IMTConMessenger
  {
public:
   //--- allowed flags
   enum EnFlags
     {
      FLAG_NONE                  =0x00000000,  // none
      FLAG_ENABLED               =0x00000001,  // messenger is enabled
      FLAG_DEFAULT               =0x00000002,  // messenger is default
      FLAG_NOTIFY_CLIENTS        =0x00000004,  // allow to notify clients
      FLAG_NOTIFY_SERVICES       =0x00000008,  // allow to use in automation and other internal services
      //--- flags borders
      FLAG_FIRST                 =FLAG_ENABLED,
      FLAG_ALL                   =FLAG_ENABLED|FLAG_DEFAULT|FLAG_NOTIFY_CLIENTS|FLAG_NOTIFY_SERVICES
     };
   //--- providers
   enum EnProviderType
     {
      //---- SMS
      PROVIDER_SMS_BULKSMS       =0,
      PROVIDER_SMS_CLICKATELL    =1,
      PROVIDER_SMS_WEBSMS        =2,
      PROVIDER_SMS_TWILIO        =3,
      PROVIDER_SMS_CMCOM         =4,
      PROVIDER_SMS_VONAGE        =5,
      PROVIDER_SMS_INFOBIP       =6,
      PROVIDER_SMS_FONIVA        =7,
      PROVIDER_SMS_SURFMAN       =8,
      PROVIDER_SMS_VOISO         =9,
      //--- SMS ranges
      PROVIDER_SMS_FIRST         =PROVIDER_SMS_BULKSMS,
      PROVIDER_SMS_LAST          =99,
      //---- Instant Messengers
      PROVIDER_IM_TELEGRAM       =100,
      PROVIDER_IM_SLACK          =101,
      PROVIDER_IM_MICROSOFT_TEAMS=102,
      //--- Instant Messengers ranges
      PROVIDER_IM_FIRST          =PROVIDER_IM_TELEGRAM,
      PROVIDER_IM_LAST           =199,
      //---- Push Services
      PROVIDER_PUSH_METAQUOTES   =200,
      PROVIDER_PUSH_UNIVERSAL    =201,
      //--- Push Services ranges
      PROVIDER_PUSH_FIRST        =PROVIDER_PUSH_METAQUOTES,
      PROVIDER_PUSH_LAST         =299,
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConMessenger* messenger)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- name
   virtual LPCWSTR   Name(void) const=0;
   virtual MTAPIRES  Name(LPCWSTR name)=0;
   //--- sender
   virtual LPCWSTR   Sender(void) const=0;
   virtual MTAPIRES  Sender(LPCWSTR sender)=0;
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
   //--- provider SubID
   virtual LPCWSTR   ProviderSubId(void) const=0;
   virtual MTAPIRES  ProviderSubId(LPCWSTR subid)=0;
   //--- provider currency
   virtual LPCWSTR   ProviderCurrency(void) const=0;
   virtual MTAPIRES  ProviderCurrency(LPCWSTR currency)=0;
   //--- provider currency rate
   virtual double    ProviderCurrencyRate(void) const=0;
   virtual MTAPIRES  ProviderCurrencyRate(const double rate)=0;
   //--- EnFlags
   virtual uint64_t    Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint64_t flags)=0;
   //--- message template
   virtual LPCWSTR   MessageTemplate(void) const=0;
   virtual MTAPIRES  MessageTemplate(LPCWSTR msg_template)=0;
   //--- country settings
   virtual MTAPIRES  CountryAdd(IMTConMessengerCountry* country)=0;
   virtual MTAPIRES  CountryUpdate(const uint32_t pos,const IMTConMessengerCountry* country)=0;
   virtual MTAPIRES  CountryDelete(const uint32_t pos)=0;
   virtual MTAPIRES  CountryClear(void)=0;
   virtual MTAPIRES  CountryShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  CountryTotal(void) const=0;
   virtual MTAPIRES  CountryNext(const uint32_t pos,IMTConMessengerCountry* country) const=0;
   //--- group settings
   virtual MTAPIRES  GroupAdd(IMTConMessengerGroup* group)=0;
   virtual MTAPIRES  GroupUpdate(const uint32_t pos,const IMTConMessengerGroup* group)=0;
   virtual MTAPIRES  GroupDelete(const uint32_t pos)=0;
   virtual MTAPIRES  GroupClear(void)=0;
   virtual MTAPIRES  GroupShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  GroupTotal(void) const=0;
   virtual MTAPIRES  GroupNext(const uint32_t pos,IMTConMessengerGroup* group) const=0;
   //--- template settings
   virtual MTAPIRES  TemplateAdd(IMTConMessengerTemplate* tpl)=0;
   virtual MTAPIRES  TemplateUpdate(const uint32_t pos,const IMTConMessengerTemplate* tpl)=0;
   virtual MTAPIRES  TemplateDelete(const uint32_t pos)=0;
   virtual MTAPIRES  TemplateClear(void)=0;
   virtual MTAPIRES  TemplateShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  TemplateTotal(void) const=0;
   virtual MTAPIRES  TemplateNext(const uint32_t pos,IMTConMessengerTemplate* tpl) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConMessenger(void) {}
  };
//+------------------------------------------------------------------+
//| Messenger config events notification interface                   |
//+------------------------------------------------------------------+
class IMTConMessengerSink
  {
public:
   virtual void      OnMessengerAdd(const IMTConMessenger*    /*config*/) {  }
   virtual void      OnMessengerUpdate(const IMTConMessenger* /*config*/) {  }
   virtual void      OnMessengerDelete(const IMTConMessenger* /*config*/) {  }
   virtual void      OnMessengerSync(void)                                {  }
  };
//+------------------------------------------------------------------+
