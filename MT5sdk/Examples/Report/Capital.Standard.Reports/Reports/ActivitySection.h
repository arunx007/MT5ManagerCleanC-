//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\ReportColumn.h"
#include "..\Tools\ReportTop.h"
#include "..\Tools\ReportDictionary.h"
#include "..\Cache\ReportCache.h"
//+------------------------------------------------------------------+
//| Activity by section Report part class                            |
//+------------------------------------------------------------------+
class CActivitySection
  {
private:
   #pragma pack(push,1)
   //--- Inactive client count dataset record
   struct InactiveRecord
     {
      wchar_t           section[64];               // section
      uint32_t          count;                     // count
     };
   #pragma pack(pop)
   //--- Uint structure add operation
   class CUIntOperationAdd
     {
   public:
      void              operator()(uint32_t &l,const uint32_t r) const { l+=r; }
     };
   //--- Activity container types
   typedef TReportTop<uint32_t> UIntTop;               // UInt top type
   typedef TReportVector<uint32_t> UIntVector;         // UInt vector type

private:
   const bool        m_geo;                        // geo section flag
   UIntVector        m_inactive;                   // inactive clients
   UIntTop           m_inactive_top;               // inactive clients top section indexes
   CReportDictionary m_inactive_names;             // inactive clients section names
   //--- static data
   static ReportColumn s_columns_inactive[];       // column description inactive clients

public:
   //--- constructor/destructor
   explicit          CActivitySection(const bool geo=false);
                    ~CActivitySection(void);
   //--- clear
   void              Clear(void);
   //--- Initialize
   MTAPIRES          Initialize(void);
   //--- add inactive client
   MTAPIRES          InactiveAdd(const uint32_t section);
   //--- calculate total
   MTAPIRES          CalculateTotal(CReportParameter &params);
   //--- read top sections names
   MTAPIRES          ReadTopNames(const CReportCache &cache,const uint32_t dictionary_id);
   //--- set section name
   MTAPIRES          SetSectionName(const uint32_t section,LPCWSTR name);
   //--- prepare all graphs to be shown
   MTAPIRES          PrepareGraphs(IMTReportAPI &api,LPCWSTR section_type,const uint32_t x,const uint32_t y);

private:
   //--- prepare inactive clients data
   MTAPIRES          PrepareDataInactive(IMTDataset &data) const;
   //--- prepare graph
   MTAPIRES          PrepareGraph(IMTReportAPI &api,IMTDataset *data,LPCWSTR title,const uint32_t x,const uint32_t y);
   //--- sort uint descending
   static int32_t    SortUIntDesc(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
