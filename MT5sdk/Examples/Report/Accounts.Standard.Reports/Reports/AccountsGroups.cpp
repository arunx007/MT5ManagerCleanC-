//+------------------------------------------------------------------+
//|                           MetaTrader 5 Accounts.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "AccountsGroups.h"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
const MTReportInfo CAccountsGroups::s_info=
  {
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_ANY,
   L"Accounts Groups",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_NO_GROUPS_LOGINS,
   MTReportInfo::TYPE_HTML,
   L"Accounts",
   };
//--- User request fields descriptions
const DatasetField CAccountsGroups::s_user_fields[]=
   //--- id                                  , select, offset
  {{ IMTDatasetField::FIELD_USER_LOGIN       , true, offsetof(UserRecord,login) },
   { IMTDatasetField::FIELD_USER_GROUP      },
   { IMTDatasetField::FIELD_USER_BALANCE     , true, offsetof(UserRecord,balance) },
   { IMTDatasetField::FIELD_USER_CREDIT      , true, offsetof(UserRecord,credit) },
   { IMTDatasetField::FIELD_USER_LAST_ACCESS , true, offsetof(UserRecord,last_access) },
  };
//--- Account request fields descriptions
const DatasetField CAccountsGroups::s_account_fields[]=
   //--- id                                  , select, offset
  {{ IMTDatasetField::FIELD_ACCOUNT_LOGIN    , true, offsetof(AccountRecord,login) },
   { IMTDatasetField::FIELD_ACCOUNT_GROUP   },
   { IMTDatasetField::FIELD_ACCOUNT_BALANCE  , true, offsetof(AccountRecord,balance) },
   { IMTDatasetField::FIELD_ACCOUNT_EQUITY   , true, offsetof(AccountRecord,equity) },
   { IMTDatasetField::FIELD_ACCOUNT_PROFIT   , true, offsetof(AccountRecord,profit) },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CAccountsGroups::CAccountsGroups(void) :
   CReportBase(s_info),m_checktime(0),m_group(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CAccountsGroups::~CAccountsGroups(void)
  {
   CAccountsGroups::Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CAccountsGroups::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CAccountsGroups::Clear(void)
  {
   m_checktime=0;
//--- group config interface
   if(m_group)
     {
      m_group->Release();
      m_group=nullptr;
     }
//--- select objects
   m_user_select.Clear();
   m_account_select.Clear();
//--- groups and groups total
   m_groups.Clear();
   m_groups_totals.Clear();
  }
//+------------------------------------------------------------------+
//| Report preparation                                               |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGroups::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- activity check time
   m_checktime=m_api->TimeGeneration()-3*SECONDS_IN_MONTH;
//--- create interfaces
   if(!(m_group=m_api->GroupCreate()))
      return(MT_RET_ERR_MEM);
//--- prepare user request
   MTAPIRES res=m_user_select.Prepare(m_api,s_user_fields,_countof(s_user_fields),s_request_limit);
   if(res!=MT_RET_OK)
      return(res);
//--- prepare account request
   if((res=m_account_select.Prepare(m_api,s_account_fields,_countof(s_account_fields),s_request_limit))!=MT_RET_OK)
      return(res);
//--- load template
   if((res=m_api->HtmlTplLoadResource(IDR_ACCOUNTS_GROUPS,RT_HTML))!=MT_RET_OK)
      m_log.Error(res,L"HTML template load from resource");
   return(res);
  }
//+------------------------------------------------------------------+
//| Report writing                                                   |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGroups::Write(uint32_t)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- use template
   MTAPIRES    res=MT_RET_OK;
   MTAPISTR    tag;
   uint32_t    counter=0;
   uint32_t    current_index=0;
   uint32_t    types[]={ GROUP_REAL, GROUP_PRELIMINARY, GROUP_COVERAGE, GROUP_DEMO,GROUP_CONTEST, GROUP_MANAGER };
   CMTStr256   str,current_type;
   GroupRecord current={};
//--- HtmlTplNext return tag string and counter,that shows how many times tag has been used
   while(m_api->HtmlTplNext(tag,&counter)==MT_RET_OK)
     {
      //--- group types
      if(CMTStr::CompareNoCase(tag,L"group_types")==0)
        {
         //--- iterate group type
         if(counter<_countof(types))
           {
            //--- get type title
            if(!GetGroupTypeName(types[counter],current_type))
               continue;
            //--- prepare groups for current type
            if((res=PrepareGroupType(types[counter]))!=MT_RET_OK)
               break;
            //--- process tags
            if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
               break;
           }
         continue;
        }
      //--- group type title
      if(CMTStr::CompareNoCase(tag,L"type")==0)
        {
         if((res=m_api->HtmlWriteSafe(current_type.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            break;
         continue;
        }
      //--- groups
      if(CMTStr::CompareNoCase(tag,L"groups")==0)
        {
         //--- iterate groups in group type
         if(counter<m_groups.Total())
           {
            //--- get current record
            current=m_groups[counter];
            //--- save current record index
            current_index=counter;
            //--- process tags
            if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
               break;
           }
         continue;
        }
      //--- groups totals
      if(CMTStr::CompareNoCase(tag,L"groups_totals")==0)
        {
         //--- iterate group totals for group type
         if(counter<m_groups_totals.Total())
           {
            //--- get current record
            current=m_groups_totals[counter];
            //--- save current record index
            current_index=counter;
            //--- process tags
            if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
               break;
           }
         continue;
        }
      //--- line position
      if(CMTStr::CompareNoCase(tag,L"line")==0)
        {
         if((res=m_api->HtmlWrite(L"%u",current_index&1))!=MT_RET_OK)
            break;
         continue;
        }
      //--- group name
      if(CMTStr::CompareNoCase(tag,L"group")==0)
        {
         if((res=m_api->HtmlWriteSafe(current.group,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            break;
         continue;
        }
      //--- accounts
      if(CMTStr::CompareNoCase(tag,L"accounts")==0)
        {
         if((res=m_api->HtmlWrite(L"%u",current.accounts))!=MT_RET_OK)
            break;
         continue;
        }
      //--- active
      if(CMTStr::CompareNoCase(tag,L"active")==0)
        {
         if(current.accounts>0)
            str.Format(L"%.0lf",(double)current.active*100.0/(double)current.accounts);
         else
            str.Format(L"0");

         if((res=m_api->HtmlWriteString(str.Str()))!=MT_RET_OK)
            break;
         continue;
        }
      //--- balance
      if(CMTStr::CompareNoCase(tag,L"balance")==0)
        {
         if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,current.balance,current.currency_digits)))!=MT_RET_OK)
            break;
         continue;
        }
      //--- equity
      if(CMTStr::CompareNoCase(tag,L"equity")==0)
        {
         if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,current.equity,current.currency_digits)))!=MT_RET_OK)
            break;
         continue;
        }
      //--- profit style
      if(CMTStr::CompareNoCase(tag,L"profit_style")==0)
        {
         //--- profit < 0?
         if(current.profit<0)
            if((res=m_api->HtmlWrite(L"style=\"color:red;\""))!=MT_RET_OK)
               break;
         continue;
        }
      //--- profit
      if(CMTStr::CompareNoCase(tag,L"profit")==0)
        {
         if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,current.profit,current.currency_digits)))!=MT_RET_OK)
            break;
         continue;
        }
      //--- currency
      if(CMTStr::CompareNoCase(tag,L"currency")==0)
        {
         if((res=m_api->HtmlWriteSafe(current.currency,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            break;
         continue;
        }
      //--- report name
      if(CMTStr::CompareNoCase(tag,L"report_name")==0)
        {
         if((res=m_api->HtmlWriteSafe(s_info.name,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            break;
         continue;
        }
     }
   return(res);
  }
//+------------------------------------------------------------------+
//| Prepare information for group type                               |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGroups::PrepareGroupType(const uint32_t type)
  {
//--- clear groups and groups totals
   m_groups.Clear();
   m_groups_totals.Clear();
//--- checks
   if(!m_api || !m_group)
      return(MT_RET_ERR_PARAMS);
//--- iterate groups
   for(uint32_t pos=0;m_api->GroupNext(pos,m_group)==MT_RET_OK;pos++)
     {
      //--- check group type
      if(GetGroupType(m_group->Group())!=type)
         continue;
      //--- clear group record
      GroupRecord group={};
      //--- fill group information
      CMTStr::Copy(group.group   ,m_group->Group());
      CMTStr::Copy(group.currency,m_group->Currency());
      group.currency_digits=m_group->CurrencyDigits();
      //--- prepare information for group
      MTAPIRES res=PrepareGroup(group);
      if(res!=MT_RET_OK)
         return(res);
      //--- add group
      if(group.accounts)
         if((res=PrepareGroupAdd(group))!=MT_RET_OK)
            return(res);
     }
//--- sort groups & totals
   m_groups.Sort(SortGroupByName);
   m_groups_totals.Sort(SortGroupByAccounts);
//--- checks totals
   if(!m_groups_totals.Total())
     {
      //--- add empty group
      GroupRecord group={};
      if(!m_groups_totals.Add(&group))
         return(MT_RET_ERR_MEM);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare information for group                                    |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGroups::PrepareGroup(GroupRecord &group)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- select users by group
   MTAPIRES res=m_user_select.Select(group.group);
   if(res!=MT_RET_OK)
      return(res);
//--- select accounts by group
   if((res=m_account_select.Select(group.group))!=MT_RET_OK)
      return(res);
//--- select users part by part
   for(uint64_t login=0;IMTDataset *users=m_user_select.Next(res,login);login++)
     {
      //--- select accounts
      IMTDataset *accounts=m_account_select.Next(res,login);
      if(!accounts && res!=MT_RET_OK)
         return(res);
      //--- process datasets
      if((res=PrepareGroupPart(group,*users,accounts,login))!=MT_RET_OK)
         return(m_log.Error(res,L"processing"));
      //--- clear datasets rows
      if((res=users->RowClear())!=MT_RET_OK)
         return(res);
      if(accounts)
         if((res=accounts->RowClear())!=MT_RET_OK)
            return(res);
     }
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| Prepare information for group part                               |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGroups::PrepareGroupPart(GroupRecord &group,IMTDataset &users,IMTDataset *accounts,uint64_t &login_last)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- total users
   const uint32_t total=users.RowTotal();
   if(!total)
      return(MT_RET_OK);
   group.accounts+=total;
//--- iterate users
   UserRecord user={};
   AccountRecord account={};
   for(uint32_t i=0,j=0,total_account=accounts ? accounts->RowTotal() : 0;i<total;i++)
     {
      //--- read user
      MTAPIRES res=users.RowRead(i,&user,sizeof(user));
      if(res!=MT_RET_OK)
         return(res);
      //--- store last login
      if(login_last<user.login)
         login_last=user.login;
      //--- read account
      if(accounts && account.login<user.login && j<total_account)
        {
         if((res=accounts->RowRead(j,&account,sizeof(account)))!=MT_RET_OK)
            return(res);
         j++;
        }
      //--- account found
      if(account.login==user.login)
        {
         //--- calculate balance and equity using trade account
         group.balance=SMTMath::MoneyAdd(group.balance,account.balance,group.currency_digits);
         group.equity =SMTMath::MoneyAdd(group.equity, account.equity, group.currency_digits);
         group.profit =SMTMath::MoneyAdd(group.profit, account.profit, group.currency_digits);
        }
      else
        {
         //--- calculate balance and equity using user record
         group.balance=SMTMath::MoneyAdd(group.balance,user.balance,group.currency_digits);
         group.equity =SMTMath::MoneyAdd(group.equity, user.balance,group.currency_digits);
         group.equity =SMTMath::MoneyAdd(group.equity, user.credit ,group.currency_digits);
        }
      //--- collect 3 months active accounts
      if(m_checktime<user.last_access)
         group.active++;
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Add group records to groups, totals for groups and total         |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGroups::PrepareGroupAdd(const GroupRecord& group)
  {
//--- add to current type list
   if(!m_groups.Add(&group))
      return(MT_RET_ERR_MEM);
//--- search group type totals
   GroupRecord *ptr=m_groups_totals.Search(&group,SortGroupByCurrency);
   if(ptr)
     {
      //--- update totals
      ptr->currency_digits=std::min(ptr->currency_digits,group.currency_digits);
      //---
      ptr->accounts+=group.accounts;
      ptr->active  +=group.active;
      ptr->balance  =SMTMath::MoneyAdd(ptr->balance,group.balance,ptr->currency_digits);
      ptr->equity   =SMTMath::MoneyAdd(ptr->equity, group.equity, ptr->currency_digits);
      ptr->profit   =SMTMath::MoneyAdd(ptr->profit, group.profit, ptr->currency_digits);
     }
   else
     {
      //--- insert new total
      if(!m_groups_totals.Insert(&group,SortGroupByCurrency))
         return(MT_RET_ERR_MEM);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Sort group by currency                                           |
//+------------------------------------------------------------------+
int32_t CAccountsGroups::SortGroupByCurrency(const void *left,const void *right)
  {
   GroupRecord *lft=(GroupRecord*)left;
   GroupRecord *rgh=(GroupRecord*)right;
//--- sort by currency
   return(CMTStr::CompareNoCase(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+
//| Sort group by name                                           |
//+------------------------------------------------------------------+
int32_t CAccountsGroups::SortGroupByName(const void *left,const void *right)
  {
   GroupRecord *lft=(GroupRecord*)left;
   GroupRecord *rgh=(GroupRecord*)right;
//--- by name
   return(CMTStr::Compare(lft->group,rgh->group));
  }
//+------------------------------------------------------------------+
//| Sort group by accounts                                           |
//+------------------------------------------------------------------+
int32_t CAccountsGroups::SortGroupByAccounts(const void *left,const void *right)
  {
   GroupRecord *lft=(GroupRecord*)left;
   GroupRecord *rgh=(GroupRecord*)right;
//--- by accounts
   if(lft->accounts>rgh->accounts)
      return(-1);
   if(lft->accounts<rgh->accounts)
      return(1);
//--- by equity
   if(lft->equity>rgh->equity)
      return(-1);
   if(lft->equity<rgh->equity)
      return(1);
//--- by name
   return(CMTStr::Compare(lft->group,rgh->group));
  }
//+------------------------------------------------------------------+
