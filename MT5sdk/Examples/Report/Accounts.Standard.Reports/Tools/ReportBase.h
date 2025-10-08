//+------------------------------------------------------------------+
//|                           MetaTrader 5 Accounts.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportLog.h"
//+------------------------------------------------------------------+
//| Report base class                                                |
//+------------------------------------------------------------------+
class CReportBase : public IMTReportContext
  {
protected:
   //--- group types
   enum group_types
     {
      GROUP_REAL       =0,                // real
      GROUP_PRELIMINARY=1,                // preliminary
      GROUP_COVERAGE   =2,                // coverage
      GROUP_DEMO       =3,                // demo
      GROUP_CONTEST    =4,                // contest
      GROUP_MANAGER    =5                 // manager
     };

protected:
   const uint32_t    m_types;                         // report types mask
   IMTReportAPI     *m_api;                           // report api
   CReportLog        m_log;                           // logger

public:
                     CReportBase(const MTReportInfo &info);
   virtual          ~CReportBase(void);
   //--- release
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);

protected:
   //--- clear overridable
   virtual void      Clear(void)=0;
   //--- report prepare overridable
   virtual MTAPIRES  Prepare(void)=0;
   //--- report write overridable
   virtual MTAPIRES  Write(uint32_t type)=0;
   //--- get group type by group name
   static uint32_t   GetGroupType(LPCWSTR group);
   //--- get group type name
   static bool       GetGroupTypeName(const uint32_t type,CMTStr& name);

private:
   //--- inner and outer clear
   void              ClearAll(void);
   //--- inner clear
   void              ClearInner(void);
  };
//+------------------------------------------------------------------+
