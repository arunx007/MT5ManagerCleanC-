//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//---
#define UNKNOWN_NAME               L"Unknown"
#define UNKNOWN_CURRENCY           L"???"
#define DEFAULT_DIGITS             2
//+------------------------------------------------------------------+
//| Agents Report                                                    |
//+------------------------------------------------------------------+
class CAgentsDetailed : public IMTReportContext
  {
private:
   //--- summary struct
   struct SummaryRecord
     {
      double            total;
      wchar_t           currency[32];
     };
   //--- summaries array type
   typedef TMTArray<SummaryRecord> SummaryRecordArray;
   //--- user info struct
   struct UserInfo
     {
      uint64_t          login;
      wchar_t           name[256];
      wchar_t           currency[32];
      uint32_t          currency_digits;
     };

private:
   //---
   static MTReportInfo s_info;            // report information     
   //--- 
   IMTReportAPI     *m_api;               // api
   IMTDeal          *m_deal;              // deal
   IMTDealArray     *m_deals;             // deal array
   IMTUser          *m_user;              // user
   IMTConGroup      *m_group;             // group
   uint64_t         *m_logins;            // logins
   uint32_t          m_logins_total;      // logins total
   SummaryRecordArray m_summaries;        // summary

public:
   //--- constructor/destructor
                     CAgentsDetailed(void);
   virtual          ~CAgentsDetailed(void);
   //--- get information about report
   static void       Info(MTReportInfo& info);
   //--- release plug-in
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);

private:
   //--- create/release interfaces
   void              Clear(void);
   bool              CreateInterfaces(void);
   //--- write html
   MTAPIRES          Write(void);
   bool              WriteRows(MTAPISTR& tag,MTAPIRES& retcode);
   bool              WriteSummaries(MTAPISTR& tag,MTAPIRES& retcode);
   bool              WriteRowInstant(MTAPIRES& retcode,const IMTDeal* commission,const uint32_t line,const UserInfo& agent_info,double& subtotal);
   bool              WriteRowDaily(MTAPIRES& retcode,const IMTDeal* commission,const uint32_t line,const UserInfo& agent_info,double& subtotal);
   bool              WriteRowSubtotal(MTAPIRES& retcode,const UserInfo& agent_info,const double subtotal);
   //--- get next commission deal
   IMTDeal*          CommissionNext(uint64_t* logins,const uint32_t logins_total,uint32_t& login_current,uint32_t& deal_current);
   //--- collect summaries
   bool              CollectSummary(LPCWSTR currency,const double value);
   //--- get user details
   LPCWSTR           GetUserNameW(const uint64_t login,CMTStr& name);
   void              GetUserInfo(const uint64_t login,UserInfo& info);
   //--- sort functions
   static int32_t    SortSummary(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
