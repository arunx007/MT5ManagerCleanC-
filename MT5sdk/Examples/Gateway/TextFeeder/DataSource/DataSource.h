//+------------------------------------------------------------------+
//|                                         MetaTrader 5 Text Feeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Class of data processing for extrenal connection                 |
//+------------------------------------------------------------------+
class CDataSource
  {
public:
   //--- state of external connection
   enum
     {
      STATE_DISCONNECTED=0x00,                                       // no connection
      STATE_CONNECTSTART=0x01,                                       // connection start
      STATE_CONNECTED   =0x02,                                       // connection established
     };

private:
   //--- constants
   enum
     {
      THREAD_TIMEOUT       =2000,                                    // thread completion timeout, ms
      TICKS_PERIOD_MIN     =0,                                       // minimal period of tick sending, ms
      TICKS_PERIOD_MAX     =60000,                                   // maximal period of tick sending, ms
      BOOKS_PERIOD_MIN     =0,                                       // minimal period of books sending, ms
      BOOKS_PERIOD_MAX     =60000,                                   // maximal period of books sending, ms
      NEWS_PERIOD_MIN      =0,                                       // minimal period of news sending, ms
      NEWS_PERIOD_MAX      =60000,                                   // maximal period of news sending, ms
     };
   // separator of parameters string
   static wchar_t    s_param_seps[];
   //--- pointer to the gateway interface
   IMTGatewayAPI    *m_gateway;
   //--- data processing thread
   CMTThread         m_thread;
   //--- sign of thread operation
   volatile long     m_workflag;
   //--- sign of initialization (receipt of connection settings)
   volatile long     m_initalized;
   //--- state of the external connection
   volatile long     m_state;
   //--- data for tick processing
   CStringFile       m_tick_file;
   wchar_t           m_tick_filename[MAX_PATH];
   int32_t           m_tick_period;
   int64_t           m_tick_lasttime;
   int64_t           m_tick_start;
   int64_t           m_tick_counter;
   //--- data for books processing
   CStringFile       m_book_file;
   wchar_t           m_book_filename[MAX_PATH];
   int32_t           m_book_period;
   int64_t           m_book_lasttime;
   int64_t           m_book_start;
   int64_t           m_book_counter;
   //--- data for news processing
   CStringFile       m_news_file;
   wchar_t           m_news_filename[MAX_PATH];
   int32_t           m_news_period;
   int64_t           m_news_lasttime;
   wchar_t          *m_news_body_buffer;
   //--- data buffer
   wchar_t           m_buffer[1024];

public:
   //--- constructor/destructor
                     CDataSource();
                    ~CDataSource(void);
   //--- start/stop of external connection data processing thread
   bool              Start(IMTGatewayAPI *gateway);
   bool              Shutdown(void);
   //--- initialization of external connection
   bool              Init(const IMTConFeeder *config);
   //--- connection check
   bool              Check(void);
   //--- state of the external connection
   LONG              GetState()           { return(InterlockedExchangeAdd((LONG *)&m_state,0)); }
   LONG              SetState(LONG state) { return(InterlockedExchange((LONG *)&m_state,state));}

private:
   //--- external connection data processing thread
   static uint32_t __stdcall ProcessThreadWrapper(LPVOID param);
   void              ProcessThread(void);
   //--- external connection
   bool              ProcessConnect(void);
   //--- processing of external connection data
   bool              ProcessData(void);
   //--- sending ticks
   bool              TicksApply(void);
   bool              TickParse(MTTick &tick);
   bool              TickPeriodCheck(void);
   //--- sending books
   bool              BooksApply(void);
   bool              BookParse(MTBook &book);
   bool              BookParseItem(LPCWSTR str,MTBook &book);
   bool              BookPeriodCheck(void);
   //--- sending news
   bool              NewsApply(void);
   bool              NewsParse(MTNews &news);
   bool              NewsPeriodCheck(void);
   //--- receiving parameters
   bool              ParametersParse(const IMTConFeeder *config);
   //--- read parameters
   uint32_t          ReadParamStr(LPCWSTR string,LPCWSTR param,LPWSTR buf,const uint32_t maxchars,LPCWSTR seps=nullptr);
   bool              ReadParamInt(LPCWSTR string,LPCWSTR param,int *data);
   static int32_t    FindParameter(LPCWSTR string,LPCWSTR param,LPCWSTR seps);
  };
//+------------------------------------------------------------------+
