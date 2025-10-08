//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\RecordDataBase.h"
#include "NFAOrderRecord.h"
//+------------------------------------------------------------------+
//| Record for storing SL/TP change history                          |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct SLTPRecord
  {
   uint64_t          id;               // unique record id
   int64_t           timestamp;        // timestamp
   uint64_t          position_id;      // position id
   uint64_t          sl_id;            // SL counter
   double            sl_price;         // SL price
   uint64_t          tp_id;            // TP counter
   double            tp_price;         // TP price
   uint64_t          volume_ext;       // volume
   int64_t           time_create;      // position time create
   uint32_t          origin;           // order origin
   double            contract_size;    // contract size
   uint64_t          reserved[7];      // reserved
   //--- read/writes
   bool              Read(CMTFile& file);
   bool              Read(CMTFile& file,const uint32_t version,bool &invalid_version);
   bool              Write(CMTFile& file) const;
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Permanent storage for SL/TP records                              |
//+------------------------------------------------------------------+
class CSLTPBase : protected TRecordDataBase<SLTPRecord>
  {
private:
   enum              constants { COMPACT_CHECK_START_TIME=SECONDS_IN_WEEK };

private:
   IMTServerAPI     *m_api;
   TMTArray<SLTPRecord*> m_index_position;

public:
                     CSLTPBase(void);
                    ~CSLTPBase(void);
   //--- 
   bool              Initialize(IMTServerAPI *api,const PluginContext &context);
   void              Shutdown(void);
   void              Compact(void);
   //---
   bool              RecordGet(const uint64_t position,SLTPRecord& record);
   bool              RecordUpdate(SLTPRecord& record,const bool allow_add=true);
   bool              RecordDelete(const uint64_t position);

private:
   bool              IndexRebuild(void);
   //--- logger
   virtual void      Out(const uint32_t code,LPCWSTR msg,...);
   //--- sorting
   static int32_t    SortByPosition(const void *left,const void *right);
   static int32_t    SearchByPosition(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
