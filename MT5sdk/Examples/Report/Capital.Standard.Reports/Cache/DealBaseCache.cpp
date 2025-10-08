//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DealBaseCache.h"
//+------------------------------------------------------------------+
//| write cache range                                                |
//+------------------------------------------------------------------+
MTAPIRES CDealBaseCache::WriteRange(IMTDatasetRequest &request,int64_t from,const int64_t to)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api.DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- update cache month by month
   for(int64_t end=MonthEnd(from,to);from<to;from=end+1,end=MonthEnd(from,to))
     {
      //--- write cache month range
      const MTAPIRES res=WriteRangeLimit(*data,request,from,end);
      if(res!=MT_RET_OK)
         return(res);
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Request deals from base                                          |
//+------------------------------------------------------------------+
MTAPIRES CDealBaseCache::SelectData(IMTDataset &deals,IMTDatasetRequest &request,const int64_t from,const int64_t to,const uint64_t id_from)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- fill request
   MTAPIRES res=DealRequest(request,from,to,id_from);
   if(res!=MT_RET_OK)
      return(res);
//--- request from base
   res=m_api.DealSelect(&request,&deals);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| end of month                                                     |
//+------------------------------------------------------------------+
int64_t CDealBaseCache::MonthEnd(int64_t ctm,const int64_t to)
  {
   ctm=SMTTime::MonthBegin(SMTTime::MonthBegin(ctm)+SECONDS_IN_MONTH+SECONDS_IN_WEEK)-1;
   return(std::min(ctm,to));
  }
//+------------------------------------------------------------------+
//| Calculate upper power of 2                                       |
//+------------------------------------------------------------------+
uint32_t CDealBaseCache::UpperPowerOfTwo(uint32_t v)
  {
   v--;
   v|=v>>1;
   v|=v>>2;
   v|=v>>4;
   v|=v>>8;
   v|=v>>16;
   v++;
   return(v);
  }
//+------------------------------------------------------------------+
