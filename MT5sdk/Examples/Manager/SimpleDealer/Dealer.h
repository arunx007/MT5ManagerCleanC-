//+------------------------------------------------------------------+
//|                                                     SimpleDealer |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
class CDealer : public IMTManagerSink, public IMTRequestSink,
                public IMTOrderSink, public IMTPositionSink,
                public IMTDealSink, public IMTUserSink
  {
   enum              constants
     {
      MT5_CONNECT_TIMEOUT=30000,       // connect timeout in milliseconds
      STACK_SIZE_COMMON  =1024*1024,   // stack size for dealing thread in bytes
     };
private:
   CMTManagerAPIFactory m_factory;     // Manager API factory
   IMTManagerAPI    *m_manager;        // Manager interface
   MTAPISTR          m_server;         // server address
   uint64_t          m_login;          // dealer login
   MTAPISTR          m_password;       // dealer password
   IMTRequest       *m_request;        // request interface
   IMTConfirm       *m_confirm;        // confirmation interface
   volatile bool     m_stop_flag;      // dealing stop flag
   HANDLE            m_thread_dealer;  // dealing thread
   LONG              m_connected;      // connected flag
   HANDLE            m_event_request;  // request notifications event

public:
                     CDealer(void);
                    ~CDealer(void);
   //--- starting and stopping
   bool              Start(LPCWSTR server,uint64_t login,LPCWSTR password);
   void              Stop(void);
   //--- IMTManagerSink implementation
   virtual void      OnDisconnect(void);
   //--- IMTRequestSink implementation
   virtual void      OnRequestAdd(const IMTRequest*)    { NotifyRequest(); }
   virtual void      OnRequestUpdate(const IMTRequest*) { NotifyRequest(); }
   virtual void      OnRequestDelete(const IMTRequest*) { NotifyRequest(); }
   virtual void      OnRequestSync(void)                { NotifyRequest(); }
   //--- IMTOrderSink implementation
   virtual void      OnOrderAdd(const IMTOrder*    order);
   virtual void      OnOrderUpdate(const IMTOrder* order);
   virtual void      OnOrderDelete(const IMTOrder* order);
   //--- IMTPositionSink implementation
   virtual void      OnPositionAdd(const IMTPosition* position);
   virtual void      OnPositionUpdate(const IMTPosition* position);
   virtual void      OnPositionDelete(const IMTPosition* position);
   //--- IMTDealSink implementation
   virtual void      OnDealAdd(const IMTDeal*    deal);
   virtual void      OnDealUpdate(const IMTDeal* deal);
   virtual void      OnDealDelete(const IMTDeal* deal);
   //--- IMTUserSink implementation
   virtual void      OnUserUpdate(const IMTUser* user);
   virtual void      OnUserLogin(LPCWSTR ip,const IMTUser* user,const uint32_t type);
   virtual void      OnUserLogout(LPCWSTR ip,const IMTUser* user,const uint32_t type);

private:
   //--- initialization and shutdown
   bool              Initialize(void);
   void              Shutdown(void);
   //--- dealing thread
   static uint32_t __stdcall DealerWrapper(LPVOID param);
   void              DealerFunc(void);
   void              DealerAnswer(void);
   //--- requests notification
   void              NotifyRequest(void);
  };
//+------------------------------------------------------------------+
