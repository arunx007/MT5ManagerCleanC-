//+------------------------------------------------------------------+
//|                                         MetaTrader 5 Text Feeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//--- datasource
#include "DataSource\DataSource.h"
//+------------------------------------------------------------------+
//| Server and external connection management class                  |
//+------------------------------------------------------------------+
class CMTDatafeedApp : public IMTGatewaySink
  {
   //--- constants
   enum
     {
      SLEEP_TIMEOUT=500,                  // lag
     };
   //--- API library
   CMTGatewayAPIFactory m_apifactory;
   //--- gateway
   IMTGatewayAPI    *m_gateway;
   //--- datasource
   CDataSource      *m_source;
   //--- sign of operation
   volatile long     m_workflag;

public:
   //--- constructor/destructor
                     CMTDatafeedApp();
                    ~CMTDatafeedApp(void);
   //--- get gateway description
   static void       Info(MTGatewayInfo& info);
   //--- initialization, work, completion of work
   bool              Initialize(int32_t argc,wchar_t** argv);
   bool              Run(void);
   void              Shutdown(void);
   //--- handlers of gateway notifications
   virtual void      OnGatewayConfig(const uint64_t login,const IMTConFeeder* config);
   virtual void      OnGatewayShutdown(const uint64_t login);
  };
//+------------------------------------------------------------------+
