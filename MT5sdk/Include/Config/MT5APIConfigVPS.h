//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| VPS condition configuration                                      |
//+------------------------------------------------------------------+
class IMTConVPSCondition
  {
public:
   //--- condition types
   enum EnCondition
     {
      //--- datetime conditions
      CONDITION_DATETIME_DATETIME             =0,
      CONDITION_DATETIME_DATE                 =1,
      CONDITION_DATETIME_TIME                 =2,
      //--- datetime conditions                 
      CONDITION_DATETIME_FIRST                =0,
      CONDITION_DATETIME_LAST                 =1000,
      //--- trade account conditions
      CONDITION_ACCOUNT_LOGIN                 =1001,
      CONDITION_ACCOUNT_GROUP                 =1002,
      CONDITION_ACCOUNT_COUNTRY               =1003,
      CONDITION_ACCOUNT_CITY                  =1004,
      CONDITION_ACCOUNT_LANGUAGE              =1005,
      CONDITION_ACCOUNT_PHONE                 =1006,
      CONDITION_ACCOUNT_EMAIL                 =1007,
      CONDITION_ACCOUNT_COLOR                 =1008,
      CONDITION_ACCOUNT_COMMENT               =1009,
      CONDITION_ACCOUNT_REGISTRATION          =1010,
      CONDITION_ACCOUNT_LASTTIME              =1011,
      CONDITION_ACCOUNT_LEVERAGE              =1012,
      CONDITION_ACCOUNT_BALANCE               =1013,
      CONDITION_ACCOUNT_CREDIT                =1014,
      CONDITION_ACCOUNT_POSITIONS             =1015,
      CONDITION_ACCOUNT_ORDERS                =1016,
      CONDITION_ACCOUNT_PROFIT                =1017,
      CONDITION_ACCOUNT_EQUITY                =1018,
      CONDITION_ACCOUNT_MARGIN                =1019,
      CONDITION_ACCOUNT_MARGIN_FREE           =1020,
      CONDITION_ACCOUNT_MARGIN_LEVEL          =1021,
      CONDITION_ACCOUNT_CURRENCY              =1022,
      CONDITION_ACCOUNT_REGISTRATION_ELAPSED  =1023,
      CONDITION_ACCOUNT_LASTTIME_ELAPSED      =1024,
      CONDITION_ACCOUNT_LEAD_SOURCE           =1025,
      CONDITION_ACCOUNT_LEAD_CAMPAIGN         =1026,
      CONDITION_ACCOUNT_ONLINE                =1027,
      CONDITION_ACCOUNT_ACTIVITY_TRADE_ELAPSED=1028,
      CONDITION_ACCOUNT_ENABLED               =1029,
      CONDITION_ACCOUNT_TRADING_ENABLED       =1030,
      CONDITION_ACCOUNT_EXPERT_ENABLED        =1031,
      CONDITION_ACCOUNT_OWN_FUNDS_PERCENT     =1032,
      CONDITION_ACCOUNT_OWN_FUNDS_VOLUME      =1033,
      CONDITION_ACCOUNT_STATUS                =1034,
      CONDITION_ACCOUNT_CLIENT_ID             =1035,
      //--- trade account conditions
      CONDITION_ACCOUNT_FIRST                 =1001,
      CONDITION_ACCOUNT_LAST                  =2000,
      //--- enumeration borders
      CONDITION_FIRST                         =CONDITION_DATETIME_FIRST,
      CONDITION_LAST                          =CONDITION_ACCOUNT_LAST
     };
   //--- condition rules
   enum EnConditionRule
     {
      RULE_EQ                      =0,       // ==
      RULE_NOT_EQ                  =1,       // !=
      RULE_GREATER                 =2,       // >
      RULE_NOT_LESS                =3,       // >=
      RULE_LESS                    =4,       // <
      RULE_NOT_GREATER             =5,       // <=
      RULE_MATCH_MASK              =6,       // mask
      //--- enumeration borders
      RULE_FIRST                   =RULE_EQ,
      RULE_LAST                    =RULE_MATCH_MASK
     };
   //--- condition rules types
   enum EnConditionType
     {
      TYPE_NONE                    =0,
      TYPE_STRING                  =1,
      TYPE_INT                     =2,
      TYPE_UINT                    =3,
      TYPE_COLOR                   =4,
      TYPE_MONEY                   =5,
      TYPE_DATETIME                =6,
      TYPE_LEVERAGE                =7,
      TYPE_BOOL                    =8,
      TYPE_TIME                    =9,
      TYPE_DATE                    =10,
      TYPE_PERCENT                 =11,
      TYPE_LANGUAGE                =12,
      TYPE_LOGIN                   =13,
      //--- enumeration borders
      TYPE_FIRST                   =TYPE_NONE,
      TYPE_LAST                    =TYPE_LOGIN
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConVPSCondition* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- condition
   virtual uint32_t  Condition(void) const=0;
   virtual MTAPIRES  Condition(const uint32_t condition)=0;
   //--- condition rule
   virtual uint32_t  Rule(void) const=0;
   virtual MTAPIRES  Rule(const uint32_t rule)=0;
   //--- type
   virtual uint32_t  ValueType(void) const=0;
   //--- value int
   virtual int64_t     ValueInt(void) const=0;
   virtual MTAPIRES  ValueInt(const int64_t value)=0;
   //--- value uint
   virtual uint64_t    ValueUInt(void) const=0;
   virtual MTAPIRES  ValueUInt(const uint64_t value)=0;
   //--- value double
   virtual double    ValueDouble(void) const=0;
   virtual MTAPIRES  ValueDouble(const double value)=0;
   //--- value string
   virtual LPCWSTR   ValueString(void) const=0;
   virtual MTAPIRES  ValueString(LPCWSTR value)=0;
   //--- value color
   virtual COLORREF  ValueColor(void) const=0;
   virtual MTAPIRES  ValueColor(const COLORREF value)=0;
   //--- value money
   virtual double    ValueMoney(void) const=0;
   virtual MTAPIRES  ValueMoney(const double value)=0;
   //--- value datetime
   virtual int64_t     ValueDatetime(void) const=0;
   virtual MTAPIRES  ValueDatetime(const int64_t value)=0;
   //--- value leverage
   virtual int64_t     ValueLeverage(void) const=0;
   virtual MTAPIRES  ValueLeverage(const int64_t value)=0;
   //--- value bool
   virtual bool      ValueBool(void) const=0;
   virtual MTAPIRES  ValueBool(const bool value)=0;
   //--- value time
   virtual uint32_t  ValueTime(void) const=0;
   virtual MTAPIRES  ValueTime(const uint32_t value)=0;
   //--- value date
   virtual int64_t     ValueDate(void) const=0;
   virtual MTAPIRES  ValueDate(const int64_t value)=0;
   //--- value percent
   virtual uint32_t  ValuePercent(void) const=0;
   virtual MTAPIRES  ValuePercent(const uint32_t value)=0;
   //--- value language
   virtual uint32_t  ValueLanguage(void) const=0;
   virtual MTAPIRES  ValueLanguage(const uint32_t value)=0;
  };
//+------------------------------------------------------------------+
//| VPS rule configuration                                           |
//+------------------------------------------------------------------+
class IMTConVPSRule
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConVPSRule* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- enabled
   virtual bool      Enabled(void) const=0;
   virtual MTAPIRES  Enabled(const bool enabled)=0;
   //--- rule name
   virtual LPCWSTR   Name(void) const=0;
   virtual MTAPIRES  Name(LPCWSTR name)=0;
   //--- conditions
   virtual MTAPIRES  ConditionAdd(IMTConVPSCondition* cfg)=0;
   virtual MTAPIRES  ConditionUpdate(const uint32_t pos,const IMTConVPSCondition* cfg)=0;
   virtual MTAPIRES  ConditionDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ConditionClear(void)=0;
   virtual MTAPIRES  ConditionShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  ConditionTotal(void) const=0;
   virtual MTAPIRES  ConditionNext(const uint32_t pos,IMTConVPSCondition* cfg) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConVPSRule(void) {}
  };
