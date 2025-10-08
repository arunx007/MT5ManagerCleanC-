//+------------------------------------------------------------------+
//|                           MetaTrader 5 Accounts.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ReportBase.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CReportBase::CReportBase(const MTReportInfo &info) :
   m_types(info.types),m_api(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CReportBase::~CReportBase(void)
  {
//--- inner clear
   ClearInner();
  }
//+------------------------------------------------------------------+
//| Release                                                          |
//+------------------------------------------------------------------+
void CReportBase::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CReportBase::Generate(const uint32_t type,IMTReportAPI *api)
  {
//--- check for null
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- check for type
   if(!(type&m_types))
      return(MT_RET_ERR_NOTIMPLEMENT);
//--- remember pointer to report api
   m_api=api;
   m_log.Initialize(api);
//--- prepare report
   MTAPIRES res=Prepare();
   if(res!=MT_RET_OK)
     {
      res=m_log.Error(res,L"Report preparation");
      if(type==MTReportInfo::TYPE_HTML)
         CReportError::Write(api,L"Accounts Growth Report",L"Report generation failed. For more information see server's journal.");
      //--- clear
      ClearAll();
      return(res);
     }
//--- write report 
   if((res=Write(type))!=MT_RET_OK)
     {
      res=m_log.Error(res,L"Report writing");
      if(type==MTReportInfo::TYPE_HTML)
         CReportError::Write(api,L"Accounts Growth Report",L"Report generation failed. For more information see server's journal.");
      //--- clear
      ClearAll();
      return(res);
     }
//--- clear
   ClearAll();
//--- log result
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Inner and outer clear                                            |
//+------------------------------------------------------------------+
void CReportBase::ClearAll(void)
  {
//--- outer clear
   Clear();
//--- inner clear
   ClearInner();
  }
//+------------------------------------------------------------------+
//| Inner clear                                                      |
//+------------------------------------------------------------------+
void CReportBase::ClearInner(void)
  {
//--- zero api pointer
   m_api=nullptr;
//--- clear logger
   m_log.Clear();
  }
//+------------------------------------------------------------------+
//| Get group type by group name                                     |
//+------------------------------------------------------------------+
uint32_t CReportBase::GetGroupType(LPCWSTR group)
  {
//--- checks
   if(!group)
      return(UINT_MAX);
//--- check group name
   if(CMTStr::Find(group,L"manager")>=0)
      return(GROUP_MANAGER);
   if(CMTStr::Find(group,L"coverage")>=0)
      return(GROUP_COVERAGE);
   if(CMTStr::Find(group,L"demo")>=0)
      return(GROUP_DEMO);
   if(CMTStr::Find(group,L"contest")>=0)
      return(GROUP_CONTEST);
   if(CMTStr::Find(group,L"preliminary")>=0)
      return(GROUP_PRELIMINARY);
//--- real group
   return(GROUP_REAL);
  }
//+------------------------------------------------------------------+
//| Get group type name                                              |
//+------------------------------------------------------------------+
bool CReportBase::GetGroupTypeName(const uint32_t type,CMTStr& name)
  {
//--- clear result
   name.Clear();
//--- check type
   switch(type)
     {
      case GROUP_REAL       : name.Assign(L"Real");        return(true);
      case GROUP_PRELIMINARY: name.Assign(L"Preliminary"); return(true);
      case GROUP_DEMO       : name.Assign(L"Demo");        return(true);
      case GROUP_CONTEST    : name.Assign(L"Contest");     return(true);
      case GROUP_COVERAGE   : name.Assign(L"Coverage");    return(true);
      case GROUP_MANAGER    : name.Assign(L"Manager");     return(true);
     }
//--- unknown
   return(false);
  }
//+------------------------------------------------------------------+
