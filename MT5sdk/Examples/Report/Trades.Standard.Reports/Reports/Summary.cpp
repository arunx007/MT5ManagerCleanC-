//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Summary.h"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
MTReportInfo CSummary::s_info=
  {
   100,
   MTReportAPIVersion,
   0,
   L"Summary",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_USERS|MTReportInfo::SNAPSHOT_ACCOUNTS,
   MTReportInfo::TYPE_TABLE,
   L"Trades",
                    // params
     {{ MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, L"*" },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM         },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_TO           },
     },3            // params_total
  };
//+------------------------------------------------------------------+
//| Column descriptions                                              |
//+------------------------------------------------------------------+
ReportColumn CSummary::s_columns[]=
  {
   //--- id,                 name,             type,                         width,  width_max, offset,                          size,                              digits_column,          flags
     { COLUMN_LOGIN,         L"Login",         IMTDatasetColumn::TYPE_USER_LOGIN, 10,0,  offsetof(TableRecord,login),           0,                                 0,                      IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_NAME,            L"Name",          IMTDatasetColumn::TYPE_STRING,     20,0,  offsetof(TableRecord,name),            MtFieldSize(TableRecord,name),     0,                      0                             },
   { COLUMN_DEPOSIT,         L"Deposit",       IMTDatasetColumn::TYPE_MONEY,      10,0,  offsetof(TableRecord,deposit),         0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_WITHDRAW,        L"Withdraw",      IMTDatasetColumn::TYPE_MONEY,      10,0,  offsetof(TableRecord,withdraw),        0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_IN_OUT,          L"In/Out",        IMTDatasetColumn::TYPE_MONEY,      10,0,  offsetof(TableRecord,inout),           0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_CREDIT,          L"Credit",        IMTDatasetColumn::TYPE_MONEY,      10,0,  offsetof(TableRecord,credit),          0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_ADDITIONAL,      L"Additional",    IMTDatasetColumn::TYPE_MONEY,      10,0,  offsetof(TableRecord,additional),      0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_VOLUME,          L"Volume",        IMTDatasetColumn::TYPE_VOLUME_EXT, 10,0,  offsetof(TableRecord,volume),          0,                                 0,                      0                             },
   { COLUMN_COMMISSION,      L"Commission",    IMTDatasetColumn::TYPE_MONEY,      10,0,  offsetof(TableRecord,commission),      0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_FEE,             L"Fee",           IMTDatasetColumn::TYPE_MONEY,      10,0,  offsetof(TableRecord,fee),             0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_STORAGE,         L"Swap",          IMTDatasetColumn::TYPE_MONEY,      10,0,  offsetof(TableRecord,storage),         0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_PROFIT,          L"Profit",        IMTDatasetColumn::TYPE_MONEY,      10,0,  offsetof(TableRecord,profit),          0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_BALANCE,         L"Cur. Balance",  IMTDatasetColumn::TYPE_MONEY,      10,0,  offsetof(TableRecord,balance_cur),     0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_EQUITY,          L"Cur. Equity",   IMTDatasetColumn::TYPE_MONEY,      10,0,  offsetof(TableRecord,equity_cur),      0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_CURRENCY,        L"Currency",      IMTDatasetColumn::TYPE_STRING,     9,70,  offsetof(TableRecord,currency),        MtFieldSize(TableRecord,currency), 0,                      0                             },
   { COLUMN_CURRENCY_DIGITS, L"CurrencyDigits",IMTDatasetColumn::TYPE_UINT32,     0, 0,  offsetof(TableRecord,currency_digits), 0,                                 0,                      IMTDatasetColumn::FLAG_HIDDEN  }
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CSummary::CSummary(void) : m_api(NULL),m_deals(NULL),m_account(NULL),m_user(NULL),m_group(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CSummary::~CSummary(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CSummary::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release plug-in                                                  |
//+------------------------------------------------------------------+
void CSummary::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CSummary::Generate(const uint32_t type,IMTReportAPI *api)
  {
   uint64_t  *logins=NULL;
   uint32_t logins_total=0;
   MTAPIRES res;
//--- clear
   Clear();
//--- checks
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- checks
   if(type!=MTReportInfo::TYPE_TABLE)
      return(MT_RET_ERR_NOTIMPLEMENT);
//---
   m_api=api;
//--- prepare table
   if((res=TablePrepare())!=MT_RET_OK)
     {
      Clear();
      return(res);
     }
//--- get report logins
   if((res=api->ParamLogins(logins,logins_total))!=MT_RET_OK)
     {
      Clear();
      return(res);
     }
//--- checks logins
   if(logins && logins_total)
     {
      //--- prepare deals array & user & group
      if((m_deals  =api->DealCreateArray())  ==NULL ||
         (m_account=api->UserCreateAccount())==NULL ||
         (m_user   =api->UserCreate())       ==NULL ||
         (m_group  =api->GroupCreate())      ==NULL)
        {
         api->Free(logins);
         Clear();
         return(MT_RET_ERR_MEM);
        }
      //--- write table
      for(uint32_t i=0;i<logins_total;i++)
         if((res=TableWrite(logins[i]))!=MT_RET_OK)
           {
            api->Free(logins);
            Clear();
            return(res);
           }
     }
//--- free logins
   if(logins)
      api->Free(logins);
//--- write table summaries
   if((res=TableWriteSummaries())!=MT_RET_OK)
     {
      Clear();
      return(res);
     }
//--- clear 
   Clear();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Cleanup                                                          |
//+------------------------------------------------------------------+
void CSummary::Clear(void)
  {
//--- deals array interface
   if(m_deals)
     {
      m_deals->Release();
      m_deals=NULL;
     }
//--- account interface
   if(m_account)
     {
      m_account->Release();
      m_account=NULL;
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
//--- summaries
   m_summaries.Clear();
//--- API interface
   m_api=NULL;
  }
//+------------------------------------------------------------------+
//| Prepare table columns                                            |
//+------------------------------------------------------------------+
MTAPIRES CSummary::TablePrepare(void)
  {
   IMTDatasetColumn *column;
   MTAPIRES         res;
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create column
   if((column=m_api->TableColumnCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- clear table columns
   m_api->TableColumnClear();
//--- add column description for TableRecord
   for(uint32_t i=0;i<_countof(s_columns);i++)
     {
      //--- clear
      column->Clear();
      //--- fill info
      column->ColumnID(s_columns[i].id);
      column->Name(s_columns[i].name);
      column->Type(s_columns[i].type);
      column->Width(s_columns[i].width);
      column->WidthMax(s_columns[i].width_max);
      column->Offset(s_columns[i].offset);
      column->Size(s_columns[i].size);
      column->DigitsColumn(s_columns[i].digits_column);
      column->Flags(s_columns[i].flags);
      //--- add column
      if((res=m_api->TableColumnAdd(column))!=MT_RET_OK)
        {
         column->Release();
         return(res);
        }
     }
//--- release column
   column->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write table                                                      |
//+------------------------------------------------------------------+
MTAPIRES CSummary::TableWrite(const uint64_t login)
  {
   uint32_t    deal_total,i;
   TableRecord record={0},*ptr;
   MTAPIRES    res;
//--- checks
   if(!m_api || !login || !m_deals || !m_account || !m_user || !m_group)
      return(MT_RET_ERR_PARAMS);
//--- get deals
   if((res=m_api->DealGet(login,m_api->ParamFrom(),m_api->ParamTo(),m_deals))!=MT_RET_OK)
      return(res);
//--- get user info
   if((res=m_api->UserGetLight(login,m_user))!=MT_RET_OK)
      return(res);
//--- get user group info
   if((res=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
      return(res);
//--- get account info
   if((res=m_api->UserAccountGet(login,m_account))!=MT_RET_OK)
     {
      if(res!=MT_RET_ERR_NOTFOUND)
         return(res);
      //--- balance and equity
      record.balance_cur=m_user->Balance();
      record.equity_cur =SMTMath::MoneyAdd(record.balance_cur,m_user->Credit(),m_group->CurrencyDigits());
     }
   else
     {
      //--- balance and equity
      record.balance_cur=m_account->Balance();
      record.equity_cur =m_account->Equity();
     }
//--- fill record
   record.login          =login;
   CMTStr::Copy(record.name,m_user->Name());
   CMTStr::Copy(record.currency,m_group->Currency());
   record.currency_digits=m_group->CurrencyDigits();
//---
   for(i=0,deal_total=m_deals->Total();i<deal_total;i++)
     {
      IMTDeal *deal;
      //--- get deal
      if((deal=m_deals->Next(i))==NULL)
         continue;
      //---
      switch(deal->Action())
        {
         //--- buy or sell
         case IMTDeal::DEAL_BUY :
         case IMTDeal::DEAL_SELL:
            record.profit     =SMTMath::MoneyAdd(record.profit    ,deal->Profit()    ,record.currency_digits);
            record.storage    =SMTMath::MoneyAdd(record.storage   ,deal->Storage()   ,record.currency_digits);
            record.commission =SMTMath::MoneyAdd(record.commission,deal->Commission(),record.currency_digits);
            record.fee        =SMTMath::MoneyAdd(record.fee       ,deal->Fee()       ,record.currency_digits);
            record.volume    +=deal->VolumeExt();
            break;
            //--- balance
         case IMTDeal::DEAL_BALANCE:
            if(deal->Profit()>=0)
               record.deposit =SMTMath::MoneyAdd(record.deposit ,deal->Profit(),record.currency_digits);
            else
               record.withdraw=SMTMath::MoneyAdd(record.withdraw,deal->Profit(),record.currency_digits);
            break;
            //--- credit
         case IMTDeal::DEAL_CREDIT:
            record.credit=SMTMath::MoneyAdd(record.credit,deal->Profit(),record.currency_digits);
            break;
            //--- commissions
         case IMTDeal::DEAL_COMMISSION:
         case IMTDeal::DEAL_COMMISSION_DAILY:
         case IMTDeal::DEAL_COMMISSION_MONTHLY:
            record.commission=SMTMath::MoneyAdd(record.commission,deal->Profit(),record.currency_digits);
            break;
            //--- additional
         case IMTDeal::DEAL_CHARGE:
         case IMTDeal::DEAL_CORRECTION:
         case IMTDeal::DEAL_BONUS:
         case IMTDeal::DEAL_AGENT_DAILY:
         case IMTDeal::DEAL_AGENT_MONTHLY:
         case IMTDeal::DEAL_INTERESTRATE:
         case IMTDeal::DEAL_DIVIDEND:
         case IMTDeal::DEAL_DIVIDEND_FRANKED:
         case IMTDeal::DEAL_TAX:
         case IMTDeal::DEAL_AGENT:
         case IMTDeal::DEAL_SO_COMPENSATION:
         case IMTDeal::DEAL_SO_COMPENSATION_CREDIT:
            record.additional=SMTMath::MoneyAdd(record.additional,deal->Profit(),record.currency_digits);
            break;
        }
     }
//--- in/out
   record.inout=SMTMath::MoneyAdd(record.deposit,record.withdraw,record.currency_digits);
//--- write row
   if((res=m_api->TableRowWrite(&record,sizeof(record)))!=MT_RET_OK)
      return(res);
//--- search summary
   if((ptr=m_summaries.Search(&record,SortSummary))!=NULL)
     {
      //--- update summaries
      ptr->deposit    =SMTMath::MoneyAdd(ptr->deposit    ,record.deposit    ,ptr->currency_digits);
      ptr->withdraw   =SMTMath::MoneyAdd(ptr->withdraw   ,record.withdraw   ,ptr->currency_digits);
      ptr->inout      =SMTMath::MoneyAdd(ptr->inout      ,record.inout      ,ptr->currency_digits);
      ptr->credit     =SMTMath::MoneyAdd(ptr->credit     ,record.credit     ,ptr->currency_digits);
      ptr->additional =SMTMath::MoneyAdd(ptr->additional ,record.additional ,ptr->currency_digits);
      ptr->commission =SMTMath::MoneyAdd(ptr->commission ,record.commission ,ptr->currency_digits);
      ptr->fee        =SMTMath::MoneyAdd(ptr->fee        ,record.fee        ,ptr->currency_digits);
      ptr->profit     =SMTMath::MoneyAdd(ptr->profit     ,record.profit     ,ptr->currency_digits);
      ptr->storage    =SMTMath::MoneyAdd(ptr->storage    ,record.storage    ,ptr->currency_digits);
      ptr->balance_cur=SMTMath::MoneyAdd(ptr->balance_cur,record.balance_cur,ptr->currency_digits);
      ptr->equity_cur =SMTMath::MoneyAdd(ptr->equity_cur ,record.equity_cur ,ptr->currency_digits);
      //---
      ptr->volume+=record.volume;
     }
   else
     {
      //--- insert new summary
      if(m_summaries.Insert(&record,SortSummary)==NULL)
         return(MT_RET_ERR_MEM);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write summary                                                    |
//+------------------------------------------------------------------+
MTAPIRES CSummary::TableWriteSummaries(void)
  {
   IMTDatasetSummary *summary=NULL;
   uint32_t          i;
   MTAPIRES          res;
//--- checks
   if(!m_api)
      return(MT_RET_ERR_MEM);
//--- create summary
   if((summary=m_api->TableSummaryCreate())==NULL)
      return(MT_RET_ERR_MEM);
//---
   for(i=0;i<m_summaries.Total();i++)
     {
      //--- clear summary
      summary->Clear();
      //--- total text
      summary->ColumnID(COLUMN_LOGIN);
      summary->MergeColumn(COLUMN_NAME);
      summary->Line(i);
      summary->ValueString(i==0 ? L"Total" : L"");
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- clear summary
      summary->Clear();
      //--- total deposit
      summary->ColumnID(COLUMN_DEPOSIT);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].deposit);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total withdraw
      summary->ColumnID(COLUMN_WITHDRAW);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].withdraw);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total In/Out
      summary->ColumnID(COLUMN_IN_OUT);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].inout);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total credit
      summary->ColumnID(COLUMN_CREDIT);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].credit);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total additional
      summary->ColumnID(COLUMN_ADDITIONAL);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].additional);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total commission
      summary->ColumnID(COLUMN_COMMISSION);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].commission);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total fee
      summary->ColumnID(COLUMN_FEE);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].fee);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total profit
      summary->ColumnID(COLUMN_PROFIT);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].profit);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total storage
      summary->ColumnID(COLUMN_STORAGE);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].storage);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total current balance
      summary->ColumnID(COLUMN_BALANCE);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].balance_cur);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total current equity
      summary->ColumnID(COLUMN_EQUITY);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].equity_cur);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total volume
      summary->ColumnID(COLUMN_VOLUME);
      summary->Line(i);
      summary->ValueVolumeExt(m_summaries[i].volume);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- clear summary
      summary->Clear();
      //--- currency
      summary->ColumnID(COLUMN_CURRENCY);
      summary->Line(i);
      summary->ValueString(m_summaries[i].currency);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
     }
//--- release summary
   summary->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Sort summary                                                     |
//+------------------------------------------------------------------+
int32_t CSummary::SortSummary(const void *left,const void *right)
  {
   TableRecord *lft=(TableRecord*)left;
   TableRecord *rgh=(TableRecord*)right;
//--- sort by currency
   return(CMTStr::CompareNoCase(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+
