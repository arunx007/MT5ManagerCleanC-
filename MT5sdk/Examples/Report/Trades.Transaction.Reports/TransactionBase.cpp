//+------------------------------------------------------------------+
//|                          MetaTrader 5 Trades.Transaction.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "TransactionBase.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CTransactionBase::CTransactionBase(void)
   : m_api(NULL),
     m_file_size(0),
     m_timestamp(0),
     m_income_event(NULL),
     m_thread_exit(NULL),
     m_standby_event(NULL)
  {
   m_path.Clear();
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CTransactionBase::~CTransactionBase(void)
  {
   Shutdown();
   Close();
   m_path.Clear();
  }
//+------------------------------------------------------------------+
//| Open the file to read/write                                      |
//+------------------------------------------------------------------+
bool CTransactionBase::OpenWrite(const int64_t timestamp)
  {
//--- firstly close
   Close();
//--- checks
   if(timestamp<=0)
     {
      //--- log error
      if(m_api)
         m_api->LoggerOut(MTLogErr,L"failed to open base file for write: invalid timestamp=%I64d",timestamp);
      return(false);
     }
//--- already opened?
   if(m_file.IsOpen())
     {
      //--- this file?
      if(SMTTime::DayBegin(timestamp)==m_timestamp)
         return(true);
      //--- close old file
      Close();
     }
//--- get file name
   CMTStrPath name;
   name.Format(L"%04u%02u%02u.dat",SMTTime::Year(timestamp),SMTTime::Month(timestamp),SMTTime::Day(timestamp));
//--- get file full path
   CMTStrPath path=m_path;
   path.Append(name);
//--- open the file
   if(!m_file.Open(path.Str(),GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,OPEN_ALWAYS))
     {
      //--- log error
      if(m_api)
         m_api->LoggerOut(MTLogErr,L"failed to open base file '%s' for write, last error=%u",path.Str(),GetLastError());
      return(false);
     }
//--- get and check the base header
   if(!CheckHeader())
     {
      m_file.Close();
      //--- maybe this is an old version?
      if(!ConvertBase(timestamp))
        {
         //--- if the header is wrong - rewrite it
         if(!m_file.Open(path.Str(),GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,CREATE_ALWAYS) || !WriteHeader())
           {
            //--- log error
            if(m_api)
               m_api->LoggerOut(MTLogErr,L"failed to recreate base file '%s', last error=%u",path.Str(),GetLastError());
            //--- close file
            m_file.Close();
            return(false);
           }
        }
      else
        {
         //--- reopen the file
         if(!m_file.Open(path.Str(),GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,OPEN_ALWAYS) || !CheckHeader())
           {
            //--- log error
            if(m_api)
               m_api->LoggerOut(MTLogErr,L"failed to reopen base file '%s' for write, last error=%u",path.Str(),GetLastError());
            //--- close file
            m_file.Close();
            return(false);
           }
        }
     }
   else
     {
      if(!SeekLastRecord())
        {
         //--- log error
         if(m_api)
            m_api->LoggerOut(MTLogErr,L"failed to seek last record in base file '%s', last error=%u",path.Str(),GetLastError());
         //--- close file
         m_file.Close();
         return(false);
        }
     }
//--- set real size of the file
   m_file_size=m_file.CurrPos();
//--- expand the file
   if((m_file.Size()-m_file_size)<FILE_DELTA_RESIZE)
     {
      m_file.ChangeSize(m_file.Size()+FILE_STEP_SIZE);
      m_file.Seek(m_file_size,FILE_BEGIN);
     }
//--- set timestamp of the file
   m_timestamp=SMTTime::DayBegin(timestamp);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Open the file by timestamp                                       |
//+------------------------------------------------------------------+
bool CTransactionBase::OpenRead(const int64_t timestamp)
  {
//--- firstly close
   Close();
//--- checks
   if(timestamp<=0)
     {
      //--- log error
      if(m_api)
         m_api->LoggerOut(MTLogErr,L"failed to open base file for read: invalid timestamp=%I64d",timestamp);
      return(false);
     }
//--- get name of the file
   int64_t day_begin=SMTTime::DayBegin(timestamp);
   CMTStrPath name;
   name.Format(L"%04u%02u%02u.dat",SMTTime::Year(day_begin),SMTTime::Month(day_begin),SMTTime::Day(day_begin));
   CMTStrPath path=m_path;
   path.Append(name);
//--- open the file
   if(!m_file.Open(path.Str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,OPEN_EXISTING))
     {
      //--- log error
      if(m_api)
         m_api->LoggerOut(MTLogWarn,L"failed to open base file '%s' for read, last error=%u",path.Str(),GetLastError());
      return(false);
     }
//--- check header and return
   if(!CheckHeader())
     {
      m_file.Close();
      //--- maybe this is an old version?
      if(!ConvertBase(day_begin))
        {
         //--- log error
         if(m_api)
            m_api->LoggerOut(MTLogWarn,L"failed to convert base file '%s' for read, last error=%u. Delete file.",path.Str(),GetLastError());
         //--- simply delete this file
         DeleteFileW(path.Str());
         return(false);
        }
      return(OpenRead(day_begin));
     }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Close journal file                                               |
//+------------------------------------------------------------------+
void CTransactionBase::Close(void)
  {
//--- close file
   m_file_sync.Lock();
   m_file.Close();
   m_file_sync.Unlock();
//--- clear
   m_file_size=0;
   m_timestamp=0;
  }
//+------------------------------------------------------------------+
//| Initialize the base                                              |
//+------------------------------------------------------------------+
bool CTransactionBase::Initialize(CMTStr& path,IMTServerAPI *api)
  {
//--- close
   Close();
//--- store server API interface
   m_api=api;
//--- checks
   if(path.Empty())
      return(false);
//--- set the path
   m_path.Assign(path);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Asynchronous record writing to current file                      |
//+------------------------------------------------------------------+
bool CTransactionBase::Write(const TransactionRecord& record)
  {
//--- initializing thread for writing to file
   if(!Start() || !m_income_event)
     {
      //--- writing record directly to file
      return(WriteRecord(record));
     }
//--- trying to add record to array
   if(!PushRecord(record))
     {
      //--- stop operation, otherwise writing directly to file violates chronology
      return(false);
     }
//--- set prepared array processing signal
   SetEvent(m_income_event);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Write record to current file                                     |
//+------------------------------------------------------------------+
bool CTransactionBase::WriteRecord(const TransactionRecord& record)
  {
//--- under lock
   m_file_sync.Lock();
//--- is base already opened?
   if(!m_file.IsOpen() || m_timestamp!=SMTTime::DayBegin(record.timestamp))
      if(!OpenWrite(record.timestamp))
        {
         m_file_sync.Unlock();
         return(false);
        }
//--- write it
   DWORD written=m_file.Write(&record,sizeof(record));
   m_file_size+=written;
//--- expand the file
   if((m_file.Size()-m_file_size)<FILE_DELTA_RESIZE)
     {
      m_file.ChangeSize(m_file.Size()+FILE_STEP_SIZE);
      m_file.Seek(m_file_size,FILE_BEGIN);
     }
//--- return result
   m_file_sync.Unlock();
   return(written!=0);
  }
//+------------------------------------------------------------------+
//| Check the database header of a file                              |
//+------------------------------------------------------------------+
bool CTransactionBase::CheckHeader(void)
  {
//--- checks
   if(!m_file.IsOpen())
      return(false);
//--- set position
   if(m_file.Seek(0,FILE_BEGIN)==CMTFile::INVALID_POSITION)
      return(false);
//--- 
   DataBaseHeader header;
   if(!m_file.Read(&header,sizeof(header)))
      return(false);
   CMTStr::Terminate(header.copyright);
   CMTStr::Terminate(header.name);
//--- check and return
   return(header.version==TransactionsBaseVersion && CMTStr::Compare(header.name,TransactionsBaseName)==0);
  }
//+------------------------------------------------------------------+
//| Write the data base header                                       |
//+------------------------------------------------------------------+
bool CTransactionBase::WriteHeader(void)
  {
//--- set position
   if(!m_file.IsOpen())
      return(false);
   if(m_file.Seek(0,FILE_BEGIN)==CMTFile::INVALID_POSITION)
      return(false);
//--- fill the header
   DataBaseHeader header;
   header.version=TransactionsBaseVersion;
   GetSystemTimeAsFileTime((FILETIME*)&header.timesign);
   CMTStr::Copy(header.copyright,Copyright);
   CMTStr::Copy(header.name,TransactionsBaseName);
//--- write it
   if(!m_file.Write(&header,sizeof(header)))
      return(false);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Seek the last record (the first empty record)                    |
//+------------------------------------------------------------------+
bool CTransactionBase::SeekLastRecord(void)
  {
//--- checks
   if(!m_file.IsOpen())
      return(false);
//--- get current position
   uint64_t currpos=m_file.CurrPos();
//--- find the last record
   TransactionRecord buffer[8]={};
   uint64_t filepos=0,i=0,readed=0;
   for(readed=0;i==readed;)
     {
      //--- read by chunk
      if((readed=m_file.Read(buffer,sizeof(buffer)))==0)
         break;
      //--- is it empty?
      readed/=sizeof(buffer[0]);
      for(i=0;i<readed;i++,filepos++)
         if(buffer[i].IsEmpty())
            break;
     }
//--- seek the last record
   if(m_file.Seek(currpos+sizeof(buffer[0])*filepos,FILE_BEGIN)==CMTFile::INVALID_POSITION)
      return(false);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Returns the next transaction from the opened base                |
//+------------------------------------------------------------------+
bool CTransactionBase::Next(TransactionRecord& record)
  {
//--- clear all
   ZeroMemory(&record,sizeof(record));
//--- checks
   if(!m_file.IsOpen())
      return(false);
//--- read the next record
   if(!m_file.Read(&record,sizeof(record)))
      return(false);
//--- is it empty
   if(record.IsEmpty())
      return(false);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Base convertation                                                |
//+------------------------------------------------------------------+
bool CTransactionBase::ConvertBase(const int64_t timestamp)
  {
//--- firstly close
   Close();
//--- checks
   if(timestamp<=0)
      return(false);
//--- get name of the file
   int64_t      day_begin=SMTTime::DayBegin(timestamp);
   CMTStrPath name;
   name.Format(L"%04u%02u%02u.dat",SMTTime::Year(day_begin),SMTTime::Month(day_begin),SMTTime::Day(day_begin));
   CMTStrPath path=m_path;
   path.Append(name);
   CMTStrPath path_convert=m_path;
   path_convert.Append(L"converted_");
   path_convert.Append(name);
//--- open the file
   if(!m_file.Open(path.Str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,OPEN_EXISTING))
      return(false);
//--- get header
   DataBaseHeader header;
   if(!m_file.Read(&header,sizeof(header)))
     {
      m_file.Close();
      return(false);
     }
   CMTStr::Terminate(header.copyright);
   CMTStr::Terminate(header.name);
//--- check header
   uint32_t old_version=header.version;
   if(old_version!=100 && old_version!=101 && CMTStr::Compare(header.name,TransactionsBaseName)==0)
     {
      m_file.Close();
      return(false);
     }
//--- open converted file
   CMTFile file_converted;
   if(!file_converted.Open(path_convert.Str(),GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,CREATE_ALWAYS))
     {
      file_converted.Close();
      m_file.Close();
      return(false);
     }
//--- set convert size
   if(!file_converted.ChangeSize(m_file.Size()))
     {
      m_file.Close();
      file_converted.Close();
      return(false);
     }
//--- write new header
   file_converted.Seek(0,FILE_BEGIN);
   header.version=TransactionsBaseVersion;
   GetSystemTimeAsFileTime((FILETIME*)&header.timesign);
   CMTStr::Copy(header.copyright,Copyright);
   CMTStr::Copy(header.name     ,TransactionsBaseName);
   if(!file_converted.Write(&header,sizeof(header)))
     {
      m_file.Close();
      file_converted.Close();
      return(false);
     }
//--- convert to new file
   TransactionRecord record={0};
   do
     {
      //--- read the record
      if(m_file.Read(&record,sizeof(record))!=sizeof(record))
         break;
      //--- convert from old format
      switch(old_version)
        {
         case 100:
            ConvertBase100(record);
            break;
         case 101:
            ConvertBase101(record);
            break;
        }
      //--- write to converted file
      if(!file_converted.Write(&record,sizeof(record)))
        {
         m_file.Close();
         file_converted.Close();
         DeleteFileW(path_convert.Str());
         return(false);
        }
     }
   while(!record.IsEmpty());
//--- switch the files under lock
   m_file.Close();
   file_converted.Close();
   m_file_sync.Lock();
   if(!MoveFileExW(path_convert.Str(),path.Str(),MOVEFILE_REPLACE_EXISTING))
      return(false);
   m_file_sync.Unlock();
//--- converted
   return(true);
  }
//+------------------------------------------------------------------+
//| Base convertation                                                |
//+------------------------------------------------------------------+
void CTransactionBase::ConvertBase100(TransactionRecord& record)
  {
//--- convert the record
   switch(record.action)
     {
      //--- market in operation
      case TransactionRecord::ACTION_MARKET_IN   :
         record.action=TransactionRecord::ACTION_MARKET;
         record.entry =TransactionRecord::ENTRY_IN;
         break;
         //--- market out operation
      case TransactionRecord::ACTION_MARKET_OUT  :
         record.action=TransactionRecord::ACTION_MARKET;
         record.entry =TransactionRecord::ENTRY_OUT;
         break;
         //--- market in/out operation
      case TransactionRecord::ACTION_MARKET_INOUT:
         record.action=TransactionRecord::ACTION_MARKET;
         record.entry =TransactionRecord::ENTRY_INOUT;
         break;
         //--- pending order activation
      default:
         record.entry=TransactionRecord::ENTRY_UNKNOWN;
         break;
     }
  }
//+------------------------------------------------------------------+
//| Base convertation                                                |
//+------------------------------------------------------------------+
void CTransactionBase::ConvertBase101(TransactionRecord& record)
  {
   record.lots=SMTMath::VolumeExtFromVolume(record.lots);
  }
//+------------------------------------------------------------------+
//| Adding record to writing to file array                           |
//+------------------------------------------------------------------+
inline bool CTransactionBase::PushRecord(const TransactionRecord& record)
  {
//--- under lock
   m_income_sync.Lock();
//--- trying to add record to array
   if(!m_income.Add(&record))
     {
      m_income_sync.Unlock();
      return(false);
     }
//--- reset standby event
   if(m_standby_event)
      ResetEvent(m_standby_event);
//--- ok
   m_income_sync.Unlock();
   return(true);
  }
//+------------------------------------------------------------------+
//| Extracting records array for writing to file                     |
//+------------------------------------------------------------------+
inline void CTransactionBase::PopRecords(RecordArray& records)
  {
//--- preparing array
   records.Clear();
//--- under lock
   m_income_sync.Lock();
//--- extracting records
   records.Swap(m_income);
   m_income_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Start thread of records processing                               |
//+------------------------------------------------------------------+
uint32_t CTransactionBase::ProcessWrapper(LPVOID param)
  {
//--- check
   CTransactionBase *pThis=reinterpret_cast<CTransactionBase*>(param);
   if(pThis)
      pThis->Process();
//--- ok
   return(0);
  }
//+------------------------------------------------------------------+
//| Process records                                                  |
//+------------------------------------------------------------------+
void CTransactionBase::Process(void)
  {
   RecordArray records;
   HANDLE      events[]={ m_income_event,m_thread_exit };
   while(WaitForMultipleObjects(_countof(events),events,FALSE,INFINITE)==WAIT_OBJECT_0)
     {
      //--- reset thread standy event
      if(m_standby_event)
         ResetEvent(m_standby_event);
      //--- pop and write records
      PopRecords(records);
      for(uint32_t i=0;i<records.Total();i++)
        {
         WriteRecord(records[i]);
        }
      //--- set thread standy event
      if(m_standby_event)
        {
         //--- under lock
         m_income_sync.Lock();
         //--- set stanby event on empty queque
         if(!m_income.Total())
            SetEvent(m_standby_event);
         m_income_sync.Unlock();
        }
     }
  }
//+------------------------------------------------------------------+
//| Wait for records processing thread standby                       |
//+------------------------------------------------------------------+
bool CTransactionBase::WaitStandby(const uint32_t timeout)
  {
   if(!m_standby_event)
      return(true);
//--- wait for processing thread standby
   if(WaitForSingleObject(m_standby_event,timeout)!=WAIT_OBJECT_0)
      return(false);
//--- flush file
   if(m_file.IsOpen())
      m_file.Flush();
   return(true);
  }
//+------------------------------------------------------------------+
//| Start thread of records processing                               |
//+------------------------------------------------------------------+
bool CTransactionBase::Start(void)
  {
//--- reset forced thread completion signal
   if(m_thread_exit)
      ResetEvent(m_thread_exit);
   else if((m_thread_exit=CreateEvent(NULL,TRUE,FALSE,NULL))==NULL)
         return(false);
//--- prepare array processing signal with records
   if(m_income_event==NULL)
      if((m_income_event=CreateEvent(NULL,FALSE,FALSE,NULL))==NULL)
         return(false);
//--- create thread standby event
   if(m_standby_event==NULL)
      if((m_standby_event=CreateEvent(NULL,TRUE,TRUE,NULL))==NULL)
         return(false);
//--- launching thread
   if(!m_thread.IsBusy())
      if(!m_thread.Start(ProcessWrapper,this,0))
         return(false);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Shutdown thread of records processing                            |
//+------------------------------------------------------------------+
void CTransactionBase::Shutdown(void)
  {
//--- set forced thread completion signal
   if(m_thread_exit)
      SetEvent(m_thread_exit);
//--- waiting thread completion
   m_thread.Shutdown();
//--- close forced thread completion signal
   if(m_thread_exit)
     {
      CloseHandle(m_thread_exit);
      m_thread_exit=NULL;
     }
//--- close prepared array processing signal
   if(m_income_event)
     {
      CloseHandle(m_income_event);
      m_income_event=NULL;
     }
//--- close thread standby event
   if(m_standby_event)
     {
      CloseHandle(m_standby_event);
      m_standby_event=NULL;
     }
//--- clear server API interface
   m_api=NULL;
  }
//+------------------------------------------------------------------+
