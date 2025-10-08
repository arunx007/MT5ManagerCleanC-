//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportVector.h"
//+------------------------------------------------------------------+
//| Report dictionary class                                          |
//+------------------------------------------------------------------+
class CReportDictionary
  {
private:
   typedef TReportVector<LPCWSTR> TVectorCStr;  // array type

private:
   TVectorCStr       m_strings;                 // strings array
   uint32_t          m_spectial_pos;            // special string position
   LPCWSTR           m_spectial_string;         // special string

public:
                     CReportDictionary(void) : m_strings(false),m_spectial_pos(0),m_spectial_string(nullptr) {}
                    ~CReportDictionary(void) {}
   //--- clear
   void              Clear(void);
   //--- initialization
   bool              Initialize(const uint32_t size,const uint32_t spectial_pos=0,LPCWSTR spectial_string=nullptr);
   //--- string by position
   LPCWSTR           String(const uint32_t pos) const;
   //--- insert string by position
   bool              String(const uint32_t pos,LPCWSTR string);
  };
//+------------------------------------------------------------------+
//| clear                                                            |
//+------------------------------------------------------------------+
inline void CReportDictionary::Clear(void)
  {
//--- delete strings
   for(uint32_t i=0,total=m_strings.Total();i<total;i++)
      if(LPCWSTR *string=m_strings.Item(i))
         if(*string)
            delete[] *string;
//--- clear strings vector
   m_strings.Clear();
//--- clear special string
   m_spectial_pos=0;
   m_spectial_string=nullptr;
  }
//+------------------------------------------------------------------+
//| initialization                                                   |
//+------------------------------------------------------------------+
inline bool CReportDictionary::Initialize(const uint32_t size,const uint32_t spectial_pos,LPCWSTR spectial_string)
  {
//--- clear
   Clear();
//--- reserve items memory
   if(!m_strings.Initialize(size))
      return(false);
//--- store special string
   m_spectial_pos=spectial_pos;
   m_spectial_string=spectial_string;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| string by position                                               |
//+------------------------------------------------------------------+
inline LPCWSTR CReportDictionary::String(const uint32_t pos) const
  {
//--- check special position
   if(m_spectial_string && m_spectial_pos==pos)
      return(m_spectial_string);
//--- get string by position
   const LPCWSTR *string=m_strings.Item(pos);
   if(!string)
      return(nullptr);
//--- return string
   return(*string);
  }
//+------------------------------------------------------------------+
//| insert string by position                                        |
//+------------------------------------------------------------------+
inline bool CReportDictionary::String(const uint32_t pos,LPCWSTR string)
  {
//--- check string
   if(!string)
      return(false);
//--- get string by position
   LPCWSTR *dst=m_strings.Item(pos);
   if(!dst)
      return(false);
//--- check string exists
   if(*dst)
      return(true);
//--- get string length
   const uint32_t len=CMTStr::Len(string);
//--- allocate string buffer
   *dst=new wchar_t [len+1];
   if(!*dst)
      return(false);
//--- copy string to buffer   
   CMTStr::Copy((LPWSTR)*dst,len+1,string,len);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
