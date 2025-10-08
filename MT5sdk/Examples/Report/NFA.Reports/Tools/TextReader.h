//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "Zip\UnzipFile.h"
//+------------------------------------------------------------------+
//| Class helper for reading/writing ANSI-encoded files line-by-line |
//+------------------------------------------------------------------+
class CTextReader
  {
   enum constants
     {
      BUFFER_SIZE=256*1024                 // 256 Kb
     };
private:
   CMTFile           m_file;
   CUnzipFile        m_file_zip;
   char             *m_buf;               // file buffer
   char             *m_str;               // buffer for the string reading
   char             *m_str_end;           // the end of the string
   uint32_t          m_buf_size;          // sizes of buffers
   uint32_t          m_index;             // index of current symbol
   uint32_t          m_readed;            // number of symbols readed
   //--- line parser
   CMTStr4096        m_line;
   int32_t           m_line_pos;

public:
                     CTextReader(const uint32_t buf_size=BUFFER_SIZE);
                    ~CTextReader(void);
   //--- open/close
   bool              Open(LPCWSTR path);
   void              Close(void);
   //--- read line
   bool              ReadLine(void);
   //--- line parser
   void              String(LPWSTR dst,uint32_t dstsize);
   template <uint32_t dstsize>
   void              String(wchar_t (&dst)[dstsize]) { String(dst,dstsize); }
   void              Date(int64_t &value);
   void              DateTime(int64_t &value);
   void              Double(double &value);
   void              Integer(uint64_t &value);
   void              Skip(const uint32_t count=1);

private:
   //--- line parse
   bool              LineEnd(void) const;
   void              LineNextColumn(void);
   //--- reading data from file
   uint32_t          Read(void *buffer,const uint32_t length);
   //--- processing string
   bool              ReadNextLine(void);
  };
//+------------------------------------------------------------------+
