//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Text based CSV database                                          |
//+------------------------------------------------------------------+
class CTextBase
  {
private:
   enum constants
     {
      BASE_GROW_STEP=512*KB,           // file grow step
      BASE_READ_SIZE=32*KB,            // read buffer size
      BADE_IDEN_SIZE=256,              // id buffer size
     };

private:
   //--- common
   uint64_t          m_id;             // last record id
   char              m_delimiter;      // delimiter
   //--- file base
   CMTFile           m_file;           // file
   uint64_t          m_file_size;      // file size
   uint64_t          m_file_max;       // file capacity
   //--- buffer
   char             *m_buffer;         // buffer
   uint32_t          m_buffer_size;    // buffer size

public:
                     CTextBase(void);
                    ~CTextBase(void);
   //--- open/close
   bool              Open(LPCWSTR path,const char delimiter='|');
   void              Close(void);
   //--- shutdown
   void              Shutdown(void);
   //--- access
   bool              IsOpen(void)  const { return(m_file.IsOpen()); }
   bool              IsEmpty(void) const { return(m_file_size==0);  }
   uint64_t          ID(void)      const { return(m_id);            }
   //--- write
   bool              WriteLine(LPCWSTR text);
   bool              WriteRecord(LPCWSTR text);
   //--- flush
   bool              Flush(void) { return(m_file.Flush()); }

private:
   //--- initialization
   bool              Initialize(void);
   //--- raw write
   bool              Write(LPCSTR text);
   //--- reserve buffer
   bool              Reserve(const uint32_t size);
  };
//+------------------------------------------------------------------+

