//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "MT5APIOnline.h"
//+------------------------------------------------------------------+
//| User record interface                                            |
//+------------------------------------------------------------------+
class IMTUser
  {
public:
   //--- client rights bit flags
   enum EnUsersRights
     {
      USER_RIGHT_NONE             =0x0000000000000000,  // none
      USER_RIGHT_ENABLED          =0x0000000000000001,  // client allowed to connect
      USER_RIGHT_PASSWORD         =0x0000000000000002,  // client allowed to change password
      USER_RIGHT_TRADE_DISABLED   =0x0000000000000004,  // client trading disabled
      USER_RIGHT_INVESTOR         =0x0000000000000008,  // client is investor
      USER_RIGHT_CONFIRMED        =0x0000000000000010,  // client certificate confirmed
      USER_RIGHT_TRAILING         =0x0000000000000020,  // trailing stops are allowed
      USER_RIGHT_EXPERT           =0x0000000000000040,  // expert advisors are allowed
      USER_RIGHT_OBSOLETE         =0x0000000000000080,  // obsolete value
      USER_RIGHT_REPORTS          =0x0000000000000100,  // trade reports are allowed
      USER_RIGHT_READONLY         =0x0000000000000200,  // client is readonly
      USER_RIGHT_RESET_PASS       =0x0000000000000400,  // client must change password at next login
      USER_RIGHT_OTP_ENABLED      =0x0000000000000800,  // client allowed to use one-time password
      USER_RIGHT_SPONSORED_HOSTING=0x0000000000002000,  // client allowed to use sponsored by broker MetaTrader Virtual Hosting
      USER_RIGHT_API_ENABLED      =0x0000000000004000,  // client API are allowed
      USER_RIGHT_PUSH_NOTIFICATION=0x0000000000008000,  // allow to subscribe on trade notifications
      USER_RIGHT_TECHNICAL        =0x0000000000010000,  // technical account
      USER_RIGHT_EXCLUDE_REPORTS  =0x0000000000020000,  // exclude from reports
      //--- enumeration borders
      USER_RIGHT_DEFAULT   =USER_RIGHT_ENABLED |USER_RIGHT_PASSWORD|USER_RIGHT_TRAILING|USER_RIGHT_EXPERT|USER_RIGHT_REPORTS,
      USER_RIGHT_ALL       =USER_RIGHT_ENABLED |USER_RIGHT_PASSWORD |USER_RIGHT_TRADE_DISABLED|
      USER_RIGHT_INVESTOR  | USER_RIGHT_CONFIRMED   | USER_RIGHT_TRAILING |
      USER_RIGHT_EXPERT    | USER_RIGHT_REPORTS     | USER_RIGHT_READONLY |
      USER_RIGHT_RESET_PASS| USER_RIGHT_OTP_ENABLED | USER_RIGHT_SPONSORED_HOSTING | USER_RIGHT_API_ENABLED | USER_RIGHT_PUSH_NOTIFICATION |
      USER_RIGHT_TECHNICAL | USER_RIGHT_EXCLUDE_REPORTS
     };
   //--- password types
   enum EnUsersPasswords
     {
      USER_PASS_MAIN    =0,
      USER_PASS_INVESTOR=1,
      USER_PASS_API     =2,
      //--- enumeration borders
      USER_PASS_FIRST   =USER_PASS_MAIN,
      USER_PASS_LAST    =USER_PASS_API,
     };
   //--- connection types
   enum EnUsersConnectionTypes
     {
      //--- client types
      USER_TYPE_CLIENT            =0,
      USER_TYPE_CLIENT_WINMOBILE  =1,
      USER_TYPE_CLIENT_WINPHONE   =2,
      USER_TYPE_CLIENT_API_WEB    =3,
      USER_TYPE_CLIENT_IPHONE     =4,
      USER_TYPE_CLIENT_ANDROID    =5,
      USER_TYPE_CLIENT_BLACKBERRY =6,
      USER_TYPE_CLIENT_WEB        =11,
      //--- manager types
      USER_TYPE_ADMIN             =32,
      USER_TYPE_MANAGER           =33,
      USER_TYPE_MANAGER_API       =34,
      USER_TYPE_ADMIN_API         =36,
      USER_TYPE_MANAGER_API_WEB   =37,
      //--- enumeration borders
      USER_TYPE_FIRST             =USER_TYPE_CLIENT,
      USER_TYPE_LAST              =USER_TYPE_MANAGER_API_WEB
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTUser* user)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- login
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- group
   virtual LPCWSTR   Group(void) const=0;
   virtual MTAPIRES  Group(LPCWSTR group)=0;
   //--- certificate serial number
   virtual uint64_t    CertSerialNumber(void) const=0;
   //--- EnUsersRights
   virtual uint64_t    Rights(void) const=0;
   virtual MTAPIRES  Rights(const uint64_t rights)=0;
   //--- registration datetime (filled by MT5)
   virtual int64_t     Registration(void) const=0;
   virtual int64_t     LastAccess(void) const =0;
   virtual LPCWSTR   LastIP(MTAPISTR& ip) const=0;
   //--- name
   virtual LPCWSTR   Name(void) const=0;
   virtual MTAPIRES  Name(LPCWSTR name)=0;
   //--- company
   virtual LPCWSTR   Company(void) const=0;
   virtual MTAPIRES  Company(LPCWSTR id)=0;
   //--- external system account (exchange, ECN, etc)
   virtual LPCWSTR   Account(void) const=0;
   virtual MTAPIRES  Account(LPCWSTR account)=0;
   //--- country
   virtual LPCWSTR   Country(void) const=0;
   virtual MTAPIRES  Country(LPCWSTR account)=0;
   //--- client language (WinAPI LANGID)
   virtual uint32_t  Language(void) const=0;
   virtual MTAPIRES  Language(const uint32_t language)=0;
   //--- city
   virtual LPCWSTR   City(void) const=0;
   virtual MTAPIRES  City(LPCWSTR city)=0;
   //--- state
   virtual LPCWSTR   State(void) const=0;
   virtual MTAPIRES  State(LPCWSTR state)=0;
   //--- ZIP code
   virtual LPCWSTR   ZIPCode(void) const=0;
   virtual MTAPIRES  ZIPCode(LPCWSTR code)=0;
   //--- address
   virtual LPCWSTR   Address(void) const=0;
   virtual MTAPIRES  Address(LPCWSTR code)=0;
   //--- phone
   virtual LPCWSTR   Phone(void) const=0;
   virtual MTAPIRES  Phone(LPCWSTR phone)=0;
   //--- email
   virtual LPCWSTR   EMail(void) const=0;
   virtual MTAPIRES  EMail(LPCWSTR email)=0;
   //--- additional ID
   virtual LPCWSTR   ID(void) const=0;
   virtual MTAPIRES  ID(LPCWSTR email)=0;
   //--- additional status
   virtual LPCWSTR   Status(void) const=0;
   virtual MTAPIRES  Status(LPCWSTR id)=0;
   //--- comment
   virtual LPCWSTR   Comment(void) const=0;
   virtual MTAPIRES  Comment(LPCWSTR comment)=0;
   //--- color
   virtual COLORREF  Color(void) const=0;
   virtual MTAPIRES  Color(const COLORREF color)=0;
   //--- phone password
   virtual LPCWSTR   PhonePassword(void) const=0;
   virtual MTAPIRES  PhonePassword(LPCWSTR password)=0;
   //--- leverage
   virtual uint32_t  Leverage(void) const=0;
   virtual MTAPIRES  Leverage(const uint32_t leverage)=0;
   //--- agent account
   virtual uint64_t    Agent(void) const=0;
   virtual MTAPIRES  Agent(const uint64_t agent)=0;
   //--- balance & credit
   virtual double    Balance(void) const=0;
   virtual double    Credit(void) const=0;
   //--- accumulated interest rate
   virtual double    InterestRate(void) const=0;
   //--- accumulated daily and monthly commissions
   virtual double    CommissionDaily(void) const=0;
   virtual double    CommissionMonthly(void) const=0;
   //--- accumulated daily and monthly agent commissions
   virtual double    CommissionAgentDaily(void) const=0;
   virtual double    CommissionAgentMonthly(void) const=0;
   //--- previous balance state
   virtual double    BalancePrevDay(void) const=0;
   virtual double    BalancePrevMonth(void) const=0;
   //--- previous equity state
   virtual double    EquityPrevDay(void) const=0;
   virtual double    EquityPrevMonth(void) const=0;
   //--- user record internal data for API usage
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const int64_t value)=0;
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const uint64_t value)=0;
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const double value)=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,int64_t& value) const=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,uint64_t& value) const=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,double& value) const=0;
   virtual MTAPIRES  ApiDataClear(const uint16_t app_id)=0;
   virtual MTAPIRES  ApiDataClearAll(void)=0;
   //--- external accounts
   virtual MTAPIRES  ExternalAccountAdd(const uint64_t gateway_id,LPCWSTR account)=0;
   virtual MTAPIRES  ExternalAccountUpdate(const uint32_t pos,const uint64_t gateway_id,LPCWSTR account)=0;
   virtual MTAPIRES  ExternalAccountDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ExternalAccountClear(void)=0;
   virtual uint32_t  ExternalAccountTotal(void) const=0;
   virtual MTAPIRES  ExternalAccountNext(const uint32_t pos,uint64_t& gateway_id,MTAPISTR& account) const=0;
   virtual MTAPIRES  ExternalAccountGet(const uint64_t gateway_id,MTAPISTR& account) const=0;
   //--- last password change datetime (filled by MT5)
   virtual int64_t     LastPassChange(void) const =0;
   //--- client's MetaQuotes ID
   virtual LPCWSTR   MQID(MTAPISTR& mqid) const=0;
   //--- user record internal data for API usage
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const int64_t value)=0;
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const uint64_t value)=0;
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const double value)=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,int64_t& value) const=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,uint64_t& value) const=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,double& value) const=0;
   //--- clients passwords derivative hash for password comparison
   virtual MTAPIRES  PasswordHash(const uint32_t type,MTAPISTR& password_hash) const=0;
   //--- lead campaign
   virtual LPCWSTR   LeadCampaign(void) const=0;
   virtual MTAPIRES  LeadCampaign(LPCWSTR lead_campaign)=0;
   //--- lead source
   virtual LPCWSTR   LeadSource(void) const=0;
   virtual MTAPIRES  LeadSource(LPCWSTR lead_source)=0;
   //--- client id
   virtual uint64_t  ClientID(void) const=0;
   virtual MTAPIRES  ClientID(const uint64_t id)=0;
   //--- first name
   virtual LPCWSTR   FirstName(void) const=0;
   virtual MTAPIRES  FirstName(LPCWSTR first_name)=0;
   //--- last name
   virtual LPCWSTR   LastName(void) const=0;
   virtual MTAPIRES  LastName(LPCWSTR last_name)=0;
   //--- middle name
   virtual LPCWSTR   MiddleName(void) const=0;
   virtual MTAPIRES  MiddleName(LPCWSTR middle_name)=0;
   //--- registration datetime (filled by MT5)
   virtual MTAPIRES  RegistrationSet(const int64_t datetime)=0;
   //--- otp secret
   virtual LPCWSTR   OTPSecret(void) const=0;
   virtual MTAPIRES  OTPSecret(LPCWSTR otp_secret)=0;
   //--- open orders limit
   virtual uint32_t  LimitOrders(void) const=0;
   virtual MTAPIRES  LimitOrders(const uint32_t id)=0;
   //--- open positions value limit
   virtual double    LimitPositionsValue(void) const=0;
   virtual MTAPIRES  LimitPositionsValue(const double value)=0;
   //--- visitor id
   virtual uint64_t    VisitorID(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTUser(void) {}
  };
