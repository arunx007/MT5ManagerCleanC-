//+------------------------------------------------------------------+
//|                           MetaTrader 5 Gateways.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Turnover.h"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
MTReportInfo CTurnover::s_info=
  {
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_ANY,
   L"Gateways Turnover",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_NONE,
   MTReportInfo::TYPE_HTML,
   L"Gateways",
                   // params
     {{ MTReportParam::TYPE_DATE,MTAPI_PARAM_GROUPS, L"!demo*,!contest*,*" },
      { MTReportParam::TYPE_DATE,MTAPI_PARAM_FROM,   L"" },
      { MTReportParam::TYPE_DATE,MTAPI_PARAM_TO,     L"" },
     },3,          // params_total
     {             // configs
      { MTReportParam::TYPE_STRING,L"Currency",DEFAULT_CURRENCY },
     },1           // configs_total     
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CTurnover::CTurnover(void) : m_api(NULL),m_report(NULL),m_param(NULL),m_group(NULL),m_symbol(NULL),m_deals(NULL),m_currency_digits(0)
  {
   ZeroMemory(m_currency,sizeof(m_currency));
  }
//+------------------------------------------------------------------+
//| destructor                                                       |
//+------------------------------------------------------------------+
CTurnover::~CTurnover(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CTurnover::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release                                                          |
//+------------------------------------------------------------------+
void CTurnover::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CTurnover::Generate(const uint32_t type,IMTReportAPI *api)
  {
   MTAPIRES res=MT_RET_OK;
//--- clear all
   Clear();
//--- checks
   if(!api)                          return(MT_RET_ERR_PARAMS);
   if(type!=MTReportInfo::TYPE_HTML) res=MT_RET_ERR_NOTIMPLEMENT;
//--- save api pointer;
   m_api=api;
//--- creating interfaces
   if(res==MT_RET_OK)
      if((m_report=m_api->ReportCreate())   ==NULL ||
         (m_param =m_api->ParamCreate())    ==NULL ||
         (m_group =m_api->GroupCreate())    ==NULL ||
         (m_symbol=m_api->SymbolCreate())   ==NULL ||
         (m_deals =m_api->DealCreateArray())==NULL)
         res=MT_RET_ERR_MEM;
//--- get currency from config
   if(res==MT_RET_OK)
      if(m_api->ReportCurrent(m_report)==MT_RET_OK && m_report->ParameterGet(L"Currency",m_param)==MT_RET_OK)
        {
         CMTStr::Copy(m_currency,m_param->ValueString());
         CMTStr::ToUpper(m_currency);
        }
//--- checks
   if(m_currency[0]==L'\0') CMTStr::Copy(m_currency,DEFAULT_CURRENCY);
//--- setup digits
   m_currency_digits=SMTMath::MoneyDigits(m_currency);
//--- collecting data
   if(res==MT_RET_OK)
      res=GenerateCollect();
//--- generating HTML
   if(res==MT_RET_OK)
      res=GenerateHtml();
//--- show error page if error exists
   switch(res)
     {
      //--- all right
      case MT_RET_OK:
         break;
         //--- logins array is empty
      case MT_RET_OK_NONE:
         res=CReportError::Write(api,L"Gateways Turnover Repost",L"Report is empty for the group, that you use.");
         break;
         //--- some error
      default:
         res=CReportError::Write(api,L"Gateways Turnover Repost",L"Report generation failed. For more information see server's journal.");
     }
//--- ok
   return(res);
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CTurnover::Clear(void)
  {
//--- check report
   if(m_report)
     {
      m_report->Release();
      m_report=NULL;
     }
//--- check param
   if(m_param)
     {
      m_param->Release();
      m_param=NULL;
     }
//--- check group
   if(m_group)
     {
      m_group->Release();
      m_group=NULL;
     }
//--- check symbol
   if(m_symbol)
     {
      m_symbol->Release();
      m_symbol=NULL;
     }
//--- check deals
   if(m_deals)
     {
      m_deals->Release();
      m_deals=NULL;
     }
//--- currency
   ZeroMemory(m_currency,sizeof(m_currency));
   m_currency_digits=0;
//--- clear turnovers
   m_turnovers.Clear();
//--- api interface
   m_api=NULL;
  }
//+------------------------------------------------------------------+
//| Collecting data                                                  |
//+------------------------------------------------------------------+
MTAPIRES CTurnover::GenerateCollect(void)
  {
//--- checks
   if(!m_api || !m_group || !m_symbol)
      return(MT_RET_ERR_PARAMS);
//--- get logins list
   uint64_t       *logins=NULL;
   uint32_t      logins_total=0;
   RateInfo      rate={},*rate_ptr;
   RateInfoArray rates;
   MTAPIRES      res;

   if((res=m_api->ParamLogins(logins,logins_total))!=MT_RET_OK)
     {
      if(logins) m_api->Free(logins);
      return(res);
     }
//--- if logins empty
   if(!logins_total)
      return(MT_RET_OK_NONE);
//--- checks
   if(logins)
     {
      //--- iterate logins
      for(uint32_t i=0;i<logins_total;i++)
         if((res=GenerateUser(logins[i]))!=MT_RET_OK)
           {
            m_api->Free(logins);
            return(res);
           }
      //--- free logins
      m_api->Free(logins);
     }
//--- finalize amounts and rates results
   for(uint32_t i=0;i<m_turnovers.Total();i++)
     {
      //--- reference
      Turnover &turnover=m_turnovers[i];
      //--- clear currency
      ZeroMemory(&turnover.amount_currency,sizeof(turnover.amount_currency));
      //--- clear rate and amount
      turnover.amount_rate=turnover.amount=0;
      //--- get symbol
      if(m_api->SymbolGet(turnover.symbol,m_symbol)==MT_RET_OK)
        {
         //--- store base currency
         CMTStr::Copy(turnover.amount_currency,m_symbol->CurrencyBase());
         //--- search ready rate
         if((rate_ptr=rates.Search(turnover.amount_currency,SearchRates))==NULL)
           {
            //--- set base currency
            CMTStr::Copy(rate.currency,turnover.amount_currency);
            //--- prepare buy rate
            if(m_api->TradeRateBuy(turnover.amount_currency,m_currency,rate.rate_buy)!=MT_RET_OK) rate.rate_buy=0;
            //--- prepare sell rate
            if(m_api->TradeRateSell(turnover.amount_currency,m_currency,rate.rate_sell)!=MT_RET_OK) rate.rate_sell=0;
            //--- insert
            rate_ptr=rates.Insert(&rate,SortRates);
           }
         //--- check rate
         if(rate_ptr)
           {
            //--- setup rate
            if(turnover.amount_raw>=0)
               turnover.amount_rate=rate_ptr->rate_sell;
            else
               turnover.amount_rate=rate_ptr->rate_buy;
            //--- calculate amount in our currency (USD)
            turnover.amount=SMTMath::PriceNormalize(turnover.amount_rate*turnover.amount_raw,m_currency_digits);
           }
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Collecting data by user                                          |
//+------------------------------------------------------------------+
MTAPIRES CTurnover::GenerateUser(const uint64_t login)
  {
   IMTDeal *deal;
   uint32_t total,i;
   Turnover turnover,*ptr;
   MTAPIRES res;
//--- checks
   if(!m_api || !m_symbol || !m_deals) return(MT_RET_ERR_PARAMS);
//--- get deals by login
   if((res=m_api->DealGet(login,m_api->ParamFrom(),m_api->ParamTo(),m_deals))!=MT_RET_OK)
      return(res);
//--- iterate deals
   for(i=0,total=m_deals->Total();i<total;i++)
     {
      //--- check
      if((deal=m_deals->Next(i))==NULL) continue;
      //--- check deal
      if(deal->Action()!=IMTDeal::DEAL_BUY && deal->Action()!=IMTDeal::DEAL_SELL) continue;
      //--- zero turnover
      ZeroMemory(&turnover,sizeof(turnover));
      //--- copy gateway
      CMTStr::Copy(turnover.gateway,deal->Gateway());
      turnover.gateway_id=deal->Dealer();
      //--- check gateway
      if(turnover.gateway[0]==L'\0' && turnover.gateway_id!=DEALER_ECN_LOGIN) continue;
      //--- get symbol
      if(m_api->SymbolGetLight(deal->Symbol(),m_symbol)!=MT_RET_OK) continue;
      //--- prepare turnover
      CMTStr::Copy(turnover.symbol,deal->Symbol());
      turnover.lots=deal->Volume();
      turnover.deals=1;
      //--- calculate raw amount
      turnover.amount_raw=SGatewayUtils::CalcAmountRaw(m_symbol,deal);
      //--- search exist record
      if((ptr=m_turnovers.Search(&turnover,SortBySymbol))!=NULL)
        {
         //--- update exist record
         ptr->lots+=turnover.lots;
         ptr->amount_raw=SMTMath::PriceNormalize(ptr->amount_raw+turnover.amount_raw,m_symbol->CurrencyBaseDigits());
         ptr->deals++;
        }
      else
        {
         //--- insert new record
         if(m_turnovers.Insert(&turnover,SortBySymbol)==NULL) return(MT_RET_ERR_MEM);
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Generating HTML                                                  |
//+------------------------------------------------------------------+
MTAPIRES CTurnover::GenerateHtml(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- use template
   MTAPISTR    tag={};
   CMTStr64    tmp;
   Turnover    gateway={},summary={};
   uint32_t    counter=0,current=0,gateway_idx=0;
   MTAPIRES    res;

   if((res=m_api->HtmlTplLoadResource(IDR_TURNOVER,RT_HTML))!=MT_RET_OK)
      return(res);
//--- sort by amount
   m_turnovers.Sort(SortByAmount);
//--- process tags
   while(res==MT_RET_OK && (res=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
     {
      //--- report name
      if(CMTStr::CompareNoCase(tag,L"report_name")==0)
        {
         res=m_api->HtmlWriteSafe(s_info.name,IMTReportAPI::HTML_SAFE_USENOBSP);
         continue;
        }
      //--- company name
      if(CMTStr::CompareNoCase(tag,L"company")==0)
        {
         IMTConCommon *common=m_api->CommonCreate();
         //--- check
         if(common)
           {
            //--- get common and write owner name
            if(m_api->CommonGet(common)==MT_RET_OK)
               res=m_api->HtmlWriteSafe(common->Owner(),IMTReportAPI::HTML_SAFE_USENOBSP);
            common->Release();
           }
         continue;
        }
      //--- server name
      if(CMTStr::CompareNoCase(tag,L"server")==0)
        {
         MTReportServerInfo info={};
         //--- get info and write server name
         if(m_api->About(info)==MT_RET_OK)
            res=m_api->HtmlWriteSafe(info.platform_name,IMTReportAPI::HTML_SAFE_USENOBSP);
         continue;
        }
      //--- from
      if(CMTStr::CompareNoCase(tag,L"from")==0)
        {
         res=m_api->HtmlWriteString(SMTFormat::FormatDateTime(tmp,m_api->ParamFrom(),false,false));
         continue;
        }
      //--- to
      if(CMTStr::CompareNoCase(tag,L"to")==0)
        {
         res=m_api->HtmlWriteString(SMTFormat::FormatDateTime(tmp,m_api->ParamTo(),false,false));
         continue;
        }
      //--- groups
      if(CMTStr::CompareNoCase(tag,L"groups")==0)
        {
         if(m_param && m_api->ParamGet(MTAPI_PARAM_GROUPS,m_param)==MT_RET_OK)
            res=m_api->HtmlWriteSafe(m_param->ValueGroups(),IMTReportAPI::HTML_SAFE_NONE);
         continue;
        }
      //--- currency
      if(CMTStr::CompareNoCase(tag,L"currency")==0)
        {
         tmp.Assign(m_currency);
         tmp.ToUpper();
         res=m_api->HtmlWriteSafe(tmp.Str(),IMTReportAPI::HTML_SAFE_USENOBSP);
         continue;
        }
      //--- empty banner
      if(CMTStr::CompareNoCase(tag,L"empty_banner")==0)
        {
         //--- check
         if(!m_turnovers.Total() && !counter)
            res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
         continue;
        }
      //--- gateways
      if(CMTStr::CompareNoCase(tag,L"gateways")==0)
        {
         //--- get gateway index
         if(GatewayGet(counter,gateway_idx) && gateway_idx<m_turnovers.Total())
           {
            //--- clear gateway stat
            ZeroMemory(&gateway,sizeof(gateway));
            //--- setup gateway name
            CMTStr::Copy(gateway.gateway,m_turnovers[gateway_idx].gateway);
            //--- setup gateway id
            gateway.gateway_id=m_turnovers[gateway_idx].gateway_id;
            //--- process <mt5:gateways> tag
            res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
           }
         continue;
        }
      //--- gateway: name
      if(CMTStr::CompareNoCase(tag,L"gateway_name")==0)
        {
         //--- get gateway name
         if(!SGatewayUtils::GatewayName(m_api,gateway.gateway,gateway.gateway_id,tmp))
            tmp.Format(L"%s (%I64u)",gateway.gateway,gateway.gateway_id);
         //--- write gateway name
         res=m_api->HtmlWriteSafe(tmp.Str(),IMTReportAPI::HTML_SAFE_USENOBSP);
         continue;
        }
      //--- gateway: deals
      if(CMTStr::CompareNoCase(tag,L"gateway_deals")==0)
        {
         res=m_api->HtmlWrite(L"%u",gateway.deals);
         continue;
        }
      //--- gateway: lots
      if(CMTStr::CompareNoCase(tag,L"gateway_lots")==0)
        {
         res=m_api->HtmlWriteString(SMTFormat::FormatVolume(tmp,gateway.lots,false));
         continue;
        }
      //--- gateway: amount
      if(CMTStr::CompareNoCase(tag,L"gateway_amount")==0)
        {
         res=m_api->HtmlWriteString(SMTFormat::FormatMoney(tmp,gateway.amount,m_currency_digits));
         continue;
        }
      //--- summary: deals
      if(CMTStr::CompareNoCase(tag,L"summary_deals")==0)
        {
         res=m_api->HtmlWrite(L"%u",summary.deals);
         continue;
        }
      //--- summary: lots
      if(CMTStr::CompareNoCase(tag,L"summary_lots")==0)
        {
         res=m_api->HtmlWriteString(SMTFormat::FormatVolume(tmp,summary.lots,false));
         continue;
        }
      //--- summary: amount
      if(CMTStr::CompareNoCase(tag,L"summary_amount")==0)
        {
         res=m_api->HtmlWriteString(SMTFormat::FormatMoney(tmp,summary.amount,m_currency_digits));
         continue;
        }
      //--- symbols
      if(CMTStr::CompareNoCase(tag,L"symbols")==0)
        {
         //--- calculate current
         current=gateway_idx+counter;
         //--- checks
         if(current<m_turnovers.Total() && CMTStr::Compare(gateway.gateway,m_turnovers[current].gateway)==0 && gateway.gateway_id==m_turnovers[current].gateway_id)
           {
            //--- summary: deals
            gateway.deals+=m_turnovers[current].deals;
            summary.deals+=m_turnovers[current].deals;
            //--- summary: lots
            gateway.lots+=m_turnovers[current].lots;
            summary.lots+=m_turnovers[current].lots;
            //--- summary: amount
            gateway.amount=SMTMath::MoneyAdd(gateway.amount,m_turnovers[current].amount,m_currency_digits);
            summary.amount=SMTMath::MoneyAdd(summary.amount,m_turnovers[current].amount,m_currency_digits);
            //--- process <mt5:symbols> tag
            res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
           }
         continue;
        }
      //--- line position
      if(CMTStr::CompareNoCase(tag,L"line")==0)
        {
         res=m_api->HtmlWrite(L"%u",(current-gateway_idx)&1);
         continue;
        }
      //--- symbol
      if(CMTStr::CompareNoCase(tag,L"symbol")==0)
        {
         //--- checks
         if(counter<m_turnovers.Total())
            res=m_api->HtmlWriteSafe(m_turnovers[current].symbol,IMTReportAPI::HTML_SAFE_USENOBSP);
         continue;
        }
      //--- symbol: deals
      if(CMTStr::CompareNoCase(tag,L"deals")==0)
        {
         //--- checks
         if(counter<m_turnovers.Total())
            res=m_api->HtmlWrite(L"%u",m_turnovers[current].deals);
         continue;
        }
      //--- symbol: lots
      if(CMTStr::CompareNoCase(tag,L"lots")==0)
        {
         //--- checks
         if(counter<m_turnovers.Total())
            res=m_api->HtmlWriteString(SMTFormat::FormatVolume(tmp,m_turnovers[current].lots,false));
         continue;
        }
      //--- symbol: amount raw
      if(CMTStr::CompareNoCase(tag,L"amount_raw")==0)
        {
         //--- checks
         if(counter<m_turnovers.Total())
            res=m_api->HtmlWriteString(SMTFormat::FormatMoney(tmp,m_turnovers[current].amount_raw,SMTMath::MoneyDigits(m_turnovers[current].amount_currency)));
         continue;
        }
      //--- symbol: amount currency
      if(CMTStr::CompareNoCase(tag,L"amount_currency")==0)
        {
         //--- checks
         if(counter<m_turnovers.Total())
            res=m_api->HtmlWriteSafe(m_turnovers[current].amount_currency,IMTReportAPI::HTML_SAFE_USENOBSP);
         continue;
        }
      //--- symbol: amount rate
      if(CMTStr::CompareNoCase(tag,L"amount_rate")==0)
        {
         //--- checks
         if(counter<m_turnovers.Total())
            res=m_api->HtmlWriteString(SMTFormat::FormatPrice(tmp,m_turnovers[current].amount_rate,5));
         continue;
        }
      //--- symbol: amount
      if(CMTStr::CompareNoCase(tag,L"amount")==0)
        {
         //--- checks
         if(counter<m_turnovers.Total())
            res=m_api->HtmlWriteString(SMTFormat::FormatMoney(tmp,m_turnovers[current].amount,m_currency_digits));
         continue;
        }
      //--- skip other
      res=m_api->HtmlWriteSafe(L"",IMTReportAPI::HTML_SAFE_USENOBSP);
     }
//--- checks
   if(res!=MT_RET_REPORT_TEMPLATE_END) return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Get gateway index by gateway's position                          |
//+------------------------------------------------------------------+
bool CTurnover::GatewayGet(const uint32_t pos,uint32_t& index)
  {
   uint32_t total=0,i=0;
//--- clear index
   index=0;
//--- checks
   if(!m_turnovers.Total()) return(false);
//--- find gateway
   for(i=1;i<m_turnovers.Total() && total<pos;i++)
      if(CMTStr::Compare(m_turnovers[i].gateway,m_turnovers[i-1].gateway)!=0 || m_turnovers[i].gateway_id!=m_turnovers[i-1].gateway_id)
         total++;
//--- checks
   if(total==pos && (i-1)<m_turnovers.Total())
     {
      //--- store index
      index=i-1;
      //--- ok
      return(true);
     }
//--- not found
   return(false);
  }
//+------------------------------------------------------------------+
//| Sort by symbol                                                   |
//+------------------------------------------------------------------+
int32_t CTurnover::SortBySymbol(const void *left,const void *right)
  {
   Turnover *lft=(Turnover*)left;
   Turnover *rgh=(Turnover*)right;
   int32_t       res;
//--- compare by gateway
   if((res=CMTStr::Compare(lft->gateway,rgh->gateway))!=0) return(res);
//--- compare by id
   if(lft->gateway_id>rgh->gateway_id) return(1);
   if(lft->gateway_id<rgh->gateway_id) return(-1);
//--- compare by symbol
   return(CMTStr::Compare(lft->symbol,rgh->symbol));
  }
//+------------------------------------------------------------------+
//| Sort by amount                                                   |
//+------------------------------------------------------------------+
int32_t CTurnover::SortByAmount(const void *left,const void *right)
  {
   Turnover *lft=(Turnover*)left;
   Turnover *rgh=(Turnover*)right;
   int32_t       res;
//--- compare by gateway
   if((res=CMTStr::Compare(lft->gateway,rgh->gateway))!=0) return(res);
//--- compare by id
   if(lft->gateway_id>rgh->gateway_id) return(1);
   if(lft->gateway_id<rgh->gateway_id) return(-1);
//--- compare by amount (desc)
   if(lft->amount>rgh->amount) return(-1);
   if(lft->amount<rgh->amount) return(1);
//--- equals
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort rates                                                       |
//+------------------------------------------------------------------+
int32_t CTurnover::SortRates(const void *left,const void *right)
  {
   RateInfo *lft=(RateInfo*)left;
   RateInfo *rgh=(RateInfo*)right;
//--- compare by symbol
   return(CMTStr::Compare(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+
//| Search rates                                                     |
//+------------------------------------------------------------------+
int32_t CTurnover::SearchRates(const void *left,const void *right)
  {
   LPCWSTR   lft=(LPCWSTR)left;
   RateInfo *rgh=(RateInfo*)right;
//--- compare by symbol
   return(CMTStr::Compare(lft,rgh->currency));
  }
//+------------------------------------------------------------------+
