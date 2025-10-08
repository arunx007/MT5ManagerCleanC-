//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//---
#include "UnzipFile.h"
//+------------------------------------------------------------------+
//| Class for reading log files                                      |
//+------------------------------------------------------------------+
class CLogFileReader
  {
private:
   //--- 
   enum constants
     {
      BUFFER_SIZE=256*1024                 // 256 Kb
     };
   //--- data buffers
   CMTFile           m_file;              // file
   CUnzipFile        m_zip_file;          // zip-распаковщик
   wchar_t          *m_buf;               // file buffer
   wchar_t          *m_str;               // buffer for the string reading
   wchar_t          *m_str_end;           // the end of the string
   uint32_t          m_buf_size;          // sizes of buffers
   uint32_t          m_index;             // index of current symbol
   uint32_t          m_readed;            // number of symbols readed

public:
                     CLogFileReader(const uint32_t buf_size=BUFFER_SIZE);
                    ~CLogFileReader(void);
   //--- open/close
   bool              Open(const LPCWSTR path);
   void              Close(void);
   //--- getting string
   LPCWSTR           GetNextLine(uint32_t& linesize);

private:
   //--- reading data from file
   uint32_t          Read(void *buffer,const uint32_t length);
   //--- processing string
   bool              ReadNextLine(void);
  };
//+------------------------------------------------------------------+