//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Server |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Client connection context                                        |
//+------------------------------------------------------------------+
class CUniNewsContext : public CUniNewsContextBase
  {
private:
   //--- constants
   enum EnConstants
     {
      THREAD_TIMEOUT       =20,     // thread timeout, s
      THREAD_SLEEP         =10,     // thread sleep time
      RECEIVE_BUFFER_SIZE  =64*1024 // receive buffer size, bytes
     };

private:
   //--- news source reference
   CUniNewsSource   &m_data_source;
   //--- socket
   __declspec(align(4)) SOCKET m_socket;
   volatile int64_t  m_close_time;
   static volatile long m_connect_id;
   CMTStr64          m_ip;
   //--- thread
   CMTThread         m_thread;
   volatile bool     m_workflag;
   //--- pointer to next context in list
   CUniNewsContext  *m_next;
   //--- protocol
   CUniNewsProtocol  m_protocol;
   //--- last data send/receive time
   volatile int64_t  m_receive_checktime;
   volatile int64_t  m_send_checktime;
   //--- buffers
   CMTMemPack        m_buffer_send;
   CMTMemPack        m_buffer_receive;
   CMTMemPack        m_buffer_tmp;
   CMTSync           m_buffer_sync;
   //--- login and password
   wchar_t           m_login[64];
   wchar_t           m_password[64];
   //--- subscription parameters
   int64_t           m_subscribe_from;
                     TMTArray<wchar_t[64]>m_subscribe_keywords;
   uint32_t          m_subscribe_languages[32];
   volatile bool     m_subscribed;

public:
                     CUniNewsContext(SOCKET sock,const uint32_t ip,CUniNewsSource &data_source);
   virtual          ~CUniNewsContext(void);
   //--- initialization/shutdown
   bool              Initialize(LPCWSTR login,LPCWSTR password);
   void              Shutdown(void);
   //--- update activity time
   void              UpdateTime(const int64_t timeout)     { InterlockedExchange64(&m_close_time,_time64(nullptr)+timeout); }
   //--- check context
   bool              Check(const int64_t curtime);
   //--- list
   CUniNewsContext*  Next()                              { return(m_next); }
   void              Next(CUniNewsContext* next)         { m_next=next;    }
   //--- connection state
   bool              Connected(void) const               { return(m_socket!=INVALID_SOCKET); }
   //--- close socket
   void              CloseConnection(void);
   //--- send data
   virtual bool      MessageSend(CUniNewsMsg *msg);
   //--- process login message
   virtual bool      OnMessageLogin(CUniNewsMsgLogin *msg);
   //--- process logout message
   virtual bool      OnMessageLogout(CUniNewsMsgLogout *msg);
   //--- process subscription message
   virtual bool      OnMessageSubscribe(CUniNewsMsgSubscribe *msg);
   //--- send login result message
   bool              SendLoginResult(const uint32_t result=CUniNewsMsgLoginResult::RESULT_COMMON_ERROR);
   //--- send logout message
   bool              SendLogout(const uint32_t reason=CUniNewsMsgLogout::LOGOUT_REASON_UNKNOWN);
   //--- send news message
   bool              SendNews(CUniNewsMsgNews *news,LPCWSTR keywords,bool history_news=false);
   //--- send ping message
   bool              SendPing(void);

private:
   //--- check login
   bool              CheckLogin(LPCWSTR login,LPCWSTR password_hash);
   //--- thread of connection processing
   static uint32_t __stdcall ProcessThreadWrapper(LPVOID param);
   void              ProcessThread(void);
   //--- receive data
   bool              Receive(bool &data_processed);
   //--- send data
   bool              Send(bool &data_processed);
   //--- check if readible
   int32_t           IsReadible(void);
   //--- read socket data
   bool              Read(char *buf,int buf_max,int &buf_readed,DWORD* err_code=NULL);
   //--- write socket data
   bool              Write(const char *buf,const uint32_t len,DWORD* err_code=NULL);
  };
//+------------------------------------------------------------------+
