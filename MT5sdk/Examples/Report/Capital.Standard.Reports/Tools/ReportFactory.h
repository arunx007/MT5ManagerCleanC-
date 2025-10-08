//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Report factory class                                             |
//+------------------------------------------------------------------+
struct CReportFactory
  {
public:
   typedef void      (*InfoFuctionPtr)(MTReportInfo &);   // report information function pointer type
   typedef IMTReportContext* (*CreateFuctionPtr)(void);  // report fabric function pointer type

private:
   InfoFuctionPtr    m_func_info;   // report information function pointer
   CreateFuctionPtr  m_func_create;  // report fabric function pointer

public:
                     CReportFactory(InfoFuctionPtr func_info,CreateFuctionPtr func_create);
                     CReportFactory(const CReportFactory &r);
                    ~CReportFactory(void);
   //--- factory template method
   template<class TReport>
   static CReportFactory Create(void);
   //--- assignment operator
   CReportFactory&   operator=(const CReportFactory &r);
   //--- report information
   MTAPIRES          About(MTReportInfo& info) const;
   //--- create report
   MTAPIRES          Create(IMTReportContext **context) const;

private:
   //--- default construction prohibited
                     CReportFactory(void) {}
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CReportFactory::CReportFactory(InfoFuctionPtr func_info,CreateFuctionPtr func_create) :
   m_func_info(func_info),m_func_create(func_create)
  {
  }
//+------------------------------------------------------------------+
//| Copy constructor                                                 |
//+------------------------------------------------------------------+
CReportFactory::CReportFactory(const CReportFactory &r) :
   m_func_info(r.m_func_info),m_func_create(r.m_func_create)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CReportFactory::~CReportFactory(void)
  {
  }
//+------------------------------------------------------------------+
//| Factory template method                                          |
//+------------------------------------------------------------------+
template<class TReport>
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CReportFactory CReportFactory::Create(void)
  {
   return(CReportFactory(TReport::Info,TReport::Create));
  }
//+------------------------------------------------------------------+
//| Assignment operator                                              |
//+------------------------------------------------------------------+
CReportFactory& CReportFactory::operator=(const CReportFactory &r)
  {
//--- check self assignment
   if(this==&r)
      return(*this);
//--- assign functions pointers
   m_func_info=r.m_func_info;
   m_func_create=r.m_func_create;
//--- return self reference
   return(*this);
  }
//+------------------------------------------------------------------+
//| report information                                               |
//+------------------------------------------------------------------+
MTAPIRES CReportFactory::About(MTReportInfo& info) const
  {
//--- check function pointer
   if(!m_func_info)
      return(MT_RET_ERROR);
//--- get report info
   m_func_info(info);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| create report                                                    |
//+------------------------------------------------------------------+
MTAPIRES CReportFactory::Create(IMTReportContext **context) const
  {
//--- check argument
   if(!context)
      return(MT_RET_ERR_PARAMS);
//--- check function pointer
   if(!m_func_create)
      return(MT_RET_ERROR);
//--- create instance
   *context=m_func_create();
   if(!*context)
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
