//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "TextBase.h"
//+------------------------------------------------------------------+
//| Holds opened TextBase file and reopen/create it if need          |
//+------------------------------------------------------------------+
class CTextBaseWriter
  {
private:
   CTextBase         m_base;
   CMTStr16          m_last_date;
   CMTStr256         m_last_error;

public:
                     CTextBaseWriter(void);
                    ~CTextBaseWriter(void);
   //--- if WriteRecord returned false - returns error description
   LPCWSTR           LastError(void) const;
   //--- creates report directory, file, file header and writes record into the database
   bool              WriteRecord(LPCWSTR server_dir,LPCWSTR base_dir,LPCWSTR name,tm &batch,LPCWSTR header,LPCWSTR record);
   //--- ensure day closed
   void              CloseDay(const tm &batch);
  };
//+------------------------------------------------------------------+
