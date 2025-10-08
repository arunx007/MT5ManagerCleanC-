//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "NFAOrderRecord.h"
//+------------------------------------------------------------------+
//| Record for storing Request history                               |
//+------------------------------------------------------------------+
struct RequestRecord
  {
   uint64_t          request_id;
   CNFAOrderRecord   record;
  };
//+------------------------------------------------------------------+
//| Memory based requests storage                                    |
//+------------------------------------------------------------------+
class CRequestBase
  {
private:
   TMTArray<RequestRecord*> m_requests;

public:
                     CRequestBase(void);
                    ~CRequestBase(void);
   //--- clear/add/remove
   void              Clear(void);
   bool              Add(const uint64_t request_id,const CNFAOrderRecord& record);
   bool              Remove(const uint64_t request_id);
   bool              Remove(const uint64_t request_id,CNFAOrderRecord& record);

private:
   //--- memory
   RequestRecord*    RequestAllocate(void);
   void              RequestFree(RequestRecord *record);
   //--- sort/search
   static int32_t    SortRequests(const void *left,const void *right);
   static int32_t    SearchRequests(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
