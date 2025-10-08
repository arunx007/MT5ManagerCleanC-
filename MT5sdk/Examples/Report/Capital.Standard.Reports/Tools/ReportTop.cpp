//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ReportTop.h"
//+------------------------------------------------------------------+
//| clear                                                            |
//+------------------------------------------------------------------+
void CReportTopBase::Clear(void)
  {
   m_index.Clear();
   m_index_sorted.Clear();
  }
//+------------------------------------------------------------------+
//| get top count report parameter value                             |
//+------------------------------------------------------------------+
MTAPIRES CReportTopBase::TopCount(CReportParameter &params,uint32_t &top_count,LPCWSTR param_name,LPCWSTR top_count_default)
  {
//--- checks
   if(!param_name || !*param_name || !top_count_default || !*top_count_default)
      return(MT_RET_ERR_PARAMS);
//--- get top count from report parameters
   int64_t value=0;
   const MTAPIRES res=params.ValueInt(value,param_name,top_count_default);
   if(res!=MT_RET_OK)
      return(res);
//--- check top count
   if(value<0 || value>TOP_COUNT_MAX)
      return(MT_RET_ERR_PARAMS);
//--- check for empty
   top_count=(uint32_t)value;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| sort index static method                                         |
//+------------------------------------------------------------------+
int32_t CReportTopBase::SortIndex(const void *left,const void *right)
  {
//--- type conversion
   const uint32_t &l=*(const uint32_t*)left;
   const uint32_t &r=*(const uint32_t*)right;
//--- compare indexes
   if(l<r)
      return(-1);
   if(l>r)
      return(1);
   return(0);
  }
//+------------------------------------------------------------------+
