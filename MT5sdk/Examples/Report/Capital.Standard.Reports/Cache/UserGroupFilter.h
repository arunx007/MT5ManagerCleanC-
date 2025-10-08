//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "DealWeekCache.h"
//+------------------------------------------------------------------+
//| Parameters                                                       |
//+------------------------------------------------------------------+
#define PARAMETER_GROUP_FILTER            L"Groups filter"
#ifdef _DEBUG
#define DEFAULT_GROUP_FILTER              L"*"
#else
#define DEFAULT_GROUP_FILTER              L"*,!demo*,!contest*"
#endif
//+------------------------------------------------------------------+
//| User filter class                                                |
//+------------------------------------------------------------------+
class CUserGroupFilter
  {
private:
   MTByteArray       m_groups;                     // groups filter flags by positions in dictionary

public:
   //--- initialization
   MTAPIRES          Initialize(IMTReportAPI &api,CReportCache &cache,uint32_t dictionary_id);
   MTAPIRES          Initialize(IMTReportAPI &api,CDealWeekCache &cache) { return(Initialize(api,cache,CDealWeekCache::DEAL_KEY_FIELD_GROUP)); }
   //--- match group with filter
   bool              GroupMatch(const uint32_t group) const { return(group<m_groups.Total() && m_groups[group]); }
  };
//+------------------------------------------------------------------+
