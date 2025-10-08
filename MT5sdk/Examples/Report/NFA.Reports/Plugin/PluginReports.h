//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "PluginContext.h"
//+------------------------------------------------------------------+
//| Interface for safe communications with plugin instance           |
//+------------------------------------------------------------------+
class CPluginInstance;
//+------------------------------------------------------------------+
//| Class for preparing and sending reports in separated thread      |
//+------------------------------------------------------------------+
class CPluginReports
  {
private:
   enum contants
     {
      THREAD_SLEEP        =500,
      PROCESS_TIMEOUT_SEND=60000,     // timeout for send process (msec)
     };

private:
   CPluginInstance  *m_parent;         // plugin
   IMTServerAPI     *m_api;            // api
   CMTThread         m_thread;         // thread
   bool              m_workflag;       // work flag
   PluginContext     m_context;        // plugin context
   int64_t           m_time_report;    // report time
   int64_t           m_time_sent;      // sent time
   bool              m_compact_process;// compact time
   //--- buffers
   char              m_buffer[4096];
   CMTStr4096        m_buffer_line;

public:
                     CPluginReports(void);
                    ~CPluginReports(void);
   //--- initialize/shutdown
   bool              Initialize(CPluginInstance *parent,IMTServerAPI *api,const PluginContext &context);
   void              Shutdown(void);

private:
   //--- thread
   static uint32_t __stdcall ThreadWrapper(void *param);
   void              Thread(void);
   //--- report writing helpers
   bool              FileReportOpen(CMTFile &file,LPCWSTR path,LPCSTR header);
   bool              FileReportWrite(CMTFile &file,LPCWSTR line);
   //--- report generating
   void              MakeReports(const tm &batch_ttm);
   bool              MakeReportCompress(const tm &batch_ttm,LPCWSTR name,LPCWSTR report_dir);
   bool              MakeReportEmpty(const tm &batch_ttm,LPCWSTR name,LPCSTR header);
   void              MakeReportCustomer(const tm &batch_ttm,CMTStr &batch_str,const int64_t req_from,const int64_t req_to);
   void              MakeReportTick(const tm &batch_ttm,CMTStr &batch_str,const int64_t req_from,const int64_t req_to);
   void              MakeReportAdjustments(const tm &batch_ttm,CMTStr &batch_str,const int64_t req_from,const int64_t req_to);
   //--- report sending
   bool              SendReports(const tm &batch_ttm);
   bool              SendPuttyExtract(LPCWSTR path);
   bool              SendPuttyProcess(LPCWSTR report_dir,LPCWSTR report_name,LPCWSTR file_date,LPCWSTR putty_path);
   //--- report helpers
   LPCWSTR           ReportPath(CMTStr &str,const tm &ttm,LPCWSTR name) const;
   LPCWSTR           ReportDirectory(CMTStr &str,const tm &ttm) const;
   //--- adjustments
   LPCWSTR           AdjustmentNote(const IMTDeal *deal);
   //--- customers
   LPCWSTR           CustomerCountryType(LPCWSTR country) const;
   bool              CustomerCheck(IMTUser *user,IMTAccount *account,IMTPositionArray *positions,IMTOrderArray *orders,IMTDealArray *deals,const int64_t from,const int64_t to,double &equity);
   //--- logger
   void              Out(uint32_t code,LPCWSTR msg,...);
  };
//+------------------------------------------------------------------+
