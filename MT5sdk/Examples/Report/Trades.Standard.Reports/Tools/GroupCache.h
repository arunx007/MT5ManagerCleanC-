//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Currency converter class with cache                              |
//+------------------------------------------------------------------+
class CGroupCache
  {
public:
   //--- static string types
   typedef wchar_t   (GroupName)[64];     // group name type
   typedef wchar_t   (Currency)[32];      // currency type

private:
   //--- group record
   struct GroupRecord
     {
      GroupName         name;             // group name
      Currency          currency;         // currency
      uint32_t          currency_digit;   // currency digits
     };
   //--- types
   typedef TMTArray<GroupRecord,16> GroupArray;      // group record array type
   typedef TMTArray<const GroupRecord*,16> GroupPtrArray;  // group record pointer array type

private:
   IMTReportAPI     &m_api;               // report api
   IMTConGroup      *m_group;             // group config interface
   GroupArray        m_groups;            // group record array
   GroupPtrArray     m_index;             // group index array
   const GroupRecord *m_current;          // current group record

public:
                     CGroupCache(IMTReportAPI &api);
                    ~CGroupCache(void);
   //--- check for same group
   bool              SameGroup(const GroupName &group) const { return(m_current && !CMTStr::Compare(m_current->name,group)); }
   //--- get group currency
   MTAPIRES          GroupCurrency(const GroupName &group,Currency &currency,uint32_t &currency_digit);

private:
   //--- select group
   MTAPIRES          SelectGroup(const GroupName &group);
   //--- add group
   MTAPIRES          GroupAdd(const GroupName &group);
   //--- rebuild index
   MTAPIRES          RebuldIndex(void);
   //--- sort functions
   static int32_t    SearchGroup(const void *left,const void *right);
   static int32_t    SortGroup(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