//+------------------------------------------------------------------+
//| VPS group configuration                                          |
//+------------------------------------------------------------------+
class IMTConVPSGroup
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConVPSGroup* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- group list allowed
   virtual LPCWSTR   Group(void) const=0;
   virtual MTAPIRES  Group(LPCWSTR group)=0;
   //--- minimal balance for sponsored VPS
   virtual double    MinBalance(void) const=0;
   virtual MTAPIRES  MinBalance(const double balance)=0;
   //--- inactivity days to switch off sponsored VPS
   virtual uint32_t  InactiveDays(void) const=0;
   virtual MTAPIRES  InactiveDays(const uint32_t days)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConVPSGroup(void) {}
  };
//+------------------------------------------------------------------+
//| VPS configuration                                                |
//+------------------------------------------------------------------+
class IMTConVPS
  {
public:
   //--- flags
   enum EnFlags
     {
      VPS_NONE          =0x00000000,         // none
      VPS_SPONSOR_ACTIVE=0x00000001,         // activate VPS
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConVPS* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- flags
   virtual uint64_t    Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint64_t flags)=0;
   //--- MQL5 login
   virtual LPCWSTR   MQL5Login(void) const=0;
   virtual MTAPIRES  MQL5Login(LPCWSTR login)=0;
   //--- MQL5 password
   virtual LPCWSTR   MQL5Password(void) const=0;
   virtual MTAPIRES  MQL5Password(LPCWSTR password)=0;
   //--- groups settings (obsolete)
   virtual MTAPIRES  GroupAdd(IMTConVPSGroup* group)=0;
   virtual MTAPIRES  GroupUpdate(const uint32_t pos,const IMTConVPSGroup* group)=0;
   virtual MTAPIRES  GroupDelete(const uint32_t pos)=0;
   virtual MTAPIRES  GroupClear(void)=0;
   virtual MTAPIRES  GroupShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  GroupTotal(void) const=0;
   virtual MTAPIRES  GroupNext(const uint32_t pos,IMTConVPSGroup* group) const=0;
   //--- rules
   virtual MTAPIRES  RuleAdd(IMTConVPSRule* rule)=0;
   virtual MTAPIRES  RuleUpdate(const uint32_t pos,const IMTConVPSRule* rule)=0;
   virtual MTAPIRES  RuleDelete(const uint32_t pos)=0;
   virtual MTAPIRES  RuleClear(void)=0;
   virtual MTAPIRES  RuleShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  RuleTotal(void) const=0;
   virtual MTAPIRES  RuleNext(const uint32_t pos,IMTConVPSRule* rule) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConVPS(void) {}
  };
//+------------------------------------------------------------------+
//| Time config events notification interface                        |
//+------------------------------------------------------------------+
class IMTConVPSSink
  {
public:
   virtual void      OnVPSUpdate(const IMTConVPS* /*config*/) {  }
   virtual void      OnVPSSync(void)                          {  }
  };
//+------------------------------------------------------------------+
