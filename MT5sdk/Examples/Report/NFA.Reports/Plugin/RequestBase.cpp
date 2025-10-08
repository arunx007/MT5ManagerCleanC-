//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "RequestBase.h"
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CRequestBase::CRequestBase(void)
  {
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CRequestBase::~CRequestBase(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear all requests                                               |
//+------------------------------------------------------------------+
void CRequestBase::Clear(void)
  {
   for(uint32_t i=0;i<m_requests.Total();i++)
      RequestFree(m_requests[i]);
   m_requests.Shutdown();
  }
//+------------------------------------------------------------------+
//| Storing request                                                  |
//+------------------------------------------------------------------+
bool CRequestBase::Add(const uint64_t request_id,const CNFAOrderRecord& record)
  {
   RequestRecord *rec=RequestAllocate();
   if(rec)
     {
      rec->request_id=request_id;
      rec->record    =record;
      if(m_requests.Insert(&rec,SortRequests)!=NULL)
         return(true);
      RequestFree(rec);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Remove request                                                   |
//+------------------------------------------------------------------+
bool CRequestBase::Remove(const uint64_t request_id)
  {
   RequestRecord **ptr=m_requests.Search(&request_id,SearchRequests);
   if(ptr)
     {
      RequestRecord *deleted=*ptr;
      m_requests.Delete(ptr);
      RequestFree(deleted);
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Extract request                                                  |
//+------------------------------------------------------------------+
bool CRequestBase::Remove(const uint64_t request_id,CNFAOrderRecord& record)
  {
   RequestRecord **ptr=m_requests.Search(&request_id,SearchRequests);
   if(ptr)
     {
      RequestRecord *deleted=*ptr;
      m_requests.Delete(ptr);
      if(deleted)
        {
         record=deleted->record;
         RequestFree(deleted);
         return(true);
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Allocate request                                                 |
//+------------------------------------------------------------------+
RequestRecord* CRequestBase::RequestAllocate(void)
  {
   RequestRecord* record=new(std::nothrow) RequestRecord;
   return(record);
  }
//+------------------------------------------------------------------+
//| Free request                                                     |
//+------------------------------------------------------------------+
void CRequestBase::RequestFree(RequestRecord *record)
  {
   if(record)
      delete record;
  }
//+------------------------------------------------------------------+
//| Sort requests                                                    |
//+------------------------------------------------------------------+
int32_t CRequestBase::SortRequests(const void *left,const void *right)
  {
   RequestRecord *lft=*(RequestRecord**)left;
   RequestRecord *rgh=*(RequestRecord**)right;
//---
   if(lft->request_id>rgh->request_id) return(1);
   if(lft->request_id<rgh->request_id) return(-1);
   return(0);
  }
//+------------------------------------------------------------------+
//| Search requests                                                  |
//+------------------------------------------------------------------+
int32_t CRequestBase::SearchRequests(const void *left,const void *right)
  {
   uint64_t         lft=*(uint64_t*)left;
   RequestRecord *rgh=*(RequestRecord**)right;
//---
   if(lft>rgh->request_id) return(1);
   if(lft<rgh->request_id) return(-1);
   return(0);
  }
//+------------------------------------------------------------------+
