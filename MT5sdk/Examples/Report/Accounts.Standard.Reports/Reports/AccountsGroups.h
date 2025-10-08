//+------------------------------------------------------------------+
//|                           MetaTrader 5 Accounts.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\ReportBase.h"
#include "..\Tools\UserSelect.h"
//+------------------------------------------------------------------+
//| Accounts Groups Report                                           |
//+------------------------------------------------------------------+
class CAccountsGroups : public CReportBase
  {
private:
   #pragma pack(push,1)
   //--- user record
   struct UserRecord
     {
      uint64_t          login;            // login
      double            balance;          // balance
      double            credit;           // credit
      int64_t           last_access;      // last access time
     };
   //--- account record
   struct AccountRecord
     {
      uint64_t          login;            // login
      double            balance;          // balance
      double            equity;           // equity
      double            profit;           // profit
     };
   #pragma pack(pop)
   //--- group info
   struct GroupRecord
     {
      wchar_t           group[64];        // group name
      double            balance;          // balance
      double            equity;           // equity
      double            profit;           // profit
      wchar_t           currency[64];     // currency
      uint32_t          accounts;         // accounts
      uint32_t          active;           // active accounts for the last 3 months
      uint32_t          currency_digits;  // currency digits
     };
   //---
   typedef TMTArray<GroupRecord> GroupRecordArray;

private:
   int64_t           m_checktime;                  // activity check time
   IMTConGroup      *m_group;                      // group interface
   CUserSelect       m_user_select;                // user select object
   CAccountSelect    m_account_select;             // account select object
   //---
   GroupRecordArray  m_groups;                     // groups (current group type)
   GroupRecordArray  m_groups_totals;              // totals for groups by currecny(current group type)
   //---
   static const MTReportInfo s_info;               // report info
   static const DatasetField s_user_fields[];      // user request fields descriptions
   static const DatasetField s_account_fields[];   // account request fields descriptions
   static const uint32_t s_request_limit=1000000;      // request limit

public:
   //--- constructor/destructor
                     CAccountsGroups(void);
   virtual          ~CAccountsGroups(void);
   //--- report information
   static void       Info(MTReportInfo& info);

private:
   //--- base overrides
   virtual void      Clear(void) override;
   virtual MTAPIRES  Prepare(void) override;
   virtual MTAPIRES  Write(uint32_t) override;
   //--- prepare information for group type
   MTAPIRES          PrepareGroupType(const uint32_t type);
   //--- prepare information for group
   MTAPIRES          PrepareGroup(GroupRecord &group);
   //--- prepare information for group part
   MTAPIRES          PrepareGroupPart(GroupRecord &group,IMTDataset &users,IMTDataset *accounts,uint64_t &login_last);
   //--- add group to groups and update totals
   MTAPIRES          PrepareGroupAdd(const GroupRecord& group);
   //--- sort functions
   static int32_t    SortGroupByCurrency(const void *left,const void *right);
   static int32_t    SortGroupByName(const void *left,const void *right);
   static int32_t    SortGroupByAccounts(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
