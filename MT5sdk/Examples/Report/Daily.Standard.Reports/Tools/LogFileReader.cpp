//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "LogFileReader.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CLogFileReader::CLogFileReader(const uint32_t buf_size) : m_buf(NULL),m_str(NULL),m_str_end(NULL),m_buf_size(buf_size),
                                                m_index(0),m_readed(0)
  {
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CLogFileReader::~CLogFileReader(void)
  {
//--- close file
   Close();
//--- clear buffer
   if(m_buf!=NULL)     { delete[] m_buf; m_buf=NULL; }
//--- clear string`s buffer
   if(m_str!=NULL)     { delete[] m_str; m_str=NULL; }
  }
//+------------------------------------------------------------------+
//| Closing file                                                     |
//+------------------------------------------------------------------+
void CLogFileReader::Close(void)
  {
//--- close the file
   m_file.Close();
//--- close file in archive
   m_zip_file.FileClose();
//--- close archive
   m_zip_file.Close();
  }
//+------------------------------------------------------------------+
//| Opening file only for reading                                    |
//+------------------------------------------------------------------+
bool CLogFileReader::Open(const LPCWSTR filename)
  {
   LPCWSTR file_ext=NULL;
   BYTE    code[2]={0};
//--- open file for reading
   if(!m_file.OpenRead(filename))
     {
      //--- is it archive?
      if((file_ext=wcsrchr(filename,L'.'))==NULL)
         return(false);
      //--- generate path
      wchar_t path[MAX_PATH]={};

      wcsncpy_s(path,_countof(path),filename,file_ext-filename);
      wcsncat_s(path,_countof(path),L".zip",_TRUNCATE);
      //--- open archive and and first file in it
      if(!m_zip_file.Open(path) || !m_zip_file.FileOpen())
        {
         m_zip_file.Close();
         return(false);
        }
     }
//--- read unicode sign
   if((Read(code,sizeof(code))!=sizeof(code)) || (code[0]!=0xFF) || (code[1]!=0xFE))
      return(false);
//--- create buffer
   if((m_buf==NULL) && ((m_buf=new(std::nothrow) wchar_t[m_buf_size])==NULL))
      return(false);
//--- reset buffer
   ZeroMemory(m_buf,m_buf_size*sizeof(wchar_t));
//--- create string buffer
   if((m_str==NULL) && ((m_str=new(std::nothrow) wchar_t[m_buf_size])==NULL))
      return(false);
//--- reset string buffer
   ZeroMemory(m_str,m_buf_size*sizeof(wchar_t));
//--- reset counters
   m_readed=0;
   m_index =0;
//--- all right
   return(true);
  }
//+------------------------------------------------------------------+
//| Extract the next string suitable for the filter                  |
//+------------------------------------------------------------------+
LPCWSTR CLogFileReader::GetNextLine(uint32_t& linesize)
  {
   bool found=false;
//--- reset line of string
   linesize=0;
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
   if(!found) return(NULL);
//--- return length of string
   linesize=uint32_t(m_str_end-m_str);
//--- return string
   return(m_str);
  }
//+------------------------------------------------------------------+
//| Reading data from file                                           |
//+------------------------------------------------------------------+
uint32_t CLogFileReader::Read(void *buffer,const uint32_t length)
  {
//--- если открыт обычный файл, читаем из него
   if(m_file.IsOpen())
      return(m_file.Read(buffer,length));
//--- иначе читаем из архива
   if(m_zip_file.IsOpen())
      return(m_zip_file.FileRead(buffer,length));
//--- нет ничего
   return(0);
  }
//+------------------------------------------------------------------+
//| Reading next string from file                                    |
//+------------------------------------------------------------------+
bool CLogFileReader::ReadNextLine(void)
  {
   wchar_t *currsym=m_str,*lastsym=&m_str[m_buf_size-1],*curpos=&m_buf[m_index];
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
         m_readed=Read(m_buf,m_buf_size)/sizeof(wchar_t);
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
