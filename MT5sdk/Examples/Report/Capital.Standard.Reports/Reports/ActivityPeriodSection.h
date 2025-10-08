//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\ReportColumn.h"
#include "..\Tools\ReportVector.h"
#include "..\Tools\ReportInterval.h"
//+------------------------------------------------------------------+
//| Activity by section Report part class                            |
//+------------------------------------------------------------------+
class CActivityPeriodSection
  {
private:
   #pragma pack(push,1)
   //--- Inactive client count dataset record
   struct ActivityRecord
     {
      int64_t           date;                      // date
      uint32_t          count;                     // count
     };
   #pragma pack(pop)
   //--- Activity container types
   typedef TReportVector<uint32_t> UIntVector;         // UInt vector type

private:
   const CReportInterval &m_interval;              // report interval reference
   UIntVector        m_inactive;                   // inactive clients
   //--- static data
   static ReportColumn s_columns_inactive[];       // column description inactive clients

public:
   //--- constructor/destructor
   explicit          CActivityPeriodSection(const CReportInterval &interval);
                    ~CActivityPeriodSection(void);
   //--- clear
   void              Clear(void);
   //--- Initialize
   MTAPIRES          Initialize(void);
   //--- add inactive client
   MTAPIRES          InactiveAdd(const int64_t time);
   //--- prepare all graphs to be shown
   MTAPIRES          PrepareGraphs(IMTReportAPI &api,LPCWSTR title,const uint32_t y);

private:
   //--- prepare inactive clients data
   MTAPIRES          PrepareDataInactive(IMTDataset &data) const;
   //--- prepare graph
   MTAPIRES          PrepareGraph(IMTReportAPI &api,IMTDataset *data,LPCWSTR title,const uint32_t y);
  };
//+------------------------------------------------------------------+
