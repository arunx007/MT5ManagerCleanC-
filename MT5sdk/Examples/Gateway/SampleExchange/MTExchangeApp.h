//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Class of application                                             |
//+------------------------------------------------------------------+
class CMTExchangeApp
  {

private:
   //--- trade dispatcher
   CTradeDispatcher  m_trade_dispatcher;
   //--- gateway connection context
   CExchangeContext  m_context;
   //--- gateway connection address
   wchar_t           m_address[MAX_PATH];

public:
   //--- constructor/destructor
                     CMTExchangeApp();
                    ~CMTExchangeApp(void);
   //--- initialize, run, shutdown, restart
   bool              Initialize(int32_t argc,wchar_t** argv);
   void              Run(void);
   void              Shutdown(void);
   bool              Restart(void);

private:
   //--- process command line arguments
   bool              ProcessCommandLine(int32_t argc,wchar_t** argv);
  };
//+------------------------------------------------------------------+
