//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Journal class                                                    |
//+------------------------------------------------------------------+
class CLogger
  {

private:
   //--- constants
   enum constants
     {
      PREBUF_SIZE    =16*1024                     // size of a preliminary buffer
     };
   //--- pointer to the gateway interface
   IMTGatewayAPI    *m_api_gateway;
   //--- message buffer
   wchar_t           m_prebuf[PREBUF_SIZE];
   //--- synchronizer
   CMTSync           m_sync;

public:
   //--- constructor/destructor
                     CLogger();
                    ~CLogger(void);
   //--- basic functional
   void              SetGatewayAPI(IMTGatewayAPI *api_m_gateway);
   void              Out(int32_t code,LPCWSTR msg,...);
   void              OutString(int32_t code,LPCWSTR string);
  };
extern CLogger  ExtLogger;
//+------------------------------------------------------------------+