//+------------------------------------------------------------------+
//| User array interface                                             |
//+------------------------------------------------------------------+
class IMTUserArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTUserArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTUser* user)=0;
   virtual MTAPIRES  AddCopy(const IMTUser* user)=0;
   virtual MTAPIRES  Add(IMTUserArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTUserArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTUser*  Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTUser* account)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTUser* account)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTUser*  Next(const uint32_t index) const=0;
   //--- sorting and search
   virtual MTAPIRES  Sort(MTSortFunctionPtr sort_function)=0;
   virtual int32_t   Search(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchGreatOrEq(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchGreater(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchLessOrEq(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchLess(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchLeft(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchRight(const void *key,MTSortFunctionPtr sort_function) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTUserArray(void) {}
  };
//+------------------------------------------------------------------+
//| User records notification interface                              |
//+------------------------------------------------------------------+
class IMTUserSink
  {
public:
   //--- events
   virtual void      OnUserAdd(const IMTUser*    /*user*/)           {  }
   virtual void      OnUserUpdate(const IMTUser* /*user*/)           {  }
   virtual void      OnUserDelete(const IMTUser* /*user*/)           {  }
   virtual void      OnUserClean(const uint64_t /*login*/)             {  }
   virtual void      OnUserLogin(LPCWSTR /*ip*/,const IMTUser* /*user*/,const uint32_t /*type*/) {  }
   virtual void      OnUserSync(void)                                {  }
   //--- hooks
   virtual MTAPIRES  HookUserAdd(IMTUser* /*user*/)                                            { return(MT_RET_OK); }
   virtual MTAPIRES  HookUserUpdate(const IMTUser* /*prev*/,IMTUser* /*user*/)                 { return(MT_RET_OK); }
   virtual MTAPIRES  HookUserDelete(const IMTUser* /*user*/)                                   { return(MT_RET_OK); }
   virtual MTAPIRES  HookUserLogin(LPCWSTR /*ip*/,const IMTUser* /*user*/,const uint32_t /*type*/) { return(MT_RET_OK); }
   //--- events
   virtual void      OnUserLogout(LPCWSTR /*ip*/,const IMTUser* /*user*/,const uint32_t /*type*/)  {                    }
   virtual void      OnUserArchive(const IMTUser*  /*user*/)                                   {                    }
   virtual void      OnUserRestore(const IMTUser*  /*user*/)                                   {                    }
   //--- hooks
   virtual MTAPIRES  HookUserArchive(const IMTUser*  /*user*/)                                 { return(MT_RET_OK); }
   virtual MTAPIRES  HookUserLoginExt(const IMTUser* /*user*/,const IMTOnline* /*online*/)     { return(MT_RET_OK); }
   //--- events                                                                                
   virtual void      OnUserLoginExt(const IMTUser* /*user*/,const IMTOnline* /*online*/)       {  }
   virtual void      OnUserLogoutExt(const IMTUser* /*user*/,const IMTOnline* /*online*/)      {  }
   virtual void      OnUserAddExt(const IMTUser* /*user*/,LPCWSTR /*master_password*/,LPCWSTR /*investor_password*/)   {  }
   virtual void      OnUserChangePassword(const IMTUser* /*user*/,const uint32_t /*password_type*/,LPCWSTR /*password*/)   {  }
   //--- hooks
   virtual MTAPIRES  HookUserAddExt(IMTUser* /*user*/,LPCWSTR /*master_password*/,LPCWSTR /*investor_password*/) { return(MT_RET_OK); }
   virtual MTAPIRES  HookUserChangePassword(const IMTUser* /*user*/,const uint32_t /*password_type*/,LPCWSTR /*password*/) { return(MT_RET_OK); }
  };
//+------------------------------------------------------------------+
