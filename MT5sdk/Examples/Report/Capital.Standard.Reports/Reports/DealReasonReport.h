//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "DealSectionReport.h"
//+------------------------------------------------------------------+
//| Deal by Reason report                                            |
//+------------------------------------------------------------------+
class CDealReasonReport : public CDealSectionReport
  {
private:
   static MTReportInfo s_info;                        // static report info

public:
                     CDealReasonReport(void);
   virtual          ~CDealReasonReport(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- factory method
   static IMTReportContext* Create(void);

private:
   //--- fill top names
   virtual MTAPIRES  FillTopNames(const uint32_t dictionary_id,const CReportTopBase &top,CReportDictionary &names) override;
   //--- fet reason type as string
   static LPCWSTR    GetReasonTypeName(const uint32_t reason);
  };
//+------------------------------------------------------------------+
