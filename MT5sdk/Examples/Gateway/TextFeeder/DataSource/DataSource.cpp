//+------------------------------------------------------------------+
//|                                         MetaTrader 5 Text Feeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DataSource.h"
//+------------------------------------------------------------------+
//| Initialize static variables                                      |
//+------------------------------------------------------------------+
wchar_t CDataSource::s_param_seps[]=L"|\n";
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDataSource::CDataSource() : m_gateway(NULL),m_workflag(0),m_initalized(0),m_state(STATE_DISCONNECTED),
                             m_tick_period(0),m_tick_lasttime(0),m_tick_start(0),m_tick_counter(0),
                             m_book_period(0),m_book_lasttime(0),m_book_start(0),m_book_counter(0),
                             m_news_period(0),m_news_lasttime(0),m_news_body_buffer(NULL)
  {
//--- null strings
   m_tick_filename[0]  =L'\0';
   m_book_filename[0]  =L'\0';
   m_news_filename[0]  =L'\0';
   m_buffer[0]         =L'\0';
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDataSource::~CDataSource(void)
  {
//--- close all
   Shutdown();
//--- delete the news buffer if there is any
   if(m_news_body_buffer)
     {
      delete[] m_news_body_buffer;
      m_news_body_buffer=NULL;
     }
  }
//+------------------------------------------------------------------+
//| Thread of the external connection handling                       |
//+------------------------------------------------------------------+
bool CDataSource::Start(IMTGatewayAPI *gateway)
  {
//--- checking
   if(!gateway)
     {
      ExtLogger.OutString(MTLogErr,L"invalid datafeed parameters");
      return(false);
     }
//--- remember the pointer to the gateway interface
   m_gateway=gateway;
//--- set the thread operation sign
   InterlockedExchange(&m_workflag,1);
//--- start the thread handling data of the external connection
   if(!m_thread.Start(ProcessThreadWrapper,this,STACK_SIZE_COMMON))
     {
      ExtLogger.OutString(MTLogErr,L"failed to start remote server connection thread");
      m_gateway=NULL;
      return(false);
     }
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Stop the handling thread                                         |
//+------------------------------------------------------------------+
bool CDataSource::Shutdown(void)
  {
//--- reset the initialization sign
   InterlockedExchange(&m_initalized,0);
//--- set the thread operation completion sign
   InterlockedExchange(&m_workflag,0);
//--- complete the operation of the quote feed thread
   if(m_thread.Handle())
      if(!m_thread.Shutdown(THREAD_TIMEOUT))
         m_thread.Terminate();
//--- reset the pointer to the gateway interface
   m_gateway=NULL;
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Initialization of the quote feed                                 |
//+------------------------------------------------------------------+
bool CDataSource::Init(const IMTConFeeder *config)
  {
//--- checking
   if(!config)
     {
      ExtLogger.OutString(MTLogErr,L"invalid datafeed config interface");
      return(false);
     }
//--- check, if already initialized, quit
   if(InterlockedExchangeAdd(&m_initalized,0))
      return(true);
//--- set the feeder parameters
   if(!ParametersParse(config))
     {
      ExtLogger.OutString(MTLogErr,L"failed to parse datafeed parameters");
      SetState(STATE_DISCONNECTED);
      return(false);
     }
//--- record to the journal
   ExtLogger.OutString(MTLogOK,L"datafeed initialized");
//--- refresh the state
   SetState(STATE_CONNECTSTART);
//--- set the feeder initialization sign
   InterlockedExchange(&m_initalized,1);
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Check timeouts                                                   |
//+------------------------------------------------------------------+
bool CDataSource::Check(void)
  {
//--- if not initialized, quit
   if(!InterlockedExchangeAdd(&m_initalized,0))
      return(true);
//--- check the state
   if(GetState()==STATE_DISCONNECTED)
      return(false);
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Starting the thread handling data of the external connection     |
//+------------------------------------------------------------------+
uint32_t __stdcall CDataSource::ProcessThreadWrapper(LPVOID param)
  {
//--- checking
   CDataSource *pThis=reinterpret_cast<CDataSource*>(param);
   if(pThis)
      pThis->ProcessThread();
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Processing data of the external connection                       |
//+------------------------------------------------------------------+
__declspec(noinline) void CDataSource::ProcessThread(void)
  {
//--- external connection management loop
   while(InterlockedExchangeAdd(&m_workflag,0)>0)
     {
      //--- analyze the state, start connection, and connect
      if(GetState()==STATE_CONNECTSTART)
         ProcessConnect();
      //--- connected, receive data
      if(GetState()==STATE_CONNECTED)
         ProcessData();
      //--- sleep
      Sleep(100);
     }
  }
//+------------------------------------------------------------------+
//| Connection                                                       |
//+------------------------------------------------------------------+
bool CDataSource::ProcessConnect(void)
  {
   bool ticks=false,news=false,res=true;
//--- open the file of ticks, if necessary
   if(m_tick_filename[0])
     {
      if(m_tick_file.Open(m_tick_filename))
         ticks=true;
      else
        {
         ExtLogger.Out(MTLogErr,L"failed to open ticks file: %s [%d]",m_tick_filename,::GetLastError());
         m_tick_filename[0]=0;
        }
     }
//--- open the file of books, if necessary
   if(m_book_filename[0])
     {
      if(m_book_file.Open(m_book_filename))
         ticks=true;
      else
        {
         ExtLogger.Out(MTLogErr,L"failed to open books file: %s [%d]",m_book_filename,::GetLastError());
         m_book_filename[0]=0;
        }
     }
//--- open the file of news, if necessary
   if(m_news_filename[0])
     {
      if(m_news_file.Open(m_news_filename))
         news=true;
      else
        {
         ExtLogger.Out(MTLogErr,L"failed to open news file: %s [%d]",m_news_filename,::GetLastError());
         m_news_filename[0]=0;
        }
     }
//--- the datafeed should receive news or quotes or economic events
   res=news || ticks;
//--- record the result to journal and refresh the state
   if(res)
     {
      ExtLogger.OutString(MTLogOK,L"connect to remote server completed");
      SetState(STATE_CONNECTED);
     }
   else
     {
      ExtLogger.OutString(MTLogErr,L"connect to remote server failed");
      SetState(STATE_DISCONNECTED);
     }
//--- update connected state
   if(m_gateway)
      m_gateway->StateConnect(true);
//--- return the result
   return(true);
  }
//+------------------------------------------------------------------+
//| Receiving quotes                                                 |
//+------------------------------------------------------------------+
bool CDataSource::ProcessData(void)
  {
   bool res=true;
//--- sending ticks
   if(res && !TicksApply())
     {
      ExtLogger.OutString(MTLogErr,L"ticks apply failed");
      res=false;
     }
//--- sending books
   if(res && !BooksApply())
     {
      ExtLogger.OutString(MTLogErr,L"books apply failed");
      res=false;
     }
//--- sending news
   if(res && !NewsApply())
     {
      ExtLogger.OutString(MTLogErr,L"news apply failed");
      res=false;
     }
//--- if everything is bad, refresh the state
   if(!res)
      SetState(STATE_DISCONNECTED);
//--- return the result
   return(res);
  }
//+------------------------------------------------------------------+
//| Sending ticks                                                    |
//+------------------------------------------------------------------+
bool CDataSource::TicksApply(void)
  {
   MTTick tick={0};
   bool   res=false;
//--- checking time
   if(!TickPeriodCheck())
      return(true);
//--- check using of file
   if(m_tick_filename[0]==L'\0')
      return(true);
//--- analyze the string with a tick
   if(res=TickParse(tick))
     {
      //--- sending data
      if(m_gateway)
         res=m_gateway->SendTicks(&tick,1)==MT_RET_OK;
     }
   else
      ExtLogger.Out(MTLogErr,L"tick parse failed '{0}'",m_buffer);
//--- return the result
   return(res);
  }
//+------------------------------------------------------------------+
//| Checking the period of ticks                                     |
//+------------------------------------------------------------------+
bool CDataSource::TickPeriodCheck(void)
  {
   int64_t crnt_time=GetTickCount();
//--- compare the current time and the time of the last reference
   if((crnt_time-m_tick_lasttime)<m_tick_period) return(false);
   m_tick_lasttime=crnt_time;
   return(true);
  }
//+------------------------------------------------------------------+
//| Parsing of ticks                                                 |
//+------------------------------------------------------------------+
bool CDataSource::TickParse(MTTick &tick)
  {
   wchar_t tmp_buf[16]={0};
   int32_t     beg=0,end=0;
//--- if we couldn't read the string, reset the file and try to read once again
   if(!m_tick_file.GetNextLine(m_buffer,_countof(m_buffer)))
     {
      //--- file end, reset the file
      m_tick_file.Reset();
      //--- read, if we couldn't read, there is an error
      if((m_tick_file.GetNextLine(m_buffer,_countof(m_buffer)))==0)
        {
         ExtLogger.Out(MTLogErr,L"tick file %s reading error[%d]",m_tick_filename,::GetLastError());
         return(false);
        }
     }
//--- update incoming traffic
   if(m_gateway)
      m_gateway->StateTraffic(CMTStr::Len(m_buffer)*sizeof(wchar_t),0);
//--- receive the symbol
   if((end=CMTStr::FindChar(m_buffer+beg,L','))<0)
      return(false);
   wcsncpy_s(tick.symbol,_countof(tick.symbol),m_buffer+beg,end);
//--- bank
   beg+=end+1;
   if((end=CMTStr::FindChar(m_buffer+beg,L','))<0)
      return(false);
   wcsncpy_s(tick.bank,_countof(tick.bank)-1,m_buffer+beg,end);
//--- buy
   beg+=end+1;
   if((end=CMTStr::FindChar(m_buffer+beg,L','))<0)
      return(false);
   wcsncpy_s(tmp_buf,_countof(tmp_buf)-1,m_buffer+beg,end);
   if((tick.bid=_wtof(tmp_buf))<0)
      tick.bid=0;
//--- sell
   beg+=end+1;
   if((end=CMTStr::FindChar(m_buffer+beg,L','))<0)
      return(false);
   wcsncpy_s(tmp_buf,_countof(tmp_buf)-1,m_buffer+beg,end);
   if((tick.ask=_wtof(tmp_buf))<0)
      tick.ask=0;
//--- last price
   beg+=end+1;
   if((end=CMTStr::FindChar(m_buffer+beg,L','))<0)
      return(false);
   wcsncpy_s(tmp_buf,_countof(tmp_buf),m_buffer+beg,end);
   if((tick.last=_wtof(tmp_buf))<0)
      tick.last=0;
//--- tick volume
   beg+=end+1;
   tick.volume=_wtoi(m_buffer+beg);
//--- tick datetime 
   if(m_tick_start)
     {
      //--- tick datetime
      tick.datetime+=m_tick_start+m_tick_counter*m_tick_period;
      //--- tick counter
      m_tick_counter++;
     }
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Sending books                                                    |
//+------------------------------------------------------------------+
bool CDataSource::BooksApply(void)
  {
   MTBook book={0};
   bool   res =false;
//--- checking time
   if(!BookPeriodCheck())
      return(true);
//--- check using of file
   if(m_book_filename[0]==L'\0')
      return(true);
//--- analyze the string with a tick
   if(res=BookParse(book))
     {
      //--- sending data
      if(m_gateway)
         res=m_gateway->SendBooks(&book,1)==MT_RET_OK;
     }
   else
      ExtLogger.Out(MTLogErr,L"book parse failed(%s)",m_buffer);
//--- return the result
   return(res);
  }
//+------------------------------------------------------------------+
//| Checking the period of books                                     |
//+------------------------------------------------------------------+
bool CDataSource::BookPeriodCheck(void)
  {
   int64_t crnt_time=GetTickCount();
//--- compare the current time and the time of the last reference
   if((crnt_time-m_book_lasttime)<m_book_period) return(false);
   m_book_lasttime=crnt_time;
   return(true);
  }
//+------------------------------------------------------------------+
//| Parsing of books                                                 |
//+------------------------------------------------------------------+
bool CDataSource::BookParse(MTBook &book)
  {
   wchar_t flags[64]={0};
//--- if we couldn't read the string, reset the file and try to read once again
   if(!m_book_file.GetNextLine(m_buffer,_countof(m_buffer)))
     {
      //--- file end, reset the file
      m_book_file.Reset();
      //--- read, if we couldn't read, there is an error
      if((m_book_file.GetNextLine(m_buffer,_countof(m_buffer)))==0)
        {
         ExtLogger.Out(MTLogErr,L"book file %s reading error[%d]",m_book_filename,::GetLastError());
         return(false);
        }
     }
//--- update incoming traffic
   if(m_gateway)
      m_gateway->StateTraffic(CMTStr::Len(m_buffer)*sizeof(wchar_t),0);
//--- read book symbol
   if(!ReadParamStr(m_buffer,L"Symbol=",book.symbol,_countof(book.symbol)))
     {
      ExtLogger.Out(MTLogErr,L"failed to read book symbol[%s]",m_buffer);
      return(false);
     }
//--- if we couldn't read the string, reset the file and try to read once again
   if(!m_book_file.GetNextLine(m_buffer,_countof(m_buffer)))
     {
      ExtLogger.Out(MTLogErr,L"book file %s reading error[%d]",m_book_filename,::GetLastError());
      return(false);
     }
//--- update incoming traffic
   if(m_gateway)
      m_gateway->StateTraffic(CMTStr::Len(m_buffer)*sizeof(wchar_t),0);
//--- read book flags
   if(!ReadParamStr(m_buffer,L"Flags=",flags,_countof(flags)))
     {
      ExtLogger.Out(MTLogErr,L"failed to read book flags[%s]",m_buffer);
      return(false);
     }
//--- analyze flag values
   if(CMTStr::Find(flags,L"PRE_AUCTION")>=0)
      book.flags=MTBook::FLAG_PRE_AUCTION;
   else
      book.flags=MTBook::FLAG_NONE;
//--- read, if we couldn't read, there is an error
   if((m_book_file.GetNextLine(m_buffer,_countof(m_buffer)))==0)
     {
      ExtLogger.Out(MTLogErr,L"unexpected end of book file %s[%d]",m_book_filename,::GetLastError());
      return(false);
     }
//--- update incoming traffic
   if(m_gateway)
      m_gateway->StateTraffic(CMTStr::Len(m_buffer)*sizeof(wchar_t),0);
//--- read all book items until "End" not found
   while(CMTStr::Find(m_buffer,L"End")!=0)
     {
      //--- read book item to book
      if(!BookParseItem(m_buffer,book))
        {
         ExtLogger.Out(MTLogErr,L"failed to read book item[%s]",m_buffer);
         return(false);
        }
      //--- read, if we couldn't read, there is an error
      if((m_book_file.GetNextLine(m_buffer,_countof(m_buffer)))==0)
        {
         ExtLogger.Out(MTLogErr,L"unexpected end of book file %s[%d]",m_book_filename,::GetLastError());
         return(false);
        }
      //--- update incoming traffic
      if(m_gateway)
         m_gateway->StateTraffic(CMTStr::Len(m_buffer)*sizeof(wchar_t),0);
     }
//--- book datetime 
   if(m_book_start)
     {
      //--- tick datetime
      book.datetime+=m_book_start+m_book_counter*m_book_period;
      //--- tick counter
      m_book_counter++;
     }
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Read book item from buffer                                       |
//+------------------------------------------------------------------+
bool CDataSource::BookParseItem(LPCWSTR str,MTBook &book)
  {
   LPCTSTR ptr  =str;
   int32_t     index=0;
//--- checking, check book size
   if(!str || book.items_total+1>=_countof(book.items))
      return(false);
//--- clear item type
   book.items[book.items_total].type=MTBookItem::ItemReset;
//--- Sell item
   if(CMTStr::Compare(str,L"Sell,",_countof(L"Sell,")-1)==0)
     {
      //--- item type
      book.items[book.items_total].type=MTBookItem::ItemSell;
     }
//--- Buy item
   if(CMTStr::Compare(str,L"Buy,",_countof(L"Buy,")-1)==0)
     {
      //--- item type
      book.items[book.items_total].type=MTBookItem::ItemBuy;
     }
//--- SellMarket item
   if(CMTStr::Compare(str,L"SellMarket,",_countof(L"SellMarket,")-1)==0)
     {
      //--- item type
      book.items[book.items_total].type=MTBookItem::ItemSellMarket;
     }
//--- BuyMarket item
   if(CMTStr::Compare(str,L"BuyMarket,",_countof(L"BuyMarket,")-1)==0)
     {
      //--- item type
      book.items[book.items_total].type=MTBookItem::ItemBuyMarket;
     }
//--- check item type
   if(book.items[book.items_total].type!=MTBookItem::ItemSell && book.items[book.items_total].type!=MTBookItem::ItemBuy &&
      book.items[book.items_total].type!=MTBookItem::ItemSellMarket && book.items[book.items_total].type!=MTBookItem::ItemBuyMarket)
     {
      ExtLogger.Out(MTLogErr,L"invalid book item type[%s]",str);
      return(false);
     }
//--- skip delimiter
   if((index=CMTStr::FindChar(ptr,L','))<0)
     {
      ExtLogger.Out(MTLogErr,L"invalid book item[%s]",str);
      return(false);
     }
   index++;
   ptr+=index;
//--- get item price
   book.items[book.items_total].price=_wtof(ptr);
//--- skip next delimiter
   if((index=CMTStr::FindChar(ptr,L','))<0)
     {
      ExtLogger.Out(MTLogErr,L"invalid book item[%s]",str);
      return(false);
     }
   index++;
   ptr+=index;
//--- get item volume
   book.items[book.items_total].volume=_wtoi64(ptr);
//--- check item
   if(book.items[book.items_total].price<0 || book.items[book.items_total].volume<=0)
     {
      ExtLogger.Out(MTLogErr,L"invalid book item[%s]",str);
      return(false);
     }
//--- increment total book items
   book.items_total++;
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Sending news                                                     |
//+------------------------------------------------------------------+
bool CDataSource::NewsApply(void)
  {
   MTNews news={0};
   bool   res=false;
//--- checking time
   if(!NewsPeriodCheck())
      return(true);
//--- check using of file
   if(m_news_filename[0]==L'\0')
      return(true);
//--- analyze the string with a tick
   if(res=NewsParse(news))
     {
      //--- sending data
      if(m_gateway)
         res=m_gateway->SendNews(&news,1)==MT_RET_OK;
     }
//--- return the result
   return(res);
  }
//+------------------------------------------------------------------+
//| Checking the period of news                                      |
//+------------------------------------------------------------------+
bool CDataSource::NewsPeriodCheck(void)
  {
   int64_t crnt_time=GetTickCount();
//--- compare the current time and the time of the last reference
   if((crnt_time-m_news_lasttime)<m_news_period) return(false);
   m_news_lasttime=crnt_time;
   return(true);
  }
//+------------------------------------------------------------------+
//| Parsing of news                                                  |
//+------------------------------------------------------------------+
bool CDataSource::NewsParse(MTNews &news)
  {
   bool res=true;
//--- if we couldn't read the string, reset the file and try to read once again
   if((m_news_file.GetNextLine(m_buffer,_countof(m_buffer)))==0)
     {
      //--- file end, reset the file
      m_news_file.Reset();
      //--- read, if we couldn't read, there is an error
      if((m_news_file.GetNextLine(m_buffer,_countof(m_buffer)))==0)
        {
         ExtLogger.Out(MTLogErr,L"news file %s reading error[%d]",m_news_filename,::GetLastError());
         res=false;
        }
     }
//--- update incoming traffic
   if(m_gateway)
      m_gateway->StateTraffic(CMTStr::Len(m_buffer)*sizeof(wchar_t),0);
//--- search for the descriptor of news start
   while(CMTStr::Find(m_buffer,L"<mqnews>")<0)
     {
      //--- error: there is no descriptor "<mqnews>" in the file
      if((m_news_file.GetNextLine(m_buffer,_countof(m_buffer)))==0)
        {
         ExtLogger.Out(MTLogErr,L"invalid news file %s",m_news_filename);
         return(false);
        }
      //--- update incoming traffic
      if(m_gateway)
         m_gateway->StateTraffic(CMTStr::Len(m_buffer)*sizeof(wchar_t),0);
     }
//--- get the news sybject, if we didn't get or find the subject, it's bad
   if((m_news_file.GetNextLine(m_buffer,_countof(m_buffer)))==0 ||
      !ReadParamStr(m_buffer,L"subject=",news.subject,_countof(news.subject)-1))
     {
      ExtLogger.Out(MTLogErr,L"failed to read news subject from file %s",m_news_filename);
      return(false);
     }
//--- update incoming traffic
   if(m_gateway)
      m_gateway->StateTraffic(CMTStr::Len(m_buffer)*sizeof(wchar_t),0);
//--- get the news category
   if((m_news_file.GetNextLine(m_buffer,_countof(m_buffer)))==0 ||
      !ReadParamStr(m_buffer,L"category=",news.category,_countof(news.category)-1))
     {
      ExtLogger.Out(MTLogErr,L"failed to read news category from file %s",m_news_filename);
      return(false);
     }
//--- update incoming traffic
   if(m_gateway)
      m_gateway->StateTraffic(CMTStr::Len(m_buffer)*sizeof(wchar_t),0);
//--- get the language code
   if((m_news_file.GetNextLine(m_buffer,_countof(m_buffer)))==0 ||
      !ReadParamInt(m_buffer,L"lang=",(int*)&news.language))
     {
      ExtLogger.Out(MTLogErr,L"failed to read news language from file %s",m_news_filename);
      return(false);
     }
//--- update incoming traffic
   if(m_gateway)
      m_gateway->StateTraffic(CMTStr::Len(m_buffer)*sizeof(wchar_t),0);
//--- begining of the news body
   if((m_news_file.GetNextLine(m_buffer,_countof(m_buffer)))==0 ||
      CMTStr::Find(m_buffer,L"<mqnewsbody>")<0)
     {
      ExtLogger.Out(MTLogErr,L"failed to read news body from file %s",m_news_filename);
      return(false);
     }
//--- update incoming traffic
   if(m_gateway)
      m_gateway->StateTraffic(CMTStr::Len(m_buffer)*sizeof(wchar_t),0);
//--- get the next string
   if((m_news_file.GetNextLine(m_buffer,_countof(m_buffer)))==0)
     {
      ExtLogger.Out(MTLogErr,L"unexpected end of file %s",m_news_filename);
      return(false);
     }
//--- update incoming traffic
   if(m_gateway)
      m_gateway->StateTraffic(CMTStr::Len(m_buffer)*sizeof(wchar_t),0);
//--- create a buffer for the news body, if it hasn't been created yet
   if(!m_news_body_buffer && (m_news_body_buffer=new wchar_t[MTNews::MAX_NEWS_BODY_LEN])==NULL)
     {
      ExtLogger.Out(MTLogErr,L"failed to allocate news body for %s",news.subject);
      return(false);
     }
//--- clear the buffer
   ZeroMemory(m_news_body_buffer,MTNews::MAX_NEWS_BODY_LEN*sizeof(wchar_t));
//--- we haven't found the end yet
   while(CMTStr::Find(m_buffer,L"</mqnewsbody>")<0)
     {
      //--- add a string
      CMTStr::Append(m_news_body_buffer,MTNews::MAX_NEWS_BODY_LEN-1,m_buffer);
      //--- add a line feed
      CMTStr::Append(m_news_body_buffer,MTNews::MAX_NEWS_BODY_LEN-1,L"\r\n");
      //--- get the next string
      if((m_news_file.GetNextLine(m_buffer,_countof(m_buffer)))==0)
        {
         ExtLogger.Out(MTLogErr,L"unexpected end of file %s, news body read failed",m_news_filename);
         res=false;
         break;
        }
      //--- update incoming traffic
      if(m_gateway)
         m_gateway->StateTraffic(CMTStr::Len(m_buffer)*sizeof(wchar_t),0);
     }
//--- if everything is ok, get the news end
   if(res)
     {
      //--- the next string should contain "</mqnews>"
      if((m_news_file.GetNextLine(m_buffer,_countof(m_buffer)))==0 ||
          CMTStr::Find(m_buffer,L"</mqnews>")<0)
        {
         ExtLogger.Out(MTLogErr,L"failed to read news(%s) end from file %s",news.subject,m_news_filename);
         res=false;
        }
     }
//--- if everything is ok, copy the news
   if(res)
     {
      //--- count the news size
      news.body_len=CMTStr::Len(m_news_body_buffer);
      //--- if a news has a body
      if(news.body_len)
         news.body=m_news_body_buffer;
      else
         news.body=NULL;
     }
//--- return the result
   return(res);
  }
//+------------------------------------------------------------------+
//| Getting parameters                                               |
//+------------------------------------------------------------------+
bool CDataSource::ParametersParse(const IMTConFeeder *config)
  {
   IMTConParam *param=NULL;
   CMTStrPath   str,path;
   int32_t          pos=0;
//--- checking
   if(!config)
      return(false);
//--- get the path to the application
   ::GetModuleFileNameW(NULL,path.Buffer(),path.Max());
   path.Refresh();
   if((pos=path.FindRChar(L'\\'))>0)
      path.Trim(pos);
//--- create parameter interface
   if((param=m_gateway->FeederParamCreate())==NULL)
      return(false);
//--- go through all parameters
   for(uint32_t i=0;i<config->ParameterTotal();i++)
     {
      //--- get a parameter by index
      if(config->ParameterNext(i,param)!=MT_RET_OK)
         break;
      //--- analyze the parameter name, tick sending period
      if(CMTStr::Compare(L"TickPeriod",param->Name())==0)
        {
         //--- get the parameter value
         m_tick_period=_wtoi(param->Value());
         //--- check the parameter value
         if(m_tick_period>TICKS_PERIOD_MAX)
            m_tick_period=TICKS_PERIOD_MAX;
         if(m_tick_period<TICKS_PERIOD_MIN)
            m_tick_period=TICKS_PERIOD_MIN;
         continue;
        }
      //--- a text file with ticks
      if(CMTStr::Compare(L"TickFile",param->Name())==0)
        {
         //--- get the parameter
         str.Assign(param->Value());
         //--- form a path to the file
         CMTStr::FormatStr(m_tick_filename,_countof(m_tick_filename)-1,L"%s\\%s",path.Str(),str.Str());
         continue;
        }
      //--- ticks start time
      if(CMTStr::Compare(L"TickStart",param->Name())==0)
        {
         tm ttm={};
         //--- parse datetime
         if(swscanf_s(param->Value(),L"%04d.%02d.%02d %02d:%02d:%02d",
                     &ttm.tm_year,&ttm.tm_mon,&ttm.tm_mday,
                     &ttm.tm_hour,&ttm.tm_min,&ttm.tm_sec)>=0)
           {
            ttm.tm_year-=1900;
            ttm.tm_mon -=1;
            //--- make time
            m_tick_start=SMTTime::MakeTime(&ttm);
           }
         continue;
        }
      //--- get a parameter by index
      if(config->ParameterNext(i,param)!=MT_RET_OK)
         break;
      //--- analyze the parameter name, tick sending period
      if(CMTStr::Compare(L"BookPeriod",param->Name())==0)
        {
         //--- get the parameter value
         m_book_period=_wtoi(param->Value());
         //--- check the parameter value
         if(m_book_period>BOOKS_PERIOD_MAX)
            m_book_period=BOOKS_PERIOD_MAX;
         if(m_book_period<BOOKS_PERIOD_MIN)
            m_book_period=BOOKS_PERIOD_MIN;
         continue;
        }
      //--- a text file with ticks
      if(CMTStr::Compare(L"BookFile",param->Name())==0)
        {
         //--- get the parameter
         str.Assign(param->Value());
         //--- form a path to the file
         CMTStr::FormatStr(m_book_filename,_countof(m_book_filename)-1,L"%s\\%s",path.Str(),str.Str());
         continue;
        }
      //--- ticks start time
      if(CMTStr::Compare(L"BookStart",param->Name())==0)
        {
         tm ttm={};
         //--- parse datetime
         if(swscanf_s(param->Value(),L"%04d.%02d.%02d %02d:%02d:%02d",
                     &ttm.tm_year,&ttm.tm_mon,&ttm.tm_mday,
                     &ttm.tm_hour,&ttm.tm_min,&ttm.tm_sec)>=0)
           {
            ttm.tm_year-=1900;
            ttm.tm_mon -=1;
            //--- make time
            m_book_start=SMTTime::MakeTime(&ttm);
           }
         continue;
        }
      //--- news sending period
      if(CMTStr::Compare(L"NewsPeriod",param->Name())==0)
        {
         //--- get the parameter value
         m_news_period=_wtoi(param->Value());
         //--- check the parameter value
         if(m_news_period>NEWS_PERIOD_MAX)
            m_news_period=NEWS_PERIOD_MAX;
         if(m_news_period<NEWS_PERIOD_MIN)
            m_news_period=NEWS_PERIOD_MIN;
         continue;
        }
      //--- a text file with news
      if(CMTStr::Compare(L"NewsFile",param->Name())==0)
        {
         //--- get the parameter value
         str.Assign(param->Value());
         //--- form a path to the file
         CMTStr::FormatStr(m_news_filename,_countof(m_news_filename)-1,L"%s\\%s",path.Str(),str.Str());
        }
     }
//--- release the parameter interface
   if(param)
      param->Release();
//--- return the result
   return(m_tick_filename[0] || m_book_filename[0] || m_news_filename[0]);
  }
//+------------------------------------------------------------------+
//| Search for a string parameter in the string                      |
//+------------------------------------------------------------------+
inline uint32_t CDataSource::ReadParamStr(LPCWSTR string,LPCWSTR param,LPWSTR buf,const uint32_t maxchars,LPCWSTR seps/*=NULL*/)
  {
   wchar_t *res,*end;
   int32_t      index;
//--- checking
   if(!string || !param || !buf)                  return(0);
//--- if separators are not specified, use the standard set
   if(!seps)
      seps=s_param_seps;
//--- search for the parameter
   if((index=FindParameter(string,param,seps))<0) return(0);
//--- take the result
   string+=index;
   for(res=buf,end=buf+maxchars-1; *string!=0 && (wcschr(seps,*string)==NULL) && res<end;)
      *res++=*string++;
   *res=L'\0';
//--- exit
   return(uint32_t(res-buf));
  }
//+------------------------------------------------------------------+
//| Search for an integer parameter in the string                    |
//+------------------------------------------------------------------+
inline bool CDataSource::ReadParamInt(LPCWSTR string,LPCWSTR param,int *data)
  {
   int32_t index;
//--- checking
   if(!string || !param || !data)                         return(false);
//--- secrh for the parameter
   if((index=FindParameter(string,param,s_param_seps))<0) return(false);
//--- take the result
   *data=_wtoi(&string[index]);
   return(true);
  }
//+------------------------------------------------------------------+
//| Search for a parameter in the string                             |
//+------------------------------------------------------------------+
inline int32_t CDataSource::FindParameter(LPCWSTR string,LPCWSTR param,LPCWSTR seps)
  {
   const wchar_t *str,*beg;
//--- checking
   if(!string || !param) return(-1);
//--- remember the string beginning
   beg=string;
//--- search for entries
   while(true)
     {
      if((str=wcsstr(beg,param))==NULL) return(-1);
      //--- if the pewvious symbol is a separator or entry from the first symbol, we've found
      if(((str!=beg) && wcschr(seps,*(str-1))) || (str==beg)) break;
      //--- not the full name if a parameter, go further
      beg=str+wcslen(param);
     }
//--- we've found, position of the value
   return(int(str-string+wcslen(param)));
  }
//+------------------------------------------------------------------+
