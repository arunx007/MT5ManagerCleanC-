//+------------------------------------------------------------------+
//|                                           MetaTrader 5 UniFeeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
namespace UniFeeder.NET
  {
   using MetaQuotes.MT5CommonAPI;
   using MetaQuotes.MT5GatewayAPI;
   using System;
   using System.Collections.Generic;
   using System.Threading;
   //+------------------------------------------------------------------+
   //| Class of external connection data processing                     |
   //+------------------------------------------------------------------+
   class CDataSource: IDisposable
     {
      //--- state of the external connection
      public enum EnState
        {
         STATE_DISCONNECTED=0x00,        // there is no connection
         STATE_CONNECTSTART=0x01,        // starting connection
         STATE_CONNECTED   =0x02,        // connection has been established
        };
      //--- constants
      const int THREAD_TIMEOUT   =60;           // timeout of thread completion, s
      const int READ_BUFFER_MAX  =1024*1024;    // maximal buffer size
      const int READ_BUFFER_STEP =16*1024;      // step of the buffer reallocation
      const int BUFFER_SIZE      =128*1024;     // size of the parsing buffer
      const int LOGIN_COUNT_MAX  =3;            // maximal number of authorization attempts
      //--- pointer to the gateway interface
      CIMTGatewayAPI    m_gateway;
      //--- data processing thread
      Thread            m_thread;
      //--- sign of thread operation
      int               m_workflag;
      //--- sign of initialization (receipt of connection settings)
      int               m_initalized;
      //--- state of the external connection
      int               m_state;
      //--- access to Universal DDE Connector
      CUniFeederSocket  m_unisocket;      // feeder socket
      string            m_address;        // server address
      string            m_login;          // login
      string            m_password;       // password
      string            m_symbols;        // symbols
      //--- sync for socket
      Object            m_socketsync;
      //+------------------------------------------------------------------+
      //| Constructor                                                      |
      //+------------------------------------------------------------------+
      public CDataSource()
        {
         m_gateway   =null;
         m_workflag  =0;
         m_initalized=0;
         m_state     =(int)EnState.STATE_DISCONNECTED;
         m_unisocket =null;
         m_symbols   =null;
         //--- null the strings
         m_address   =string.Empty;
         m_login     =string.Empty;
         m_password  =string.Empty;
         //---
         m_socketsync=new Object();
        }
      //+------------------------------------------------------------------+
      //| Destructor                                                       |
      //+------------------------------------------------------------------+
      public void Dispose()
        {
         //--- close all
         Shutdown();
        }
      //+------------------------------------------------------------------+
      //| Thread of the external connection handling                       |
      //+------------------------------------------------------------------+
      public bool Start(CIMTGatewayAPI gateway)
        {
         //--- checking
         if(gateway==null)
           {
            ExtLogger.OutString(EnMTLogCode.MTLogErr,"invalid UniFeeder parameters");
            return(false);
           }
         //--- remember the pointer to the gateway interface
         m_gateway=gateway;
         //--- set the thread operation flag
         Interlocked.Exchange(ref m_workflag,1);
         try
           {
            //--- start the thread of external connection data processing
            m_thread=new Thread(ProcessThread,CUniFeeder.STACK_SIZE_COMMON);
            m_thread.Start();
           }
         catch(Exception ex)
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr,"failed to start remote server connection thread [{0}]",ex.Message);
            m_gateway=null;
            return(false);
           }
         //--- everything is ok
         return(true);
        }
      //+------------------------------------------------------------------+
      //| Stop the handling thread                                         |
      //+------------------------------------------------------------------+
      public bool Shutdown()
        {
         //--- close the socket
         lock(m_socketsync)
           {
            if(m_unisocket!=null)
               m_unisocket.Close();
           }
         //--- reset the initialization sign
         Interlocked.Exchange(ref m_initalized,0);
         //--- set the thread operation completion sign
         Interlocked.Exchange(ref m_workflag,0);
         //--- complete the operation of the quote feed thread
         if(m_thread!=null)
           {
            //m_thread.
            m_thread.Join(THREAD_TIMEOUT * 1000);
           }
         //--- delete CUniFeederSocket object if it exist
         if(m_unisocket!=null)
           {
            //--- thread already stopped, no lock m_socketsync required
            m_unisocket.Dispose();
            m_unisocket=null;
           }
         //--- reset the pointer to the gateway interface
         m_gateway=null;
         //--- everything is ok
         return(true);
        }
      //+------------------------------------------------------------------+
      //| Initialization of the quote feed                                 |
      //+------------------------------------------------------------------+
      public bool Init(CIMTConFeeder config)
        {
         //--- checking
         if(config==null)
           {
            ExtLogger.OutString(EnMTLogCode.MTLogErr,"invalid UniFeeder config interface");
            return(false);
           }
         try
           {
            //--- check, if already initialized, quit
            if(Interlocked.Add(ref m_initalized,0)!=0)
               return(true);
            //--- remember settings
            m_address =config.FeedServer();
            m_login   =config.FeedLogin();
            m_password=config.FeedPassword();
            //--- calculate symbol buffer size
            for(uint i=0; i<config.SymbolTotal(); i++)
              {
               m_symbols+=config.SymbolNext(i);
               m_symbols+=";";
              }
            //--- record to journal
            ExtLogger.OutString(EnMTLogCode.MTLogOK,"UniFeeder initialized");
            //--- refresh the state
            StateSet(EnState.STATE_CONNECTSTART);
            //--- set the feeder initialization sign
            Interlocked.Exchange(ref m_initalized,1);
            //--- everything is ok
            return(true);
           }
         catch(Exception ex)
           {
            //--- do not let exceptions get out here
            ExtLogger.Out(EnMTLogCode.MTLogErr,"Error initializing UniFeeder - {0}",ex.Message);
           }
         return(false);
        }
      //+------------------------------------------------------------------+
      //| Checking timeouts                                                |
      //+------------------------------------------------------------------+
      public bool Check()
        {
         bool ret=true;
         //---
         if(Interlocked.Add(ref m_initalized,0)==0)
            return(true);
         //--- check the state
         if(StateGet()==EnState.STATE_DISCONNECTED)
            return(false);
         //--- check the socket
         lock(m_socketsync)
           {
            if(m_unisocket!=null)
               ret=m_unisocket.Check();
           }
         return(ret);
        }
      //+------------------------------------------------------------------+
      //| Getting state                                                    |
      //+------------------------------------------------------------------+
      EnState StateGet()
        {
         return((EnState)Interlocked.Add(ref m_state,0));
        }
      //+------------------------------------------------------------------+
      //| Setting state                                                    |
      //+------------------------------------------------------------------+
      EnState StateSet(EnState state)
        {
         //--- notify Gateway API about connect state changing
         if(m_gateway!=null)
            m_gateway.StateConnect(state==EnState.STATE_CONNECTED);
         //--- update state
         return((EnState)Interlocked.Exchange(ref m_state,(int)state));
        }
      //+------------------------------------------------------------------+
      //| Count traffic                                                    |
      //+------------------------------------------------------------------+
      public void StateTraffic(uint received_bytes,uint sent_bytes)
        {
         //--- count traffic using Gateway API
         if(m_gateway!=null)
            m_gateway.StateTraffic(received_bytes,sent_bytes);
        }
      //+------------------------------------------------------------------+
      //| Processing data of the external connection                       |
      //+------------------------------------------------------------------+
      void ProcessThread()
        {
         //--- external connection management loop
         while(Interlocked.Add(ref m_workflag,0)>0)
           {
            //--- analyze the state, start connection, connect
            if(StateGet()==EnState.STATE_CONNECTSTART)
               ProcessConnect();
            //--- connected, receive data
            if(StateGet()==EnState.STATE_CONNECTED)
               ProcessData();
            //--- sleep
            Thread.Sleep(100);
           }
        }
      //+------------------------------------------------------------------+
      //| Connection                                                       |
      //+------------------------------------------------------------------+
      bool ProcessConnect()
        {
         bool res=false;
         lock(m_socketsync)
           {
            //--- create CUniFeederSocket object if it does not exist
            if(m_unisocket==null)
              {
               try
                 {
                  m_unisocket=new CUniFeederSocket(this);
                 }
               catch(Exception ex)
                 { 
                  ExtLogger.Out(EnMTLogCode.MTLogOK,"UniFeeder creating socket failed [{0}]",ex.Message);
                  return(false);
                 }
              }
            //--- connect to the UniDDE server
            res=m_unisocket.Connect(m_address,m_login,m_password,m_symbols);
            //--- write the result to the journal and update the state
            if(res)
              {
               ExtLogger.Out(EnMTLogCode.MTLogOK,"UniFeeder connected to '{0}' with login '{1}'",m_address,m_login);
               StateSet(EnState.STATE_CONNECTED);
              }
            else
              {
               ExtLogger.OutString(EnMTLogCode.MTLogOK,"UniFeeder connect failed");
               StateSet(EnState.STATE_DISCONNECTED);
              }
           }
         //--- return the result
         return(true);
        }
      //+------------------------------------------------------------------+
      //| Receiving quotes                                                 |
      //+------------------------------------------------------------------+
      bool ProcessData()
        {
         MTRetCode res=MTRetCode.MT_RET_OK;
         try
           {
            List<MTTick> ticks_buffer=new List<MTTick>();
            //---
            lock(m_socketsync)
              {
               //--- receive ticks from data source
               if(m_unisocket!=null && !m_unisocket.ReadTicks(ref ticks_buffer))
                  res=MTRetCode.MT_RET_ERROR;
              }
            //--- send ticks to MT5
            if(m_gateway!=null && res!=MTRetCode.MT_RET_ERROR)
              {
               for(int i=0; i<ticks_buffer.Count; i++)
                 {
                  res=m_gateway.SendTick(ticks_buffer[i]);
                  //---
                  if(res!=MTRetCode.MT_RET_OK)
                     ExtLogger.Out(EnMTLogCode.MTLogErr,"failed to send ticks, symbol {0}, error [{1}]",ticks_buffer[i].symbol,res);
                 }
              }
           }
         catch(Exception ex)
           {
            res=MTRetCode.MT_RET_ERROR;
            ExtLogger.Out(EnMTLogCode.MTLogErr,"failed to send ticks - {0}",ex.Message);
           }
         //--- if error, refresh the state
         if(res!=MTRetCode.MT_RET_OK)
            StateSet(EnState.STATE_DISCONNECTED);
         //--- return the result
         return(res==MTRetCode.MT_RET_OK);
        }
     }
  }
//+------------------------------------------------------------------+
