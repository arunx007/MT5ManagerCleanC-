//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginReports.h"
#include "PluginInstance.h"
#include "..\Tools\Zip\ZipFile.h"
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CPluginReports::CPluginReports(void) : m_parent(NULL),m_api(NULL),m_workflag(false),m_time_report(0),m_time_sent(0),m_compact_process(false)
  {
   ZeroMemory(m_buffer,sizeof(m_buffer));
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CPluginReports::~CPluginReports(void)
  {
   Shutdown();
  }
//+------------------------------------------------------------------+
//| Copy context data and start thread                               |
//+------------------------------------------------------------------+
bool CPluginReports::Initialize(CPluginInstance *parent,IMTServerAPI *api,const PluginContext &context)
  {
   Shutdown();
//--- check
   if(!parent || !api)
      return(false);
//--- copy pointers and whole context struct
   m_parent =parent;
   m_api    =api;
   m_context=context;
//--- check times and bring warning
   if(m_context.send_time<m_context.report_time)
      Out(MTLogWarn,L"sending time must be greater then report generating time, please check your plugin settings");
//--- to start context
   m_workflag=true;
   if(!m_thread.Start(ThreadWrapper,this,STACK_SIZE_THREAD))
     {
      m_workflag=false;
      Out(MTLogErr,L"start report generator thread error [%u]",::GetLastError());
      return(false);
     }
//---
   Out(MTLogOK,L"report generator thread started");
   return(true);
  }
//+------------------------------------------------------------------+
//| Stop thread                                                      |
//+------------------------------------------------------------------+
void CPluginReports::Shutdown(void)
  {
//--- shutdown thread
   m_workflag=false;
   m_thread.Shutdown(INFINITE);
   Out(MTLogOK,L"report sender thread stopped");
   m_context.Clear();
   m_api=NULL;
   m_parent=NULL;
   m_time_report=0;
   m_time_sent=0;
  }
//+------------------------------------------------------------------+
//| Thread wrapper                                                   |
//+------------------------------------------------------------------+
uint32_t __stdcall CPluginReports::ThreadWrapper(void *param)
  {
//--- start main thread function
   CPluginReports *pThis=reinterpret_cast<CPluginReports*>(param);
   if(pThis)
      pThis->Thread();
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Thread function                                                  |
//+------------------------------------------------------------------+
__declspec(noinline) void CPluginReports::Thread(void)
  {
//--- check exit flag
   while(m_workflag)
     {
      //--- if we have not API - we have not current time, just sleep
      if(!m_api)
        {
         Sleep(THREAD_SLEEP);
         continue;
        }
      //--- get trade time
      int64_t trade_time=m_api->TimeCurrent();
      tm    ttm       ={};
      //--- prepare batch time
      SMTTime::ParseTime(trade_time,&ttm);
      SPluginHelpers::TimeToEST(ttm,m_context.server_timezone);
      int64_t now_time=SPluginHelpers::TimeToBatchDay(ttm,m_context.report_time,true);
      //--- check report time
      if(!m_time_report)
         m_time_report=now_time;
      if(m_time_report!=now_time)
        {
         SMTTime::ParseTime(m_time_report,&ttm);
         MakeReports(ttm);
         m_time_report=now_time;
         continue;
        }
      //--- prepare send time
      SMTTime::ParseTime(trade_time,&ttm);
      SPluginHelpers::TimeToEST(ttm,m_context.server_timezone);
      now_time=SPluginHelpers::TimeToBatchDay(ttm,m_context.send_time,true);
      //--- check send time
      if(!m_time_sent)
         m_time_sent=now_time;
      if(m_time_sent!=now_time)
        {
         SMTTime::ParseTime(m_time_sent,&ttm);
         SendReports(ttm);
         m_time_sent=now_time;
         continue;
        }
      //--- check compact
      if(ttm.tm_wday==0)
        {
         //--- already compacted?
         if(!m_compact_process)
           {
            if(m_parent)
               m_parent->CompactProcess();
            m_compact_process=true;
           }
        }
      else
         m_compact_process=false;
      //--- sleep
      Sleep(THREAD_SLEEP);
     }
  }
//+------------------------------------------------------------------+
//| Generating reports                                               |
//+------------------------------------------------------------------+
void CPluginReports::MakeReports(const tm &batch_ttm)
  {
//--- check directory
   CMTStrPath report_dir;
   ReportDirectory(report_dir,batch_ttm);
   CMTFile::DirectoryCreate(report_dir);
//--- format batch_date
   CMTStr32 batch_str;
   SPluginHelpers::FormatBatchDate(batch_str,batch_ttm);
//--- prepare from & to for day requests
   tm req_ttm=batch_ttm;
   req_ttm.tm_mday-=1;
   req_ttm.tm_sec -=1;
   SPluginHelpers::TimeToBatchDay(req_ttm,m_context.report_time,false);
   int64_t req_from=SPluginHelpers::TimeFromEST(req_ttm,m_context.server_timezone);
   req_ttm=batch_ttm;
   int64_t req_to=SPluginHelpers::TimeFromEST(req_ttm,m_context.server_timezone);
//--- log
   Out(MTLogOK,L"report generating started");
//--- generating
   MakeReportCustomer(batch_ttm,batch_str,req_from,req_to);
   MakeReportTick(batch_ttm,batch_str,req_from,req_to);
   MakeReportAdjustments(batch_ttm,batch_str,req_from,req_to);
   MakeReportEmpty(batch_ttm,REPORT_MANAGER,"BATCH_DATE|MANAGER_ID|NFA_ID|FDM_ID|MANAGER_NAME|COUNTRY_TYPE|START_DATE\r\n");
   MakeReportEmpty(batch_ttm,REPORT_CONCORDANCE,"BATCH_DATE|FDM_ID|MANAGER_ID|CUSTACCT_ID\r\n");
   MakeReportEmpty(batch_ttm,REPORT_MARKET_EVENTS,"FDM_ID|BATCH_DATE|MARKET_EVENT_TIME|PRODUCT_CODE|PRODUCT_STATE|EVENT_TEXT\r\n");
//--- check day file is closed
   if(m_parent)
      m_parent->CheckCloseDay(batch_ttm);
//--- check/create order book if is not exists
   MakeReportEmpty(batch_ttm,REPORT_ORDERBOOK,
      "ID|BATCH_DATE|TRANSACTION_DATETIME|ORDER_ID|FDM_ID|MKT_SEG_ID|"
      "PRODUCT_CODE|PRODUCT_CAT|CONTRACT_YEAR|CONTRACT_MONTH|"
      "CONTRACT_DAY|STRIKE|OPTION_TYPE|TRANSACTION_TYPE|ORDER_TYPE|"
      "VERB|BID_PRICE|ASK_PRICE|QUANTITY|REMAINING_QTY|PRICE|STOP_PRICE|"
      "STOP_PRODUCT_CODE|TRAIL_AMT|LIMIT_OFFSET|DURATION|EXPIRY_DATE|"
      "ORDER_ORIGIN|MANAGER_ID|CUSTACCT_ID|SERVER_ID|CUST_GROUP|"
      "LINKED_ORDER_ID|LINK_REASON|OPEN_CLOSE\r\n");
//--- check/create trade exists if is not exists
   MakeReportEmpty(batch_ttm,REPORT_TRADE,
      "ID|BATCH_DATE|TRADE_ID|ORDER_ID|FDM_ID|MKT_SEG_ID|"
      "TIME_TRADEMATCH|PRODUCT_CAT|PRODUCT_CODE|CONTRACT_YEAR|CONTRACT_MONTH|"
      "CONTRACT_DAY|STRIKE|VERB|BID_PRICE|ASK_PRICE|QUANTITY|"
      "REMAINING_QTY|FILL_PRICE|CONTRA_FILL_PRICE|"
      "SERVER_ID|IMPLIED_VOLATILITY|IB_REBATE|COMMISSION\r\n");
//--- finished
   Out(MTLogOK,L"report generating finished");
//--- compress reports
   LPCWSTR reports[]={ REPORT_ORDERBOOK,REPORT_TRADE,REPORT_CUSTOMER,REPORT_MANAGER,REPORT_CONCORDANCE,REPORT_ADJUSTMENTS,REPORT_MARKET_EVENTS,REPORT_TICK };
   for(int32_t i=0;i<_countof(reports);i++)
      MakeReportCompress(batch_ttm,reports[i],report_dir.Str());
  }
//+------------------------------------------------------------------+
//| Zip file with mt5srvupdater64.exe                                |
//+------------------------------------------------------------------+
bool CPluginReports::MakeReportCompress(const tm &batch_ttm,LPCWSTR name,LPCWSTR report_dir)
  {
   if(!name || !report_dir)
      return(false);
//--- prepare paths
   CMTStr64 date;
   SPluginHelpers::FormatFileDate(date,batch_ttm);
   CMTStrPath src,src_name,dst;
   src.Format(L"%s\\%s_%s.txt",report_dir,name,date.Str());
   src_name.Format(L"%s_%s.txt",name,date.Str());
   dst.Format(L"%s\\%s_%s.zip",report_dir,name,date.Str());
//--- check file exist
   if(GetFileAttributesW(src.Str())==INVALID_FILE_ATTRIBUTES)
     {
      Out(MTLogWarn,L"file '%s_%s.txt' doesn't exist",name,date.Str());
      return(false);
     }
//--- logger
   Out(MTLogOK,L"compress '%s_%s.txt' started",name,date.Str());
//---
   CZipFile zip_file;
   if(!zip_file.Open(dst.Str()) || !zip_file.AddFile(src.Str(),src_name.Str()))
     {
      Out(MTLogErr,L"compress '%s_%s.txt' failed",name,date.Str());
      zip_file.Close();
      DeleteFileW(dst.Str());
      return(false);
     }
   zip_file.Close();
   DeleteFileW(src.Str());
   Out(MTLogOK,L"compress '%s_%s.txt' finished",name,date.Str());
   return(true);
  }
//+------------------------------------------------------------------+
//| Create and open report file, keep it opened                      |
//+------------------------------------------------------------------+
bool CPluginReports::FileReportOpen(CMTFile &file,LPCWSTR path,LPCSTR header)
  {
//--- check file already exists
   if(GetFileAttributesW(path)!=INVALID_FILE_ATTRIBUTES)
     {
      Out(MTLogWarn,L"report file already exists [%s]",path);
      return(false);
     }
//--- create file
   if(!file.OpenWrite(path))
     {
      Out(MTLogErr,L"create report file error [%s][%u]",path,GetLastError());
      return(false);
     }
//--- write header
   if(!file.Write(header,(DWORD)strlen(header)))
     {
      Out(MTLogErr,L"write report header error [%s][%u]",path,::GetLastError());
      return(false);
     }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Put wide-character line to ANSI file                             |
//+------------------------------------------------------------------+
bool CPluginReports::FileReportWrite(CMTFile &file,LPCWSTR line)
  {
//--- convert to ANSI
   CMTStr::Copy(m_buffer,sizeof(m_buffer),line);
   m_buffer[sizeof(m_buffer)-1]=0;
//--- write to file
   if(!file.Write(m_buffer,(DWORD)strlen(m_buffer)))
     {
      Out(MTLogErr,L"write report error [%u]",GetLastError());
      return(false);
     }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Write report file with header only                               |
//+------------------------------------------------------------------+
bool CPluginReports::MakeReportEmpty(const tm &ttm,LPCWSTR name,LPCSTR header)
  {
   CMTStrPath path;
   ReportPath(path,ttm,name);
//--- check file already exists
   if(GetFileAttributesW(path.Str())!=INVALID_FILE_ATTRIBUTES)
      return(true);
//--- write header only
   CMTFile file;
   return(FileReportOpen(file,path.Str(),header));
  }
//+------------------------------------------------------------------+
//| Find all users, export filtered by group mask                    |
//+------------------------------------------------------------------+
void CPluginReports::MakeReportCustomer(const tm &batch_ttm,CMTStr &batch_str,const int64_t req_from,const int64_t req_to)
  {
//--- report header
   static const LPCSTR header="BATCH_DATE|CUSTACCT_ID|FDM_ID|CUST_NAME|CUST_BIRTH_DATE|CUST_TYPE|CUST_GROUP|COUNTRY_TYPE|NET_LIQUIDATING_VALUE|ACCT_OPEN_DATE\r\n";
//--- make and open file
   CMTFile    file;
   CMTStrPath path;
   if(!FileReportOpen(file,ReportPath(path,batch_ttm,REPORT_CUSTOMER),header))
      return;
//--- allocations
   IMTConGroup      *group    =NULL;
   IMTUser          *user     =NULL;
   IMTAccount       *account  =NULL;
   IMTClient        *client   =NULL;
   IMTPositionArray *positions=NULL;
   IMTOrderArray    *orders   =NULL;
   IMTDealArray     *deals    =NULL;
   if((group    =m_api->GroupCreate())        !=NULL &&
      (user     =m_api->UserCreate())         !=NULL &&
      (account  =m_api->UserCreateAccount())  !=NULL &&
      (client   =m_api->ClientCreate())       !=NULL &&
      (positions=m_api->PositionCreateArray())!=NULL &&
      (orders   =m_api->OrderCreateArray())   !=NULL &&
      (deals    =m_api->DealCreateArray())    !=NULL)
     {
      CMTStr64 sregdate,sbirthdate,sequity;
      //--- iterate over all groups
      for(uint32_t pos=0;m_api->GroupNext(pos,group)==MT_RET_OK;pos++)
        {
         //--- check group
         if(!CMTStr::CheckGroupMask(m_context.groups.Str(),group->Group()))
            continue;
         //--- get user list
         uint32_t    logins_total=0;
         uint64_t *logins      =NULL;
         if(m_api->UserLogins(group->Group(),logins,logins_total)==MT_RET_OK)
           {
            //--- iterate over all users
            for(uint32_t i=0;i<logins_total;i++)
              {
               //--- get user
               if(m_api->UserGet(logins[i],user)==MT_RET_OK)
                 {
                  //--- check customer for zero or negative balance
                  double equity=0;
                  if(!CustomerCheck(user,account,positions,orders,deals,req_from,req_to,equity))
                     continue;
                  //--- format registration date
                  tm ttm={};
                  SMTTime::ParseTime(user->Registration(),&ttm);
                  SPluginHelpers::TimeToEST(ttm,m_context.server_timezone);
                  SPluginHelpers::FormatBatchDate(sregdate,ttm);
                  //--- get client
                  sbirthdate.Clear();
                  const uint64_t clientid=user->ClientID();
                  if(clientid && m_api->ClientGet(clientid,client)==MT_RET_OK)
                    {
                     //--- format customer birth date
                     const int64_t birth_date=client->PersonBirthDate();
                     if(birth_date)
                       {
                        SYSTEMTIME st={};
                        if(FileTimeToSystemTime((const FILETIME*)&birth_date,&st))
                           SPluginHelpers::FormatBatchDate(sbirthdate,st);
                       }
                    }
                  //--- format equity
                  SMTFormat::FormatDouble(sequity,equity,group->CurrencyDigits());
                  //--- format 
                  m_buffer_line.Format(L"%s|"         // BATCH_DATE
                                        "%d|"         // CUSTACCT_ID
                                        "%s|"         // FDM_ID
                                        "%s|"         // CUST_NAME
                                        "%s|"         // CUST_BIRTH_DATE
                                        "RETAIL|"     // CUST_TYPE
                                        "%s|"         // CUST_GROUP
                                        "%s|"         // COUNTRY_TYPE
                                        "%s|"         // NET_LIQUIDATING_VALUE
                                        "%s\r\n",     // ACCT_OPEN_DATE
                     batch_str.Str(),
                     user->Login(),
                     m_context.fdm_id.Str(),
                     user->Name(),
                     sbirthdate.Str(),
                     user->Group(),
                     CustomerCountryType(user->Country()),
                     sequity.Str(),
                     sregdate.Str());
                  //--- write
                  FileReportWrite(file,m_buffer_line.Str());
                 }
              }
           }
         //--- cleanup
         if(logins)
            m_api->Free(logins);
        }
      //--- log
      Out(MTLogOK,L"customer report generated");
     }
   else
      Out(MTLogErr,L"not enough memory for customer report");
//--- release all interfaces
   if(group)     group->Release();
   if(user)      user->Release();
   if(account)   account->Release();
   if(client)    client->Release();
   if(positions) positions->Release();
   if(orders)    orders->Release();
   if(deals)     deals->Release();
  }
//+------------------------------------------------------------------+
//| Find all ticks for the day, export filtered by symbol mask       |
//+------------------------------------------------------------------+
void CPluginReports::MakeReportTick(const tm &batch_ttm,CMTStr &batch_str,const int64_t req_from,const int64_t req_to)
  {
//--- report header
   static const LPCSTR header="FDM_ID|BATCH_DATE|PRODUCT_CODE|QUOTE_DATETIME|BID_PRICE|ASK_PRICE\r\n";
//--- make and open file
   CMTFile    file;
   CMTStrPath path;
   if(!FileReportOpen(file,ReportPath(path,batch_ttm,REPORT_TICK),header))
      return;
//--- allocations
   IMTConSymbol *symbol=NULL;
   if((symbol=m_api->SymbolCreate())!=NULL)
     {
      CMTStr64 sdatetime,sbid,sask;
      //--- iterate over all symbols
      for(uint32_t pos=0;m_api->SymbolNext(pos,symbol)==MT_RET_OK && m_workflag;pos++)
        {
         //--- need export symbol?
         if(!CMTStr::CheckGroupMask(m_context.symbols.Str(),symbol->Symbol()))
            continue;
         //--- get digits
         uint32_t digits=symbol->Digits();
         //--- get ticks;
         MTTickShort *ticks      =NULL;
         uint32_t     ticks_total=0;
         if(m_api->TickHistoryGet(symbol->Symbol(),req_from,req_to,ticks,ticks_total)==MT_RET_OK && ticks!=NULL)
           {
            //--- iterate over all ticks
            for(uint32_t i=0;i<ticks_total;i++)
              {
               //--- prepare
               const MTTickShort &tick=ticks[i];
               tm                 tick_ttm={};
               //--- format time
               SMTTime::ParseTime(tick.datetime,&tick_ttm);
               SPluginHelpers::TimeToEST(tick_ttm,m_context.server_timezone);
               SPluginHelpers::FormatStampDatetime(sdatetime,tick_ttm);
               //--- format bid & ask
               SMTFormat::FormatDouble(sbid,tick.bid,digits);
               SMTFormat::FormatDouble(sask,tick.ask,digits);
               //--- format
               m_buffer_line.Format(L"%s|"        // FDM_ID
                                     "%s|"        // BATCH_DATE
                                     "%s|"        // PRODUCT_CODE
                                     "%s|"        // QUOTE_DATETIME
                                     "%s|"        // BID_PRICE
                                     "%s\r\n",    // ASK_PRICE
                  m_context.fdm_id.Str(),
                  batch_str.Str(),
                  symbol->Symbol(),
                  sdatetime.Str(),
                  sbid.Str(),
                  sask.Str());
               //--- write
               FileReportWrite(file,m_buffer_line.Str());
              }
           }
         //--- cleanup
         if(ticks)
            m_api->Free(ticks);
        }
      //--- log
      Out(MTLogOK,L"tick report generated");
     }
   else
      Out(MTLogErr,L"not enough memory for tick report");
//--- release
   if(symbol)
      symbol->Release();
  }
//+------------------------------------------------------------------+
//| Find all users, export filtered by group mask                    |
//+------------------------------------------------------------------+
void CPluginReports::MakeReportAdjustments(const tm &batch_ttm,CMTStr &batch_str,const int64_t req_from,const int64_t req_to)
  {
//--- report header
   static const LPCSTR header="BATCH_DATE|FDM_ID|TRADE_ID|TRADE_DATE|ADJUSTMENT_DATETIME|ADJUSTMENT_TYPE|PRICE|QUANTITY|CASH_AMOUNT|NOTE\r\n";
//--- make and open file
   CMTFile    file;
   CMTStrPath path;
   if(!FileReportOpen(file,ReportPath(path,batch_ttm,REPORT_ADJUSTMENTS),header))
      return;
//--- allocations
   IMTDeal  *deal=NULL;
   IMTUser  *user=NULL;
   if((deal=m_api->DealCreate())!=NULL && (user=m_api->UserCreate())!=NULL)
     {
      CMTStr64 tmp,sadjustment_date,strade_date,samount,sprice;
      //--- request logs
      MTLogRecord *records=NULL;
      uint32_t     records_total=0;
      if(m_api->LoggerRequest(MTLogModeStd,MTLogTypeTrade,req_from,req_to,L"' updated: Deal - ",records,records_total)==MT_RET_OK)
        {
         //--- parse logs
         for(uint32_t i=0;i<records_total;i++)
           {
            LPCWSTR message=records[i].message;
            //--- deal id
            int32_t pos=CMTStr::Find(message,L"': deal #");
            if(pos<0)
               continue;
            message+=pos+9;
            uint64_t deal_id=_wcstoui64(message,NULL,10);
            //--- get deal
            if(m_api->DealGet(deal_id,deal)!=MT_RET_OK)
               continue;
            //--- updated type
            pos=CMTStr::Find(message+1,L"' updated: Deal - ");
            if(pos<0)
               continue;
            message+=pos+18;
            pos=CMTStr::Find(message+1,L": ");
            if(pos<0)
               continue;
            tmp.Clear();
            tmp.Assign(message,pos+1);
            tmp.TrimSpaces();
            //--- checks
            if(tmp.Empty())
               continue;
            //--- parse old and new value
            message+=pos+2;
            double modified_old=_wtof(message);
            pos=CMTStr::Find(message+1,L" -> ");
            if(pos<0)
               continue;
            message+=pos+4;
            double modified_new=_wtof(message);
            //--- format ADJUSTMENT_DATETIME
            tm record_ttm={};
            SMTTime::ParseTime(records[i].datetime,&record_ttm);
            SPluginHelpers::TimeToEST(record_ttm,m_context.server_timezone);
            SPluginHelpers::FormatStampDatetime(sadjustment_date,record_ttm);
            //--- format TRADE_DATE
            tm trade_ttm={};
            SMTTime::ParseTime(deal->Time(),&trade_ttm);
            SPluginHelpers::TimeToEST(record_ttm,m_context.server_timezone);
            SPluginHelpers::FormatStampDatetime(strade_date,trade_ttm);
            //--- Price
            if(tmp.CompareNoCase(L"Price")==0)
              {
               //--- format amount
               SMTFormat::FormatDouble(sprice,modified_new,deal->Digits());
               //--- format line
               m_buffer_line.Format(L"%s|"     // BATCH_DATE
                           L"%s|"     // FDM_ID
                           L"%I64u|"  // TRADE_ID
                           L"%s|"     // TRADE_DATE
                           L"%s|"     // ADJUSTMENT_DATETIME
                           L"PRICE|"  // ADJUSTMENT_TYPE
                           L"%s|"     // PRICE
                           L"|"       // QUANTITY
                           L"|"       // CASH_AMOUNT
                           L"%s\r\n",// NOTE
                  batch_str.Str(),
                  m_context.fdm_id.Str(),
                  deal_id,
                  strade_date.Str(),
                  sadjustment_date.Str(),
                  sprice.Str(),
                  AdjustmentNote(deal));
               //--- write
               FileReportWrite(file,m_buffer_line.Str());
               continue;
              }
            //--- Volume
            if(tmp.CompareNoCase(L"Volume")==0)
              {
               //--- format line
               m_buffer_line.Format(L"%s|"     // BATCH_DATE
                                    L"%s|"     // FDM_ID
                                    L"%I64u|"  // TRADE_ID
                                    L"%s|"     // TRADE_DATE
                                    L"%s|"     // ADJUSTMENT_DATETIME
                                    L"QTY|"    // ADJUSTMENT_TYPE
                                    L"|"       // PRICE
                                    L"%I64u|"  // QUANTITY
                                    L"|"       // CASH_AMOUNT
                                    L"%s\r\n", // NOTE
                  batch_str.Str(),
                  m_context.fdm_id.Str(),
                  deal_id,
                  strade_date.Str(),
                  sadjustment_date.Str(),
                  uint64_t(SMTMath::VolumeExtToSize(deal->Volume(),deal->ContractSize())),
                  AdjustmentNote(deal));
               //--- write
               FileReportWrite(file,m_buffer_line.Str());
               continue;
              }
            //--- Volume
            if(tmp.CompareNoCase(L"Profit")==0)
              {
               //--- format amount
               SMTFormat::FormatDouble(samount,
                  SMTMath::PriceNormalize(modified_new-modified_old,deal->DigitsCurrency()),deal->DigitsCurrency());
               //--- format line
               m_buffer_line.Format(L"%s|"     // BATCH_DATE
                                    L"%s|"     // FDM_ID
                                    L"%I64u|"  // TRADE_ID
                                    L"%s|"     // TRADE_DATE
                                    L"%s|"     // ADJUSTMENT_DATETIME
                                    L"CASH|"   // ADJUSTMENT_TYPE
                                    L"|"       // PRICE
                                    L"|"       // QUANTITY
                                    L"%s|"     // CASH_AMOUNT
                                    L"%s\r\n", // NOTE
                  batch_str.Str(),
                  m_context.fdm_id.Str(),
                  deal_id,
                  strade_date.Str(),
                  sadjustment_date.Str(),
                  samount.Str(),
                  AdjustmentNote(deal));
               //--- write
               FileReportWrite(file,m_buffer_line.Str());
               continue;
              }
           }
         //--- cleanup
         if(records)
            m_api->Free(records);
        }
      //--- log
      Out(MTLogOK,L"adjustments report generated");
     }
   else
      Out(MTLogErr,L"not enough memory for adjustments report");
//--- cleanup
   if(deal)
      deal->Release();
   if(user)
      user->Release();
  }
//+------------------------------------------------------------------+
//| Note for adjustments                                             |
//+------------------------------------------------------------------+
LPCWSTR CPluginReports::AdjustmentNote(const IMTDeal *deal)
  {
//--- checks
   if(deal)
     {
      uint32_t flags=deal->ModificationFlags();
      if((flags&IMTDeal::MODIFY_FLAGS_ADMIN)!=0 || (flags&IMTDeal::MODIFY_FLAGS_API_ADMIN)!=0)
         return(L"Modified by administrator");
      if((flags&IMTDeal::MODIFY_FLAGS_MANAGER)!=0 || (flags&IMTDeal::MODIFY_FLAGS_API_MANAGER)!=0)
         return(L"Modified by manager");
     }
//--- nothing to report
   return(L"");
  }
//+------------------------------------------------------------------+
//| Convert country name to NFA's DOM/FOR                            |
//+------------------------------------------------------------------+
LPCWSTR CPluginReports::CustomerCountryType(LPCWSTR country) const
  {
//--- проверки
   if(!country)
      return L"";
//--- check country is USA
   return(CMTStr::CompareNoCase(country,L"USA")==0 || CMTStr::CompareNoCase(country,L"United States")==0) ? L"DOM" : L"FOR";
  }
//+------------------------------------------------------------------+
//| Return false if user is 'dead' (haven't to be exported)          |
//| If any errors occurred during processing - user is not 'dead'    |
//+------------------------------------------------------------------+
bool CPluginReports::CustomerCheck(IMTUser *user,IMTAccount *account,IMTPositionArray *positions,IMTOrderArray *orders,IMTDealArray *deals,const int64_t from,const int64_t to,double &equity)
  {
//--- checks
   if(!user || !account || !positions || !orders || !deals || !m_api)
      return(false);
//--- default equity
   equity=user->Balance()+user->Credit();
//--- get account
   if(m_api->UserAccountGet(user->Login(),account)==MT_RET_OK)
     {
      //--- actual equity
      equity=account->Equity();
      //--- check equity and margin
      if(account->Equity()<=0 && account->Margin()==0)
        {
         //--- check positions
         if(m_api->PositionGet(user->Login(),positions)!=MT_RET_OK || positions->Total()>0)
            return(true);
         //--- check orders
         if(m_api->OrderGet(user->Login(),orders)!=MT_RET_OK || orders->Total()>0)
            return(true);
         //--- check order history
         if(m_api->HistoryGet(from,to,user->Login(),orders)!=MT_RET_OK || orders->Total()>0)
            return(true);
         //--- check deal history
         if(m_api->DealGet(from,to,user->Login(),deals)!=MT_RET_OK || deals->Total()>0)
            return(true);
         //--- no reporting
         return(false);
        }
     }
//--- true by default
   return(true);
  }
//+------------------------------------------------------------------+
//| Put files to the SFTP server using PUTTY utility.                |
//+------------------------------------------------------------------+
bool CPluginReports::SendReports(const tm &batch_ttm)
  {
//--- logger
   Out(MTLogOK,L"transfer started");
//--- check parameters
   if(m_context.sftp_address.Empty() || m_context.sftp_login.Empty())
     {
      Out(MTLogErr,L"SFTP address or login is empty");
      return(false);
     }
//--- prepare pscp.exe
   CMTStrPath putty;
   putty.Format(L"%s\\Reports\\NFA.Reports\\pscp.exe",m_context.server_path.Str());
   if(!SendPuttyExtract(putty.Str()))
      return(false);
//--- prepare paths
   CMTStrPath report_dir;
   ReportDirectory(report_dir,batch_ttm);
   CMTStr64 file_date;
   SPluginHelpers::FormatFileDate(file_date,batch_ttm);
//---
   LPCWSTR reports[]={ REPORT_ORDERBOOK,REPORT_TRADE,REPORT_CUSTOMER,REPORT_MANAGER,REPORT_CONCORDANCE,REPORT_ADJUSTMENTS,REPORT_MARKET_EVENTS,REPORT_TICK };
   bool res=true;
   for(int32_t i=0;i<_countof(reports);i++)
     {
      //--- process each report
      if(!SendPuttyProcess(report_dir.Str(),reports[i],file_date.Str(),putty.Str()))
         res=false;
     }
//--- cleanup
   DeleteFileW(putty.Str());
//--- logger
   Out(MTLogOK,L"transfer finished");
   return(true);
  }
//+------------------------------------------------------------------+
//| Extract pscp.exe from resources                                  |
//+------------------------------------------------------------------+
bool CPluginReports::SendPuttyExtract(LPCWSTR path)
  {
   if(!path)
      return(false);
//--- find resource
   HRSRC hsrc;
   if((hsrc=FindResourceW(ExtModule,MAKEINTRESOURCE(IDR_FILES_PSCP),L"FILES"))==NULL)
     {
      Out(MTLogErr,L"pscp.exe resource doesn't found");
      return(false);
     }
//--- load and write resource to file 
   CMTFile file;
   if(!file.OpenWrite(path))
     {
      Out(MTLogErr,L"error creating pscp.exe file");
      return(false);
     }
//--- load and take resources
   DWORD   size;
   HGLOBAL handle;
   if((size=SizeofResource(ExtModule,hsrc))<0 || (handle=LoadResource(ExtModule,hsrc))==NULL)
     {
      Out(MTLogErr,L"error loading pscp.exe from resource");
      file.Close();
      DeleteFileW(path);
      return(false);
     }
//--- lock and write
   LPCVOID data;
   if((data=LockResource(handle))==NULL || file.Write(data,size)!=size)
     {
      Out(MTLogErr,L"error writing pscp.exe file");
      file.Close();
      DeleteFileW(path);
      FreeResource(handle);
      return(false);
     }
   file.Close();
   FreeResource(handle);
   return(true);
  }
//+------------------------------------------------------------------+
//| Extract pscp.exe from resources                                  |
//+------------------------------------------------------------------+
bool CPluginReports::SendPuttyProcess(LPCWSTR report_dir,LPCWSTR report_name,LPCWSTR file_date,LPCWSTR putty_path)
  {
//--- checks
   if(!report_dir || !report_name || !file_date || !putty_path)
      return(false);
//--- prepare paths
   CMTStrPath report_zip;
   report_zip.Format(L"%s\\%s_%s.zip",report_dir,report_name,file_date);
//--- logger
   Out(MTLogOK,L"transfer '%s_%s.zip' started",report_name,file_date);
//--- check file
   if(GetFileAttributesW(report_zip.Str())==INVALID_FILE_ATTRIBUTES)
     {
      Out(MTLogWarn,L"file '%s_%s.zip' doesn't exist",report_name,file_date);
      return(true);
     }
   CMTStrPath in_path;
   in_path.Format(L"%s\\sendftp.in",report_dir);
   CMTFile file;
   if(!file.OpenWrite(in_path.Str()) || file.Write("y\r\n",3)!=3)
     {
      Out(MTLogAtt,L"write 'sendftp.in' error");
      file.Close();
      DeleteFileW(in_path.Str());
      return(false);
     }
   file.Close();
//--- prepare port
   CMTStr64 param_port;
   if(!m_context.sftp_port.Empty())
      param_port.Format(L"-P \"%s\"",m_context.sftp_port.Str());
//--- key exist?
   char bat_command[1024];
   int32_t  bat_command_len=0;
   if(m_context.sftp_key_path.Empty())
     {
      //--- without key
      bat_command_len=sprintf_s(bat_command,"\"%S\" -l \"%S\" -pw \"%S\" %S \"%S\" \"%S:%S_%S.zip\" < \"%S\"",
                           putty_path,m_context.sftp_login.Str(),m_context.sftp_password.Str(),
                           param_port.Str(),report_zip.Str(),
                           m_context.sftp_address.Str(),report_name,file_date,in_path.Str());
     }
   else
     {
      //--- with key
      bat_command_len=sprintf_s(bat_command,"\"%S\" -l \"%S\" -pw \"%S\" %S -i \"%S\" \"%S\" \"%S:%S_%S.zip\" < \"%S\"",
                           putty_path,m_context.sftp_login.Str(),m_context.sftp_password.Str(),
                           param_port.Str(),m_context.sftp_key_path.Str(),report_zip.Str(),
                           m_context.sftp_address.Str(),report_name,file_date,in_path.Str());
     }
//--- формируем путь к входным данным
   CMTStrPath bat_path;
   bat_path.Format(L"%s\\sendftp.bat",report_dir);
   if(!file.OpenWrite(bat_path.Str()) || file.Write(bat_command,bat_command_len)!=(DWORD)bat_command_len)
     {
      Out(MTLogAtt,L"write 'sendftp.bat' failed");
      file.Close();
      DeleteFileW(bat_path.Str());
      return(false);
     }
   file.Close();
//--- start process
   CMTProcess process;
   if(!process.Start(bat_path,CREATE_NO_WINDOW,false))
     {
      Out(MTLogErr,L"start 'pscp.exe' error");
      return(false);
     }
//--- wait putty
   if(!process.Wait(PROCESS_TIMEOUT_SEND) || process.ExitCode())
     {
      process.Terminate();
      DeleteFileW(bat_path.Str());
      DeleteFileW(in_path.Str());
      Out(MTLogErr,L"transfer error [please check login,password and private key]");
      return(false);
     }
//--- cleanup
   DeleteFileW(bat_path.Str());
   DeleteFileW(in_path.Str());
//--- logger
   Out(MTLogOK,L"transfer '%s_%s.zip' finished",report_name,file_date);
   return(true);
  }
//+------------------------------------------------------------------+
//| Report file path                                                 |
//+------------------------------------------------------------------+
LPCWSTR CPluginReports::ReportPath(CMTStr &str,const tm &ttm,LPCWSTR name) const
  {
   CMTStr32 date;
   SPluginHelpers::FormatFileDate(date,ttm);
   str.Format(L"%s\\Reports\\NFA.Reports\\%s\\%s\\%s_%s.txt",m_context.server_path.Str(),m_context.base_directory.Str(),date.Str(),name,date.Str());
   return str.Str();
  }
//+------------------------------------------------------------------+
//| Report directory                                                 |
//+------------------------------------------------------------------+
LPCWSTR CPluginReports::ReportDirectory(CMTStr &str,const tm &ttm) const
  {
   CMTStr32 date;
   str.Format(L"%s\\Reports\\NFA.Reports\\%s\\%s\\",m_context.server_path.Str(),m_context.base_directory.Str(),date.Str(),SPluginHelpers::FormatFileDate(date,ttm));
   return(str.Str());
  }
//+------------------------------------------------------------------+
//| Writer log line safe                                             |
//+------------------------------------------------------------------+
void CPluginReports::Out(uint32_t code,LPCWSTR msg,...)
  {
   if(m_api)
     {
      wchar_t buffer[1024];
      va_list args;
      //--- prepare
      va_start(args,msg);
      _vsnwprintf_s(buffer,_countof(buffer),_TRUNCATE,msg,args);
      va_end(args);
      //--- out
      m_api->LoggerOut(code,L"Reports: %s",buffer);
     }
  }
//+------------------------------------------------------------------+
