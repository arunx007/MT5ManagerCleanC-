//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "GroupCache.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CGroupCache::CGroupCache(IMTReportAPI &api) :
   m_api(api),m_group(nullptr),m_current(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CGroupCache::~CGroupCache(void)
  {
//--- release group interface
   if(m_group)
      m_group->Release();
  }
//+------------------------------------------------------------------+
//| get group currency                                               |
//+------------------------------------------------------------------+
MTAPIRES CGroupCache::GroupCurrency(const GroupName &group,Currency &currency,uint32_t &currency_digit)
  {
//--- select group
   MTAPIRES res=SelectGroup(group);
   if(res!=MT_RET_OK)
      return(res);
   if(!m_current)
      return(MT_RET_ERROR);
//--- copy currency
   CMTStr::Copy(currency,m_current->currency);
   currency_digit=m_current->currency_digit;
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| select group                                                     |
//+------------------------------------------------------------------+
MTAPIRES CGroupCache::SelectGroup(const GroupName &group)
  {
//--- find group
   const GroupRecord **ptr=m_index.Search(&group,&CGroupCache::SearchGroup);
//--- add group
   if(!ptr)
      return(GroupAdd(group));
//--- store group
   m_current=*ptr;
   if(!m_current)
      return(MT_RET_ERROR);
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| add group                                                        |
//+------------------------------------------------------------------+
MTAPIRES CGroupCache::GroupAdd(const GroupName &group)
  {
//--- check group interface
   if(!m_group)
     {
      //--- create group interface
      m_group=m_api.GroupCreate();
      if(!m_group)
         return(MT_RET_ERR_MEM);
     }
//--- find group
   MTAPIRES res=m_api.GroupGetLight(group,m_group);
   if(res!=MT_RET_OK && res!=MT_RET_ERR_PERMISSIONS && res!=MT_RET_ERR_NOTFOUND)
      return(res);
//--- append group
   const GroupRecord *first=m_groups.First();
   GroupRecord *record=m_groups.Append();
//--- clear index when records array reallocated
   if(first!=m_groups.First())
      m_index.Clear();
   if(!record)
      return(MT_RET_ERR_MEM);
//--- fill record
   CMTStr::Copy(record->name,group);
   if(res==MT_RET_OK)
     {
      CMTStr::Copy(record->currency,m_group->Currency());
      record->currency_digit=m_group->CurrencyDigits();
     }
   else
     {
      *record->currency=0;
      record->currency_digit=0;
     }
//--- update index
   if(!m_index.Total())
     {
      //--- insert to index
      if(!m_index.Insert(&record,&CGroupCache::SortGroup))
         return(MT_RET_ERR_MEM);
     }
   else
     {
      //--- rebuild index
      if((res=RebuldIndex())!=MT_RET_OK)
         return(res);
     }
//--- update current group
   m_current=record;
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| rebuild index                                                    |
//+------------------------------------------------------------------+
MTAPIRES CGroupCache::RebuldIndex(void)
  {
//--- clear index
   m_index.Clear();
//--- groups total
   const uint32_t total=m_groups.Total();
   if(!total)
      return(MT_RET_OK);
//--- reserve index
   if(!m_index.Reserve(total))
      return(MT_RET_ERR_MEM);
//--- fill index
   for(const GroupRecord *group=m_groups.First();group;group=m_groups.Next(group))
      if(!m_index.Add(&group))
         return(MT_RET_ERR_MEM);
//--- sort index
   m_index.Sort(SortGroup);
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| search group function                                            |
//+------------------------------------------------------------------+
int32_t CGroupCache::SearchGroup(const void *left,const void *right)
  {
   const GroupName   *lft=(const GroupName*)left;
   const GroupRecord *rgh=*(const GroupRecord**)right;
//--- search by name
   return(CMTStr::Compare(*lft,rgh->name));
  }
//+------------------------------------------------------------------+
//| sort group function                                              |
//+------------------------------------------------------------------+
int32_t CGroupCache::SortGroup(const void *left,const void *right)
  {
   const GroupRecord *lft=*(const GroupRecord**)left;
   const GroupRecord *rgh=*(const GroupRecord**)right;
//--- sort by name
   return(CMTStr::Compare(lft->name,rgh->name));
  }
//+------------------------------------------------------------------+
