//+------------------------------------------------------------------+
//|                                          MetaTrader 5 TextFeeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
namespace TextFeeder.NET
  {
   using MetaQuotes.MT5CommonAPI;
   using MetaQuotes.MT5GatewayAPI;
   using System;
   //+------------------------------------------------------------------+
   //| Journal class                                                    |
   //+------------------------------------------------------------------+
   static class ExtLogger
     {
      static CIMTGatewayAPI m_gateway=null;
      static Object         m_sync   =new Object();
      //+------------------------------------------------------------------+
      //| Set the gateway interface                                        |
      //+------------------------------------------------------------------+
      public static void SetGateway(CIMTGatewayAPI gateway)
        {
         lock(m_sync)
           {
            //--- remember the gateway interface
            m_gateway=gateway;
           }
        }
      //+------------------------------------------------------------------+
      //| Record to logs                                                   |
      //+------------------------------------------------------------------+
      public static void Out(EnMTLogCode code,string format,params object[] args)
        {
         lock(m_sync)
           {
            //--- if there is a gateway interface, record an entry to the journal
            if(m_gateway!=null)
              {
               //--- record to the journal
               m_gateway.LoggerOut(code,format,args);
              }
           }
        }
      //+------------------------------------------------------------------+
      //| Record to logs without formatting                                |
      //+------------------------------------------------------------------+
      public static void OutString(EnMTLogCode code,string message)
        {
         lock(m_sync)
           {
            //--- if there is a gateway interface, record an entry to the journal
            if(m_gateway!=null)
              {
               //--- record to the journal
               m_gateway.LoggerOutString(code,message);
              }
           }
        }
     }
  }
//+------------------------------------------------------------------+
