//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Helpers.h"
#include "TextBaseWriter.h"
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CTextBaseWriter::CTextBaseWriter(void)
  {
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CTextBaseWriter::~CTextBaseWriter(void)
  {
   m_base.Close();
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
LPCWSTR CTextBaseWriter::LastError() const
  {
   return m_last_error.Str();
  }
//+------------------------------------------------------------------+
//| Creates report directory, file, file header                      |
//| and writes record into the database                              |
//+------------------------------------------------------------------+
bool CTextBaseWriter::WriteRecord(LPCWSTR server_dir,LPCWSTR base_dir,LPCWSTR name,tm &batch,LPCWSTR header,LPCWSTR record)
  {
   CMTStr32 date_str;
//--- check for same file
   if(CMTStr::Compare(SPluginHelpers::FormatFileDate(date_str,batch),m_last_date.Str())!=0)
     {
      CMTStrPath path_str;
      //--- format path and create directory
      path_str.Format(L"%s\\Reports\\NFA.Reports\\%s\\%s\\",server_dir,base_dir,date_str.Str());
      if(!CMTFile::DirectoryCreate(path_str))
        {
         m_last_error.Format(L"create directory error [%s][%u]",path_str.Str(),GetLastError());
         return(false);
        }
      //--- prepare full path and open file
      path_str.Append(name);
      path_str.Append(L'_');
      path_str.Append(date_str);
      path_str.Append(L".txt");
      if(!m_base.Open(path_str.Str()))
        {
         m_last_error.Format(L"open base file error [%s][%u]",path_str.Str(),GetLastError());
         return(false);
        }
      //--- write header if needed
      if(m_base.IsEmpty() && !m_base.WriteLine(header))
        {
         m_last_error.Format(L"write base header [%s_%s][%u]",name,date_str.Str(),GetLastError());
         return(false);
        }
      //--- Remember last written file date
      m_last_date.Assign(date_str);
     }
//--- write record
   if(!m_base.WriteRecord(record))
     {
      m_last_error.Format(L"write base record [%s_%s][%u]",name,date_str.Str(),GetLastError());
      return(false);
     }
//--- success
   return(true);
  }
//+------------------------------------------------------------------+
//| Close specified batch day if opened                              |
//+------------------------------------------------------------------+
void CTextBaseWriter::CloseDay(const tm &batch)
  {
   CMTStr32 date_str;
   if(CMTStr::Compare(SPluginHelpers::FormatFileDate(date_str,batch),m_last_date.Str())==0)
      m_base.Close();
  }
//+------------------------------------------------------------------+
