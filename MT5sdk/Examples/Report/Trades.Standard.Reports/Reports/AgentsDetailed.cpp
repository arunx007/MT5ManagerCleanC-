//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "AgentsDetailed.h"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
MTReportInfo CAgentsDetailed::s_info=
  {
   100,
   MTReportAPIVersion,
   0,
   L"Agents Detailed",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_NONE,
   MTReportInfo::TYPE_HTML,
   L"Trades",
                   // params
     {{ MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, L"*" },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM         },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_TO           },
     },3            // params_total
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CAgentsDetailed::CAgentsDetailed(void) : m_api(NULL),m_deal(NULL),m_deals(NULL),m_user(NULL),m_group(NULL),m_logins(NULL),m_logins_total(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CAgentsDetailed::~CAgentsDetailed(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CAgentsDetailed::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release plug-in                                                  |
//+------------------------------------------------------------------+
void CAgentsDetailed::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CAgentsDetailed::Generate(const uint32_t type,IMTReportAPI *api)
  {
   MTAPIRES res=MT_RET_OK;
//--- checks
   if(api==NULL)
      return(MT_RET_ERR_PARAMS);
   if(type!=MTReportInfo::TYPE_HTML)
      res=MT_RET_ERR_NOTIMPLEMENT;
//--- save api pointer
   m_api=api;
//--- create interfaces
   if(res==MT_RET_OK)
      if(!CreateInterfaces())
         res=MT_RET_ERR_MEM;
//--- write all into html
   if(res==MT_RET_OK)
      res=Write();
//--- cleanup
   Clear();
   return(res);
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CAgentsDetailed::Clear(void)
  {
//--- deal interface
   if(m_deal)
     {
      m_deal->Release();
      m_deal=NULL;
     }
//--- deals interface
   if(m_deals)
     {
      m_deals->Release();
      m_deals=NULL;
     }
//--- user interface
   if(m_user)
     {
      m_user->Release();
      m_user=NULL;
     }
//--- group interface
   if(m_group)
     {
      m_group->Release();
      m_group=NULL;
     }
//--- free logins
   if(m_api && m_logins)
      m_api->Free(m_logins);
   m_logins=NULL;
   m_logins_total=0;
//--- clear summary
   m_summaries.Clear();
  }
//+------------------------------------------------------------------+
//| Create api interfaces                                            |
//+------------------------------------------------------------------+
bool CAgentsDetailed::CreateInterfaces(void)
  {
//--- clear old
   Clear();
//--- creating
   if((m_deal=m_api->DealCreate())==NULL)
      return(false);
   if((m_deals=m_api->DealCreateArray())==NULL)
      return(false);
   if((m_user=m_api->UserCreate())==NULL)
      return(false);
   if((m_group=m_api->GroupCreate())==NULL)
      return(false);
//--- all right
   return(true);
  }
//+------------------------------------------------------------------+
//| Write report into html                                           |
//+------------------------------------------------------------------+
MTAPIRES CAgentsDetailed::Write(void)
  {
   MTAPISTR      tag;
   CMTStr256     str;
   uint32_t      counter;
   IMTConReport *report      =NULL;
   MTAPIRES      res         =MT_RET_OK;
//--- create report interface
   if((report=m_api->ReportCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- get report interface
   report->Clear();
   if((res=m_api->ReportCurrent(report))!=MT_RET_OK)
      return(res);
//--- use template
   if((res=m_api->HtmlTplLoadResource(IDR_AGENTS_DETAILED,RT_HTML))!=MT_RET_OK)
     {
      report->Release();
      return(res);
     }
//--- process tags
   while((res=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
     {
      //--- print the report name
      if(CMTStr::CompareNoCase(tag,L"report_name")==0)
        {
         if((res=m_api->HtmlWriteSafe(report->Name(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            break;
         continue;
        }
      //--- date
      if(CMTStr::CompareNoCase(tag,L"date")==0)
        {
         str.Format(L"%02u.%02u.%04u - %02u.%02u.%04u",
             SMTTime::Day(m_api->ParamFrom()),
             SMTTime::Month(m_api->ParamFrom()),
             SMTTime::Year(m_api->ParamFrom()),
             SMTTime::Day(m_api->ParamTo()),
             SMTTime::Month(m_api->ParamTo()),
             SMTTime::Year(m_api->ParamTo()));
         if((res=m_api->HtmlWriteString(str.Str()))!=MT_RET_OK)
            break;
         continue;
        }
      //--- agents commissions
      if(CMTStr::CompareNoCase(tag,L"commissions")==0)
        {
         if(WriteRows(tag,res))
           {
            if(res!=MT_RET_OK)
               return(res);
            continue;
           }
        }
      //--- summaries
      if(CMTStr::CompareNoCase(tag,L"summary")==0)
        {
         if(WriteSummaries(tag,res))
           {
            if(res!=MT_RET_OK)
               return(res);
            continue;
           }
        }
     }
   report->Release();
//--- checks
   if(res!=MT_RET_REPORT_TEMPLATE_END)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write agents commissions table                                   |
//+------------------------------------------------------------------+
bool CAgentsDetailed::WriteRows(MTAPISTR& tag,MTAPIRES& retcode)
  {
   uint32_t  login_current=0,deal_current=0;
   uint64_t    login_last   =0;
   uint32_t  line         =0;
   IMTDeal  *commission   =NULL;
   uint32_t  counter      =0;
   bool      agent_changed=false,no_commissions=false;
   UserInfo  agent_info   ={0};
   double    subtotal     =0.0;
//--- get report logins
   if(m_logins==NULL && m_logins_total==0)
      if((retcode=m_api->ParamLogins(m_logins,m_logins_total))!=MT_RET_OK)
         return(true);
//--- all messages
   while(CMTStr::CompareNoCase(tag,L"commissions")==0 && !no_commissions)
     {
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- get next commission deal
      if(!agent_changed)
         commission=CommissionNext(m_logins,m_logins_total,login_current,deal_current);
      else
         agent_changed=false;
      //--- agent changed
      if(!commission || commission->Login()!=login_last)
         agent_changed=true;
      //--- process different commissions rows
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         //--- table is empty
         if(CMTStr::CompareNoCase(tag,L"no_commissions")==0)
           {
            if(counter==0 && commission==NULL && line==0)
              {
               retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
               no_commissions=true;
              }
            continue;
           }
         //--- instant commission
         if(CMTStr::CompareNoCase(tag,L"commission_instant")==0)
           {
            if(!agent_changed && commission && commission->Action()==IMTDeal::DEAL_AGENT && WriteRowInstant(retcode,commission,line,agent_info,subtotal))
              {
               if(retcode!=MT_RET_OK)
                  return(true);
               line++;
              }
            continue;
           }
         //--- daily or monthly commission
         if(CMTStr::CompareNoCase(tag,L"commission_daily")==0)
           {
            if(!agent_changed && commission && (commission->Action()==IMTDeal::DEAL_AGENT_DAILY || commission->Action()==IMTDeal::DEAL_AGENT_MONTHLY) && WriteRowDaily(retcode,commission,line,agent_info,subtotal))
              {
               if(retcode!=MT_RET_OK)
                  return(true);
               line++;
              }
            continue;
           }
         //--- per agent commissions subtotal
         if(CMTStr::CompareNoCase(tag,L"commission_subtotal")==0)
           {
            if(agent_changed && line>0 && WriteRowSubtotal(retcode,agent_info,subtotal))
              {
               if(retcode!=MT_RET_OK)
                  return(true);
              }
            continue;
           }
         break;
        }
      //--- remember new agent
      if(commission && agent_changed)
        {
         //--- remember new agent
         login_last=commission->Login();
         //--- reset agent commission
         subtotal=0.0;
         //--- get agent info
         GetUserInfo(login_last,agent_info);
        }
      //--- there is no more commissions
      if(!commission)
         break;
     }
//--- all right
   return(true);
  }
//+------------------------------------------------------------------+
//| Write summary records                                            |
//+------------------------------------------------------------------+
bool CAgentsDetailed::WriteSummaries(MTAPISTR& tag,MTAPIRES& retcode)
  {
   uint32_t counter=0;
//--- there is no summaries
   if(m_summaries.Total()==0)
     {
      retcode=MT_RET_OK;
      return(true);
     }
//--- all messages
   while(CMTStr::CompareNoCase(tag,L"summary")==0 && counter<m_summaries.Total())
     {
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      uint32_t save_counter=counter;
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         //--- caption "Total" in first string
         if(CMTStr::CompareNoCase(tag,L"caption")==0)
           {
            if(save_counter==0 && (retcode=m_api->HtmlWriteSafe(L"Total:",IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- total agent commission value
         if(CMTStr::CompareNoCase(tag,L"agent_commission")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%.2lf",m_summaries[save_counter].total))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- currency
         if(CMTStr::CompareNoCase(tag,L"currency")==0)
           {
            if((retcode=m_api->HtmlWriteSafe(m_summaries[save_counter].currency,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
            continue;
           }
         break;
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write instant agent commission row                               |
//+------------------------------------------------------------------+
bool CAgentsDetailed::WriteRowInstant(MTAPIRES& retcode,const IMTDeal* commission,const uint32_t line,const UserInfo& agent_info,double& subtotal)
  {
   UserInfo  user_info={0};
   CMTStr256 str;
   uint64_t    login=0,ticket=0;
   int32_t       pos=0;
   uint32_t  counter=0;
   bool      has_deal=false;
   MTAPISTR  tag;
//--- check
   if(!commission)
      return(false);
//--- get commission comment
   str.Assign(commission->Comment());
//--- parse commission deal comment ("agent '2048' - #103748")
   if((pos=str.Find(L"agent '"))>=0)
      login=_wtoi64(str.Str()+pos+CMTStr::Len(L"agent '"));
   if((pos=str.Find(L"' - #"))>=0)
      ticket=_wtoi64(str.Str()+pos+CMTStr::Len(L"' - #"));
//--- get user info
   GetUserInfo(login,user_info);
//--- get user deal related to this commission
   if((retcode=m_api->DealGet(ticket,m_deal))==MT_RET_OK)
      has_deal=true;
   else
     {
      m_api->LoggerOut(MTLogWarn,L"failed to get deal #%I64u for login '%I64u' [%u]",ticket,login,retcode);
      retcode=MT_RET_OK;
     }
//--- process tag
   if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
      return(true);
//--- process cells in row
   while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
     {
      //--- print the color of string
      if(CMTStr::CompareNoCase(tag,L"line")==0)
        {
         if((retcode=m_api->HtmlWrite(L"%u",line%2))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- deal
      if(CMTStr::CompareNoCase(tag,L"deal")==0)
        {
         if((retcode=m_api->HtmlWrite(L"%I64u",ticket))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- account
      if(CMTStr::CompareNoCase(tag,L"account")==0)
        {
         if((retcode=m_api->HtmlWrite(L"%I64u",login))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- name
      if(CMTStr::CompareNoCase(tag,L"name")==0)
        {
         if((retcode=m_api->HtmlWriteSafe(user_info.name,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- time
      if(CMTStr::CompareNoCase(tag,L"time")==0)
        {
         if((retcode=m_api->HtmlWriteSafe(SMTFormat::FormatDateTime(str,commission->Time(),true,true),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- position
      if(CMTStr::CompareNoCase(tag,L"position")==0)
        {
         if(has_deal)
           {
            if(m_deal && (retcode=m_api->HtmlWrite(L"%I64u",m_deal->PositionID()))!=MT_RET_OK)
               return(true);
           }
         else
           {
            if((retcode=m_api->HtmlWrite(L"&nbsp;"))!=MT_RET_OK)
               return(true);
           }
         continue;
        }
      //--- symbol
      if(CMTStr::CompareNoCase(tag,L"symbol")==0)
        {
         if(has_deal)
           {
            if(m_deal && (retcode=m_api->HtmlWriteSafe(m_deal->Symbol(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
           }
         else
           {
            if((retcode=m_api->HtmlWrite(L"&nbsp;"))!=MT_RET_OK)
               return(true);
           }
         continue;
        }
      //--- volume
      if(CMTStr::CompareNoCase(tag,L"volume")==0)
        {
         if(has_deal)
           {
            if(m_deal && (retcode=m_api->HtmlWrite(L"%.2lf",SMTMath::VolumeToDouble(m_deal->Volume())))!=MT_RET_OK)
               return(true);
           }
         else
           {
            if((retcode=m_api->HtmlWrite(L"&nbsp;"))!=MT_RET_OK)
               return(true);
           }
         continue;
        }
      //--- action
      if(CMTStr::CompareNoCase(tag,L"action")==0)
        {
         if(has_deal)
           {
            if(m_deal && (retcode=m_api->HtmlWriteSafe(SMTFormat::FormatDealAction(str,m_deal->Action()),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
           }
         else
           {
            if((retcode=m_api->HtmlWrite(L"&nbsp;"))!=MT_RET_OK)
               return(true);
           }
         continue;
        }
      //--- entry
      if(CMTStr::CompareNoCase(tag,L"entry")==0)
        {
         if(has_deal)
           {
            if(m_deal && (retcode=m_api->HtmlWriteSafe(SMTFormat::FormatDealEntry(str,m_deal->Entry()),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
           }
         else
           {
            if((retcode=m_api->HtmlWrite(L"&nbsp;"))!=MT_RET_OK)
               return(true);
           }

         continue;
        }
      //--- price
      if(CMTStr::CompareNoCase(tag,L"price")==0)
        {
         if(has_deal)
           {
            if(m_deal && (retcode=m_api->HtmlWriteSafe(SMTFormat::FormatPrice(str,m_deal->Price(),m_deal->Digits()),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
           }
         else
           {
            if((retcode=m_api->HtmlWrite(L"&nbsp;"))!=MT_RET_OK)
               return(true);
           }
         continue;
        }
      //--- commission
      if(CMTStr::CompareNoCase(tag,L"commission")==0)
        {
         if(has_deal)
           {
            if(m_deal && (retcode=m_api->HtmlWriteSafe(SMTFormat::FormatDouble(str,m_deal->Commission(),user_info.currency_digits),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
           }
         else
           {
            if((retcode=m_api->HtmlWrite(L"&nbsp;"))!=MT_RET_OK)
               return(true);
           }
         continue;
        }
      //--- type
      if(CMTStr::CompareNoCase(tag,L"type")==0)
        {
         if((retcode=m_api->HtmlWriteSafe(SMTFormat::FormatDealAction(str,commission->Action()),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- agent commission
      if(CMTStr::CompareNoCase(tag,L"agent_commission")==0)
        {
         if((retcode=m_api->HtmlWriteSafe(SMTFormat::FormatDouble(str,commission->Profit(),agent_info.currency_digits),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(true);
         //--- collect subtotal value
         subtotal+=commission->Profit();
         //--- collect total value
         if(!CollectSummary(agent_info.currency,commission->Profit()))
           {
            retcode=MT_RET_ERR_MEM;
            return(true);
           }
         continue;
        }
      //--- currency
      if(CMTStr::CompareNoCase(tag,L"currency")==0)
        {
         if((retcode=m_api->HtmlWriteSafe(agent_info.currency,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(true);
         continue;
        }
      break;
     }
//--- all right
   return(true);
  }
//+------------------------------------------------------------------+
//| Write daily agent commission row                                 |
//+------------------------------------------------------------------+
bool CAgentsDetailed::WriteRowDaily(MTAPIRES& retcode,const IMTDeal* commission,const uint32_t line,const UserInfo& agent_info,double& subtotal)
  {
   CMTStr256 str;
   uint64_t    login=0;
   uint32_t  counter=0;
   int32_t       pos=0;
   MTAPISTR  tag;
//--- check
   if(!commission)
      return(false);
//--- get commission comment
   str.Assign(commission->Comment());
//--- parse commission deal comment ("agent from '2048' [...]")
   if((pos=str.Find(L"agent from '"))>=0)
      login=_wtoi64(str.Str()+pos+CMTStr::Len(L"agent from '"));
//--- process tag
   if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
      return(true);
//--- process cells in row
   while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
     {
      //--- print the color of string
      if(CMTStr::CompareNoCase(tag,L"line")==0)
        {
         if((retcode=m_api->HtmlWrite(L"%u",line%2))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- account
      if(CMTStr::CompareNoCase(tag,L"account")==0)
        {
         if((retcode=m_api->HtmlWrite(L"%I64u",login))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- name
      if(CMTStr::CompareNoCase(tag,L"name")==0)
        {
         if((retcode=m_api->HtmlWriteSafe(GetUserNameW(login,str),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- time
      if(CMTStr::CompareNoCase(tag,L"time")==0)
        {
         if((retcode=m_api->HtmlWriteSafe(SMTFormat::FormatDateTime(str,commission->Time(),true,true),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- type
      if(CMTStr::CompareNoCase(tag,L"type")==0)
        {
         if((retcode=m_api->HtmlWriteSafe(SMTFormat::FormatDealAction(str,commission->Action()),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- agent commission
      if(CMTStr::CompareNoCase(tag,L"agent_commission")==0)
        {
         if((retcode=m_api->HtmlWriteSafe(SMTFormat::FormatDouble(str,commission->Profit(),agent_info.currency_digits),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(true);
         //--- collect subtotal value
         subtotal+=commission->Profit();
         //--- collect total value
         if(!CollectSummary(agent_info.currency,commission->Profit()))
           {
            retcode=MT_RET_ERR_MEM;
            return(true);
           }
         continue;
        }
      //--- currency
      if(CMTStr::CompareNoCase(tag,L"currency")==0)
        {
         if((retcode=m_api->HtmlWriteSafe(agent_info.currency,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(true);
         continue;
        }
      break;
     }
//--- all right
   return(true);
  }
//+------------------------------------------------------------------+
//| Write agent subtotal row                                         |
//+------------------------------------------------------------------+
bool CAgentsDetailed::WriteRowSubtotal(MTAPIRES& retcode,const UserInfo& agent_info,const double subtotal)
  {
   CMTStr256 str;
   uint32_t  counter=0;
   MTAPISTR  tag;
//--- process tag
   if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
      return(true);
//--- process cells in row
   while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
     {
      //--- account
      if(CMTStr::CompareNoCase(tag,L"account")==0)
        {
         if((retcode=m_api->HtmlWrite(L"%I64u",agent_info.login))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- name
      if(CMTStr::CompareNoCase(tag,L"name")==0)
        {
         if((retcode=m_api->HtmlWriteSafe(agent_info.name,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- agent commission
      if(CMTStr::CompareNoCase(tag,L"agent_commission")==0)
        {
         if((retcode=m_api->HtmlWriteSafe(SMTFormat::FormatDouble(str,subtotal,agent_info.currency_digits),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(true);
         continue;
        }
      //--- currency
      if(CMTStr::CompareNoCase(tag,L"currency")==0)
        {
         if((retcode=m_api->HtmlWriteSafe(agent_info.currency,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(true);
         continue;
        }
      break;
     }
//--- all right
   return(true);
  }
//+------------------------------------------------------------------+
//| Get next commission deal                                         |
//+------------------------------------------------------------------+
IMTDeal* CAgentsDetailed::CommissionNext(uint64_t* logins,const uint32_t logins_total,uint32_t& login_current,uint32_t& deal_current)
  {
   IMTDeal *deal=NULL;
   bool     found=false;
//--- check
   if(!logins || logins_total==0 || login_current>=logins_total)
      return(NULL);
//--- get deals for first call
   if(login_current==0 && deal_current==0 && m_deals->Total()==0)
      if(m_api->DealGet(logins[login_current],m_api->ParamFrom(),m_api->ParamTo(),m_deals)!=MT_RET_OK)
         return(NULL);
//--- search for next agent commission deal
   while(!found && login_current<logins_total)
     {
      for(;!found && (deal=m_deals->Next(deal_current));deal_current++)
         if(deal->Action()==IMTDeal::DEAL_AGENT || deal->Action()==IMTDeal::DEAL_AGENT_DAILY || deal->Action()==IMTDeal::DEAL_AGENT_MONTHLY)
            found=true;
      if(!found)
        {
         login_current++;
         deal_current=0;
         //--- get deals for next login
         if(login_current<logins_total)
            if(m_api->DealGet(logins[login_current],m_api->ParamFrom(),m_api->ParamTo(),m_deals)!=MT_RET_OK)
               return(NULL);
        }
     }
//--- return deal
   return(deal);
  }
//+------------------------------------------------------------------+
//| Ñollect summary for currency                                     |
//+------------------------------------------------------------------+
bool CAgentsDetailed::CollectSummary(LPCWSTR currency,const double value)
  {
   SummaryRecord record={0},*ptr;
//--- check
   if(!currency || value<0.0)
      return(false);
//--- fill record
   CMTStr::Copy(record.currency,currency);
   record.total=value;
//--- search summary
   if((ptr=m_summaries.Search(&record,SortSummary))!=NULL)
     {
      //--- update summary
      ptr->total+=record.total;
     }
   else
     {
      //--- insert new summary
      if(m_summaries.Insert(&record,SortSummary)==NULL)
         return(false);
     }
//--- all right
   return(true);
  }
//+------------------------------------------------------------------+
//| Get user name                                                    |
//+------------------------------------------------------------------+
LPCWSTR CAgentsDetailed::GetUserNameW(const uint64_t login,CMTStr& name)
  {
//--- clear name
   name.Clear();
//--- check if user is human (have valid login)
   if(m_api->UserGetLight(login,m_user)==MT_RET_OK)
      name.Assign(m_user->Name());
   else
      name.Assign(UNKNOWN_NAME);
//--- return name
   return(name.Str());
  }
//+------------------------------------------------------------------+
//| Get user info                                                    |
//+------------------------------------------------------------------+
void CAgentsDetailed::GetUserInfo(const uint64_t login,UserInfo& info)
  {
//--- clear info
   ZeroMemory(&info,sizeof(info));
//--- fill with default values
   info.login=login;
   CMTStr::Copy(info.name,UNKNOWN_NAME);
   CMTStr::Copy(info.currency,UNKNOWN_CURRENCY);
   info.currency_digits=DEFAULT_DIGITS;
//--- check if user is human (have valid login)
   if(m_api->UserGetLight(login,m_user)==MT_RET_OK)
     {
      CMTStr::Copy(info.name,m_user->Name());
      //--- get user group
      if(m_api->GroupGetLight(m_user->Group(),m_group)==MT_RET_OK)
        {
         //--- get group deposit currency and digits
         CMTStr::Copy(info.currency,m_group->Currency());
         info.currency_digits=m_group->CurrencyDigits();
        }
     }
  }
//+------------------------------------------------------------------+
//| Sort summary                                                     |
//+------------------------------------------------------------------+
int32_t CAgentsDetailed::SortSummary(const void *left,const void *right)
  {
   SummaryRecord *lft=(SummaryRecord*)left;
   SummaryRecord *rgh=(SummaryRecord*)right;
//--- sort by currency
   return(CMTStr::CompareNoCase(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+
