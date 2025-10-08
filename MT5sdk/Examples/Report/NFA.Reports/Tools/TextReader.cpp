//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "TextReader.h"
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CTextReader::CTextReader(const uint32_t buf_size) : m_buf(NULL),m_str(NULL),m_str_end(NULL),m_buf_size(buf_size),m_index(0),m_readed(0),m_line_pos(0)
  {
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CTextReader::~CTextReader(void)
  {
//--- close file
   Close();
//--- clear buffer
   if(m_buf!=NULL) { delete[] m_buf; m_buf=NULL; }
//--- clear string`s buffer
   if(m_str!=NULL) { delete[] m_str; m_str=NULL; }
  }
//+------------------------------------------------------------------+
//| Open file or open zip archive                                    |
//+------------------------------------------------------------------+
bool CTextReader::Open(LPCWSTR path)
  {
   Close();
//--- check and open
   if(!path)
      return(false);
   if(!m_file.OpenRead(path))
     {
      LPCWSTR file_ext=NULL;
      //--- is it archive?
      if((file_ext=wcsrchr(path,L'.'))==NULL)
         return(false);
      //--- generate path
      wchar_t tmp[MAX_PATH]={};
      wcsncpy_s(tmp,_countof(tmp),path,file_ext-path);
      wcsncat_s(tmp,_countof(tmp),L".zip",_TRUNCATE);
      //--- open archive and and first file in it
      if(!m_file_zip.Open(tmp) || !m_file_zip.FileOpen())
        {
         m_file_zip.Close();
         return(false);
        }
     }
//--- create buffer
   if((m_buf==NULL) && ((m_buf=new(std::nothrow) char[m_buf_size])==NULL))
      return(false);
//--- reset buffer
   ZeroMemory(m_buf,m_buf_size);
//--- create string buffer
   if((m_str==NULL) && ((m_str=new(std::nothrow) char[m_buf_size])==NULL))
      return(false);
//--- reset string buffer
   ZeroMemory(m_str,m_buf_size);
//--- reset counters
   m_readed=0;
   m_index =0;
//--- all right
   return(true);
  }
//+------------------------------------------------------------------+
//| Close reader                                                     |
//+------------------------------------------------------------------+
void CTextReader::Close(void)
  {
//--- close the file
   m_file.Close();
//--- close file in archive
   m_file_zip.FileClose();
//--- close archive
   m_file_zip.Close();
  }
//+------------------------------------------------------------------+
//| Read one line from file and return true if read                  |
//+------------------------------------------------------------------+
bool CTextReader::ReadLine(void)
  {
   m_line_pos=0;
   bool found=false;
//--- read string by string
   while(ReadNextLine())
     {
      //--- if string is empty, then skip it
      if(m_str_end==m_str)
         continue;
      found=true;
      break;
     }
//--- if it is not found
   if(!found)
      return(false);
//--- return length of string
   m_line.Clear();
   m_line.Assign(m_str,uint32_t(m_str_end-m_str));
//--- return string
   return(true);
  }
//+------------------------------------------------------------------+
//| Get next string field                                            |
//+------------------------------------------------------------------+
void CTextReader::String(LPWSTR str,uint32_t dstsize)
  {
   if(LineEnd())
      return;
//---
   int32_t pos=CMTStr::FindChar(m_line.Str()+m_line_pos,L'|');
   if(pos<0)
      pos=m_line.Len()-m_line_pos;
//---
   CMTStr::Copy(str,dstsize,m_line.Str()+m_line_pos,pos);
//---
   m_line_pos+=pos+1;
  }
//+------------------------------------------------------------------+
//| Parse date field                                                 |
//+------------------------------------------------------------------+
void CTextReader::Date(int64_t &value)
  {
   if(LineEnd())
      return;
//---
   tm ttm={ 0 };
   if(SPluginHelpers::ParseBatchDate(m_line.Str()+m_line_pos,ttm))
      value=_mkgmtime64(&ttm);
//---
   LineNextColumn();
  }
//+------------------------------------------------------------------+
//| Parse datetime field                                             |
//+------------------------------------------------------------------+
void CTextReader::DateTime(int64_t &value)
  {
   if(LineEnd())
      return;
//---
   tm ttm={ 0 };
   if(SPluginHelpers::ParseBatchDatetime(m_line.Str()+m_line_pos,ttm))
      value=_mkgmtime64(&ttm);
//---
   LineNextColumn();
  }
//+------------------------------------------------------------------+
//| Parse double field                                               |
//+------------------------------------------------------------------+
void CTextReader::Double(double &value)
  {
   if(LineEnd())
      return;
//---
   swscanf_s(m_line.Str()+m_line_pos,L"%lf",&value);
//---
   LineNextColumn();
  }
//+------------------------------------------------------------------+
//| Parse unsigned integer field                                     |
//+------------------------------------------------------------------+
void CTextReader::Integer(uint64_t &value)
  {
   if(LineEnd())
      return;
//---
   swscanf_s(m_line.Str()+m_line_pos,L"%I64u",&value);
//---
   LineNextColumn();
  }
//+------------------------------------------------------------------+
//| Skip count of fields                                             |
//+------------------------------------------------------------------+
void CTextReader::Skip(const uint32_t count)
  {
   for(uint32_t i=0;i<count;i++)
      LineNextColumn();
  }
//+------------------------------------------------------------------+
//| Check line is over                                               |
//+------------------------------------------------------------------+
bool CTextReader::LineEnd(void) const
  {
   return(m_line_pos<0 || m_line_pos>=(int)m_line.Len());
  }
//+------------------------------------------------------------------+
//| Move line start to the next field or on the end of line          |
//+------------------------------------------------------------------+
void CTextReader::LineNextColumn(void)
  {
   if(!LineEnd())
     {
      const int32_t pos=CMTStr::FindChar(m_line.Str()+m_line_pos,L'|');
      m_line_pos=(pos<0) ? m_line.Len() : m_line_pos+pos+1;
     }
  }
//+------------------------------------------------------------------+
//| Reading data from file                                           |
//+------------------------------------------------------------------+
uint32_t CTextReader::Read(void *buffer,const uint32_t length)
  {
//--- если открыт обычный файл, читаем из него
   if(m_file.IsOpen())
      return(m_file.Read(buffer,length));
//--- иначе читаем из архива
   if(m_file_zip.IsOpen())
      return(m_file_zip.FileRead(buffer,length));
//--- нет ничего
   return(0);
  }
//+------------------------------------------------------------------+
//| Reading next string from file                                    |
//+------------------------------------------------------------------+
bool CTextReader::ReadNextLine(void)
  {
   char *currsym=m_str,*lastsym=&m_str[m_buf_size-1],*curpos=&m_buf[m_index];
//--- checking
   if(m_buf==NULL || m_str==NULL)
      return(false);
//--- reading string all the way
   while(true)
     {
      //--- entire buffer was readed
      if(m_index==m_readed)
        {
         //--- read into buffer
         m_readed=Read(m_buf,m_buf_size);
         //--- is it the end?
         if(m_readed<1)
           {
            m_str_end=currsym;
            *currsym=0;
            return(currsym!=m_str?true:false);
           }
         //--- reset pointers
         curpos=m_buf;
         m_index=0;
        }
      //--- analyzing buffer
      while(m_index<m_readed)
        {
         //--- increment index
         m_index++;
         //--- clip the string, if size was exceeded
         if(currsym>=lastsym)
           {
            m_str_end=currsym;
            *currsym=0;
            return(true);
           }
         //--- the end of string
         if(*curpos==L'\n')
           {
            m_str_end=currsym;
            *currsym=0;
            return(true);
           }
         //--- skipping service symbols 
         if((*curpos==L'\r') || (*curpos==L'\0'))
           {
            curpos++;
            continue;
           }
         //--- copy ordinary symbol
         *currsym++=*curpos++;
        }
      //--- if it was processed more than there is
      if(m_index>m_readed) break;
     }
//--- fail
   return(false);
  }
//+------------------------------------------------------------------+
