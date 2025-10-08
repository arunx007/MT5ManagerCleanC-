//+------------------------------------------------------------------+
//|                                          MetaTrader 5 TextFeeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
namespace TextFeeder.NET
  {
   using TextFeeder.NET.DataSource;
   using MetaQuotes.MT5CommonAPI;
   using MetaQuotes.MT5GatewayAPI;
   using System;
   using System.Collections.Generic;
   using System.Globalization;
   using System.IO;
   using System.Runtime.InteropServices;
   using System.Text;
   using System.Threading;
   using System.Xml;
   using System.Xml.Serialization;
   //+------------------------------------------------------------------+
   //| Server and external connection management class                  |
   //+------------------------------------------------------------------+
   class CDataSource: IDisposable
     {
      //--- state of external connection
      public enum EnState
        {
         STATE_DISCONNECTED=0x00,                                       // no connection
         STATE_CONNECTSTART=0x01,                                       // connection start
         STATE_CONNECTED   =0x02,                                       // connection established
        };
      //--- constants
      const int THREAD_TIMEOUT            =2000;                        // thread completion timeout, ms
      const int TICKS_PERIOD_MIN          =0;                           // minimal period of tick sending, ms
      const int TICKS_PERIOD_MAX          =60000;                       // maximal period of tick sending, ms
      const int BOOKS_PERIOD_MIN          =0;                           // minimal period of books sending, ms
      const int BOOKS_PERIOD_MAX          =60000;                       // maximal period of books sending, ms
      const int NEWS_PERIOD_MIN           =0;                           // minimal period of news sending, ms
      const int NEWS_PERIOD_MAX           =60000;                       // maximal period of news sending, ms
      //--- pointer to the gateway interface
      CIMTGatewayAPI    m_gateway         =null;
      //--- data processing thread
      Thread            m_thread          =null;
      //--- sign of thread operation
      long              m_workflag        =0;
      //--- sign of initialization (receipt of connection settings)
      long              m_initalized      =0;
      //--- state of the external connection
      int               m_state           =0;
      //--- data for tick processing
      StreamReader      m_tick_file       =null;
      string            m_tick_filename   =null;
      Int64             m_tick_period     =0;
      Int64             m_tick_lasttime   =0;
      Int64             m_tick_start      =0;
      Int64             m_tick_counter    =0;
      readonly MTTick   m_invalid_tick    =new MTTick() {symbol="INVALID_TICK"};
      //--- data for books processing
      StreamReader      m_book_file       =null;
      string            m_book_filename   =null;
      Int64             m_book_period     =0;
      Int64             m_book_lasttime   =0;
      Int64             m_book_start      =0;
      Int64             m_book_counter    =0;
      readonly MTBook   m_invalid_book    =new MTBook() {symbol="INVALID_BOOK"};
      //--- data for news processing
      MQNews[]          m_news            =null;
      int               m_news_current    =0;
      string            m_news_filename   =null;
      Int64             m_news_period     =0;
      Int64             m_news_lasttime   =0;
      readonly MTNews   m_invalid_news    =new MTNews() {subject="INVALID_NEWS"};
      //--- data buffer
      string            m_buffer          =null;
      //--- for valid double from string conversion
      NumberStyles      m_double_style    =NumberStyles.Any;
      IFormatProvider   m_double_format   =new NumberFormatInfo() {NumberDecimalDigits='.'};
      //+------------------------------------------------------------------+
      //| Constructor                                                      |
      //+------------------------------------------------------------------+
      public CDataSource()
        {
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
            ExtLogger.OutString(EnMTLogCode.MTLogErr,"invalid datafeed parameters");
            return(false);
           }
         //--- remember the pointer to the gateway interface
         m_gateway=gateway;
         //--- set the thread operation sign
         Interlocked.Exchange(ref m_workflag,1);
         //--- start the thread handling data of the external connection
         try
           {
            m_thread=new Thread(ProcessThread,CTextFeeder.STACK_SIZE_COMMON);
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
         //--- reset the initialization sign
         Interlocked.Exchange(ref m_initalized,0);
         //--- set the thread operation completion sign
         Interlocked.Exchange(ref m_workflag,0);
         //--- complete the operation of the quote feed thread
         if(m_thread!=null)
           {
            if(!m_thread.Join(THREAD_TIMEOUT))
               m_thread.Abort();
           }
         //--- reset the pointer to the gateway interface
         m_gateway=null;
         //---
         if(m_tick_file!=null)
            m_tick_file.Dispose();
         m_tick_file=null;
         m_tick_filename=null;
         //---
         if(m_book_file!=null)
            m_book_file.Dispose();
         m_book_file=null;
         m_book_filename=null;
         //---
         m_news_filename=null;
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
            ExtLogger.OutString(EnMTLogCode.MTLogErr,"invalid datafeed config interface");
            return(false);
           }
         //--- check, if already initialized, quit
         if(Interlocked.Add(ref m_initalized,0)!=0)
            return(true);
         //--- set the feeder parameters
         if(ParametersParse(config))
           {
            ExtLogger.OutString(EnMTLogCode.MTLogErr,"failed to parse datafeed parameters");
            SetState(EnState.STATE_DISCONNECTED);
            return(false);
           }
         //--- record to the journal
         ExtLogger.OutString(EnMTLogCode.MTLogOK,"datafeed initialized");
         //--- refresh the state
         SetState(EnState.STATE_CONNECTSTART);
         //--- set the feeder initialization sign
         Interlocked.Exchange(ref m_initalized,1);
         //--- everything is ok
         return(true);
        }
      //+------------------------------------------------------------------+
      //| Check timeouts                                                   |
      //+------------------------------------------------------------------+
      public bool Check()
        {
         //--- if not initialized, quit
         if(Interlocked.Add(ref m_initalized,0)==0)
            return(true);
         //--- check the state
         if(GetState()==EnState.STATE_DISCONNECTED)
            return(false);
         //--- everything is ok
         return(true);
        }
      //+------------------------------------------------------------------+
      //| Getting state                                                    |
      //+------------------------------------------------------------------+
      EnState GetState()
        {
         return((EnState)Interlocked.Add(ref m_state,0));
        }
      //+------------------------------------------------------------------+
      //| Setting state                                                    |
      //+------------------------------------------------------------------+
      EnState SetState(EnState state)
        {
         return((EnState)Interlocked.Exchange(ref m_state,(int)state));
        }
      //+------------------------------------------------------------------+
      //| Processing data of the external connection                       |
      //+------------------------------------------------------------------+
      void ProcessThread()
        {
         //--- external connection management loop
         while(Interlocked.Add(ref m_workflag,0)>0)
           {
            //--- analyze the state, start connection, and connect
            if(GetState()==EnState.STATE_CONNECTSTART)
               ProcessConnect();
            //--- connected, receive data
            if(GetState()==EnState.STATE_CONNECTED)
               ProcessData();
            //--- sleep
            Thread.Sleep(100);
           }
        }
      //+------------------------------------------------------------------+
      //| Create text reader object                                        |
      //+------------------------------------------------------------------+
      StreamReader CreateReader(string filename)
        {
         if(filename==null)
            return(null);
         //---
         try
           {
            StreamReader res=new StreamReader(filename,Encoding.ASCII);
            return(res);
           }
         catch(Exception ex)
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr,"failed to open file: {0} [{1}]",filename,ex.Message);
           }
         //---
         return(null);
        }
      //+------------------------------------------------------------------+
      //| Connection                                                       |
      //+------------------------------------------------------------------+
      bool ProcessConnect()
        {
         //--- open the files, if necessary
         bool ticks =((m_tick_file=CreateReader(m_tick_filename))!=null);
         bool books =((m_book_file=CreateReader(m_book_filename))!=null);
         //--- the datafeed should receive news or quotes or economic events
         bool res=ticks || books;
         //--- record the result to journal and refresh the state
         if(res)
           {
            ExtLogger.OutString(EnMTLogCode.MTLogOK,"connect to remote server completed");
            SetState(EnState.STATE_CONNECTED);
           }
         else
           {
            ExtLogger.OutString(EnMTLogCode.MTLogErr,"connect to remote server failed");
            SetState(EnState.STATE_DISCONNECTED);
           }
         //--- update connected state
         if(m_gateway!=null)
            m_gateway.StateConnect(true);
         //--- return the result
         return(true);
        }
      //+------------------------------------------------------------------+
      //| Receiving quotes                                                 |
      //+------------------------------------------------------------------+
      bool ProcessData()
        {
         bool res=true;
         //--- sending ticks
         if(res && !TicksApply())
           {
            ExtLogger.OutString(EnMTLogCode.MTLogErr,"ticks apply failed");
            res=false;
           }
         //--- sending books
         if(res && !BooksApply())
           {
            ExtLogger.OutString(EnMTLogCode.MTLogErr,"books apply failed");
            res=false;
           }
         //--- sending news
         if(res && !NewsApply())
           {
            ExtLogger.OutString(EnMTLogCode.MTLogErr,"news apply failed");
            res=false;
           }
         //--- if everything is bad, refresh the state
         if(!res)
            SetState(EnState.STATE_DISCONNECTED);
         //--- return the result
         return(res);
        }
      //+------------------------------------------------------------------+
      //| Sending ticks                                                    |
      //+------------------------------------------------------------------+
      bool TicksApply()
        {
         MTTick tick;
         bool   res=false;
         //--- checking time
         if(!TickPeriodCheck())
            return(true);
         //--- check using of file
         if(m_tick_filename==null)
            return(true);
         //--- analyze the string with a tick
         tick=TickParse(out res);
         if(res)
           {
            //--- sending data
            if(m_gateway!=null)
               res=m_gateway.SendTick(tick)==MTRetCode.MT_RET_OK;
           }
         else
            ExtLogger.Out(EnMTLogCode.MTLogErr,"tick parse failed '{0}'",m_buffer);
         //--- return the result
         return(res);
        }
      //+------------------------------------------------------------------+
      //| Checking the period of ticks                                     |
      //+------------------------------------------------------------------+
      bool TickPeriodCheck()
        {
         Int64 crnt_time=Environment.TickCount;
         //--- compare the current time and the time of the last reference
         if((crnt_time-m_tick_lasttime)<m_tick_period)
            return (false);
         //---
         m_tick_lasttime=crnt_time;
         return(true);
        }
      //+------------------------------------------------------------------+
      //| File stream read helper                                          |
      //+------------------------------------------------------------------+
      string GetStreamLine(StreamReader reader,string filename,out bool eof)
        {
         string res=null;
         //---
         eof=false;
         //--- check arguments
         if(reader==null || filename==null)
            return(null);
         //--- do not let exceptions get out here
         try
           {
            //--- get line 
            res=reader.ReadLine();
            //---
            if(res==null)
              {
               reader.DiscardBufferedData();
               reader.BaseStream.Seek(0, SeekOrigin.Begin);
               //---
               eof=true;
               res=reader.ReadLine();
              }
            //--- update incoming traffic
            if(m_gateway!=null && res!=null)
               m_gateway.StateTraffic((uint)res.Length,0);
           }
         catch(Exception ex)
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr,"reading '{0}' error [{1}]",filename,ex.Message);
           }
         //---
         m_buffer=res;
         //---
         return(res);
        }
      //+------------------------------------------------------------------+
      //| Parsing of ticks                                                 |
      //+------------------------------------------------------------------+
      MTTick TickParse(out bool res)
        {
         string   line  =null;
         string[] items =null;
         double   bid   =0;
         double   ask   =0;
         double   last  =0;
         ulong    volume=0;
         bool     is_eof;
         //---
         res = false;
         try
           {
            //--- get current line
            if((line=GetStreamLine(m_tick_file,m_tick_filename,out is_eof))!=null)
              {
               //--- and parse it
               items=line.Split(',');
               //--- 
               if((items.Length>5) && 
                  double.TryParse(items[2],m_double_style,m_double_format,out bid)  &&
                  double.TryParse(items[3],m_double_style,m_double_format,out ask)  &&
                  double.TryParse(items[4],m_double_style,m_double_format,out last) &&
                   ulong.TryParse(items[5],out volume))
                 {
                  //---
                  MTTick  tick=new MTTick();
                  tick.symbol =items[0];
                  tick.bank   =items[1];
                  tick.bid    =bid;
                  tick.ask    =ask;
                  tick.last   =last;
                  tick.volume =volume;
                  //--- tick datetime 
                  if(m_tick_start!=0)
                    {
                     //--- tick datetime
                     tick.datetime+=m_tick_start+m_tick_counter*m_tick_period;
                     //--- tick counter
                     m_tick_counter++;
                    }
                  //--- all ok
                  res=true;
                  return(tick);
                 }
              }
           }
         catch(Exception ex)
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr,"failed to create tick structures [{0}]",ex.Message);
           }
         //--- on errors return empty tick
         return(m_invalid_tick);
        }
      //+------------------------------------------------------------------+
      //| Sending books                                                    |
      //+------------------------------------------------------------------+
      bool BooksApply()
        {
         bool res=false;
         //--- checking time
         if(!BookPeriodCheck())
            return(true);
         //--- check using of file
         if(m_book_filename.Length<1)
            return(true);
         //--- analyze the string with a tick
         MTBook book=BookParse(out res);
         if(res)
           {
            //--- sending data
            if(m_gateway!=null)
               res=m_gateway.SendBook(book)==MTRetCode.MT_RET_OK;
           }
         else
            ExtLogger.Out(EnMTLogCode.MTLogErr,"book parse failed ({0})",m_buffer);
         //--- return the result
         return(res);
        }
      //+------------------------------------------------------------------+
      //| Checking the period of books                                     |
      //+------------------------------------------------------------------+
      bool BookPeriodCheck()
        {
         Int64 crnt_time=Environment.TickCount;
         //--- compare the current time and the time of the last reference
         if((crnt_time-m_book_lasttime)<m_book_period)
            return (false);
         m_book_lasttime=crnt_time;
         return(true);
        }
      //+------------------------------------------------------------------+
      //| Parsing of books                                                 |
      //+------------------------------------------------------------------+
      MTBook BookParse(out bool res)
        {
         MTBook          book;
         string          line   =null;
         bool            is_eof =false;
         int             sp     =0;    
         //---
         res=false;
         //---
         try
           {
            book            =new MTBook();
            book.items      =new MTBookItem[(int)MTBook.Constants.ITEMS_MAX];
            book.items_total=0;
            //---
            while(true)
              {
               if(is_eof && book.symbol==null)
                  break;
               //---
               if((line=GetStreamLine(m_book_file,m_book_filename,out is_eof))==null)
                  break;
               //--- is there `=` symbol present?
               if((sp=line.IndexOf('='))!=-1)
                 {
                  string name =line.Substring(0,sp).ToUpper();
                  string value=line.Substring(sp+1);
                  //---
                  switch(name)
                    {
                     //---
                     case "SYMBOL":
                        if(value.Length<1)
                          {
                           ExtLogger.Out(EnMTLogCode.MTLogErr,"failed to read book symbol [{0}]",line);
                           return(m_invalid_book);
                          }
                        book.symbol=value;
                        break;
                     //---
                     case "FLAGS":
                        if(!Enum.TryParse(value, out book.flags))
                          {
                           ExtLogger.Out(EnMTLogCode.MTLogErr,"failed to read book flags [{0}]",line);
                           return(m_invalid_book);
                          }
                        break;
                    }
                 }
               else
                 {
                  string[] items=line.Split(',');
                  //---
                  if(items==null || items.Length<1)
                    {
                     ExtLogger.Out(EnMTLogCode.MTLogErr,"failed to read book line [{0}]",line);
                     return(m_invalid_book);
                    }
                  //---
                  if(book.items_total>book.items.Length-1)
                    {
                     ExtLogger.Out(EnMTLogCode.MTLogErr,"book items overflow [{0}]",line);
                     return(m_invalid_book);
                    }
                  //--- if `END` token present --- all ok
                  if(items.Length==1 && items[0].ToUpper()=="END")
                    {
                     //--- book datetime 
                     if(m_book_start!=0)
                       {
                        //--- tick datetime
                        book.datetime+=m_book_start+m_book_counter*m_book_period;
                        //--- tick counter
                        m_book_counter++;
                       }
                     //--- everything is ok
                     res=true;
                     return(book);
                    }
                  //--- check parsed count
                  if(items.Length!=3)
                    {
                     ExtLogger.Out(EnMTLogCode.MTLogErr,"invalid book item [{0}]",line);
                     return(m_invalid_book);
                    }
                  //--- clear item type;
                  book.items[book.items_total].type = MTBookItem.EnBookItem.ItemReset;
                  //---
                  if(!Enum.TryParse(items[0],out book.items[book.items_total].type))
                    {
                     ExtLogger.Out(EnMTLogCode.MTLogErr,"invalid book item type [{0}]",line);
                     return(m_invalid_book);
                    }
                  //---
                  if(!double.TryParse(items[1],m_double_style,m_double_format,out book.items[book.items_total].price))
                    {
                     ExtLogger.Out(EnMTLogCode.MTLogErr,"invalid book item price [{0}] {1}",line,items[1]);
                     return(m_invalid_book);
                    }
                  //---
                  if(!long.TryParse(items[2],out book.items[book.items_total].volume))
                    {
                     ExtLogger.Out(EnMTLogCode.MTLogErr,"invalid book item volume [{0}] {1}",line,items[2]);
                     return(m_invalid_book);
                    }
                  //---
                  ++book.items_total;
                 }
              }
           }
         catch(Exception ex)
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr,"failed to create book structures [{0}]",ex.Message);
           }
         //--- on errors return invalid book
         return(m_invalid_book);
        }
      //+------------------------------------------------------------------+
      //| Sending news                                                     |
      //+------------------------------------------------------------------+
      bool NewsApply()
        {
         bool res=false;
         //--- checking time
         if(!NewsPeriodCheck())
            return(true);
         //--- check using of file
         if(m_news_filename==null)
            return(true);
         //--- analyze the string with a tick
         MTNews news=NewsParse(out res);
         if(res)
           {
            //--- sending data
            if(m_gateway!=null)
               res=m_gateway.SendNews(news)==MTRetCode.MT_RET_OK;
           }
         //--- return the result
         return(res);
        }
      //+------------------------------------------------------------------+
      //| Checking the period of news                                      |
      //+------------------------------------------------------------------+
      bool NewsPeriodCheck()
        {
         Int64 crnt_time=Environment.TickCount;
         //--- compare the current time and the time of the last reference
         if((crnt_time-m_news_lasttime)<m_news_period)
            return (false);
         //---
         m_news_lasttime=crnt_time;
         return(true);
        }
      //+------------------------------------------------------------------+
      //| Parsing of news                                                  |
      //+------------------------------------------------------------------+
      MTNews NewsParse(out bool res)
        {
         res=false;
         //---
         if(m_news==null)
           {
            try
              {
               using(StreamReader stream=new StreamReader(m_news_filename,Encoding.UTF8))
                 {
                  XmlSerializer serializer=new XmlSerializer(typeof(DataSource.MQNews[]),new XmlRootAttribute("MQNewsList"));
                  //---
                  using(XmlReader reader=XmlReader.Create(stream))
                    {
                     m_news=(DataSource.MQNews[])serializer.Deserialize(reader);
                     if(m_news.Length<1)
                       {
                        m_news=null;
                        ExtLogger.Out(EnMTLogCode.MTLogErr,"news file {0} reading error - no news found",m_news_filename);
                        return(m_invalid_news);
                       }
                     //---
                     if(m_gateway!=null)
                        m_gateway.StateTraffic((uint)Encoding.ASCII.GetBytes(reader.ReadOuterXml()).Length,0);
                    }
                 }
              }
            catch(Exception ex)
              {
               ExtLogger.Out(EnMTLogCode.MTLogErr,"news file {0} reading error [{1}]",m_news_filename,ex.Message);
               //--- attempt to get extended error info
               if(ex.InnerException!=null)
                  ExtLogger.Out(EnMTLogCode.MTLogErr,"{0}",ex.InnerException.Message);
              }
           }
         //---
         if(m_news!=null)
           {
            try
              {
               MQNews current=m_news[m_news_current++];
               //--- rotate news
               if(m_news_current>m_news.Length-1)
                  m_news_current=0;
               //---
               MTNews news=current.ToMTNews();
               res        =true;
               return(news);
              }
            catch(Exception ex)
              {
               ExtLogger.Out(EnMTLogCode.MTLogErr,"failed to create news structures [{0}]",ex.Message);
              }
           }
         //---
         return(m_invalid_news);
        }
      //+------------------------------------------------------------------+
      //| Getting parameters                                               |
      //+------------------------------------------------------------------+
      bool ParametersParse(CIMTConFeeder config)
        {
         CIMTConParam param=null;
         string       path =null;
         DateTime     dtime;
         //--- checking
         if(config==null)
            return(false);
         try
           {
            //--- get the path to the application
            path=AppDomain.CurrentDomain.BaseDirectory;
            //--- create parameter interface
            if((param=m_gateway.FeederParamCreate())==null)
               return(false);
            //--- go through all parameters
            for(uint i=0;i<config.ParameterTotal();i++)
              {
               //--- get a parameter by index
               if(config.ParameterNext(i,param)!=MTRetCode.MT_RET_OK)
                  break;
               //---
               switch(param.Name())
                 {
                  //--- analyze the parameter name, tick sending period
                  case "TickPeriod":
                     //--- get the parameter value
                     Int64.TryParse(param.Value(),out m_tick_period);
                     //--- check the parameter value
                     if(m_tick_period>TICKS_PERIOD_MAX)
                        m_tick_period=TICKS_PERIOD_MAX;
                     if(m_tick_period<TICKS_PERIOD_MIN)
                        m_tick_period=TICKS_PERIOD_MIN;
                     break;
                  //--- a text file with ticks
                  case "TickFile":
                     //--- form a path to the file
                     m_tick_filename=Path.Combine(path,param.Value());
                     break;
                  //--- ticks start time
                  case "TickStart":
                     if(DateTime.TryParse(param.Value(),out dtime))
                        m_tick_start=SMTTime.FromDateTime(dtime);
                     break;
                  //--- analyze the parameter name, tick sending period
                  case "BookPeriod":
                     //--- get the parameter value
                     Int64.TryParse(param.Value(),out m_book_period);
                     //--- check the parameter value
                     if(m_book_period>BOOKS_PERIOD_MAX)
                        m_book_period=BOOKS_PERIOD_MAX;
                     if(m_book_period<BOOKS_PERIOD_MIN)
                        m_book_period=BOOKS_PERIOD_MIN;
                     break;
                  //--- a text file with ticks
                  case "BookFile":
                     //--- form a path to the file
                     m_book_filename=Path.Combine(path,param.Value());
                     break;
                  //--- ticks start time
                  case "BookStart":
                     if(DateTime.TryParse(param.Value(), out dtime))
                        m_book_start=SMTTime.FromDateTime(dtime);
                     break;
                  //--- news sending period
                  case "NewsPeriod":
                     //--- get the parameter value
                     Int64.TryParse(param.Value(),out m_news_period);
                     //--- check the parameter value
                     if(m_news_period>NEWS_PERIOD_MAX)
                        m_news_period=NEWS_PERIOD_MAX;
                     if(m_news_period<NEWS_PERIOD_MIN)
                        m_news_period=NEWS_PERIOD_MIN;
                     break;
                  //--- a text file with news
                  case "NewsFile":
                     //--- form a path to the file
                     m_news_filename=Path.Combine(path,param.Value());
                     break;
                 }
              }
           }
         catch(Exception ex)
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr,"failed to parse parameters [{0}]",ex.Message);
           }
         //--- release the parameter interface
         if(param!=null)
            param.Dispose();
         //--- return the result
         return(m_tick_filename==null || m_book_filename==null || m_news_filename==null);
        }
     }
  }
//+------------------------------------------------------------------+
