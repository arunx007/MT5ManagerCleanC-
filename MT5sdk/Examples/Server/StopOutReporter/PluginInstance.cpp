//+------------------------------------------------------------------+
//|                                          StopOut Reporter Plugin |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CPluginInstance::CPluginInstance(HMODULE hmodule) : m_module(hmodule),m_api(NULL),m_config(NULL),
                                         m_terminate(FALSE),
                                         m_check_period(0),m_notify_pause(0),
                                         m_group(NULL),m_account(NULL),m_user(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CPluginInstance::~CPluginInstance(void)
  {
   Stop();
  }
//+------------------------------------------------------------------+
//| Plugin release function                                          |
//+------------------------------------------------------------------+
void CPluginInstance::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Plugin start notification function                               |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Start(IMTServerAPI* api)
  {
   MTAPIRES res    =MT_RET_OK;
//--- 
   Stop();
//--- check parameters
   if(!api)
      return(MT_RET_ERR_PARAMS);
   m_api=api;
//--- create plugin config
   if((m_config=m_api->PluginCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- group
   if((m_group=m_api->GroupCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- user
   if((m_account=m_api->UserCreateAccount())==NULL)
      return(MT_RET_ERR_MEM);
//--- user
   if((m_user=m_api->UserCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- subscribe on configuration changes
   if((res=m_api->PluginSubscribe(this))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogAtt,L"plugin subscribe failed [%d]",res);
      return(res);
     }
//--- read parameters
   if((res=ParametersRead())!=MT_RET_OK)
      return(res);
//--- read database
   BaseRead();
//--- read templates
   if(!TemplatesRead(L"mc_notify_template.htm",IDR_MC_NOTIFY,m_templ_mc) || !TemplatesRead(L"so_notify_template.htm",IDR_SO_NOTIFY,m_templ_so))
      return(MT_RET_ERR_MEM);
//--- start thread
   InterlockedExchange(&m_terminate,FALSE);
   if(!m_check_thread.Start(CheckThreadWrapper,this,STACK_SIZE_COMMON))
     {
      m_api->LoggerOutString(MTLogAtt,L"check thread start failed");
      return(MT_RET_ERROR);
     }
//--- всё хорошо
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin stop notification function                                |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Stop(void)
  {
//--- unsubscribe
   if(m_api)
      m_api->PluginUnsubscribe(this);
//--- stop flag
   InterlockedExchange(&m_terminate,TRUE);
//--- close send process
   m_send_process.Close();
//--- wait threads
   if(!m_check_thread.Shutdown())
      m_api->LoggerOutString(MTLogErr,L"check thread is not stopped");
//--- wait threads
   if(!m_send_thread.Shutdown())
      m_api->LoggerOutString(MTLogErr,L"send thread is not stopped");
//--- save base
   BaseSave();
//--- clean base
   m_stopouts.Shutdown();
//--- clean variables
   if(m_group)
     {
      m_group->Release();
      m_group=NULL;
     }
   if(m_account)
     {
      m_account->Release();
      m_account=NULL;
     }
   if(m_user)
     {
      m_user->Release();
      m_user=NULL;
     }
//--- return
   m_api=NULL;
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Parameters                                                       |
//+------------------------------------------------------------------+
void CPluginInstance::OnPluginUpdate(const IMTConPlugin* plugin)
  {
//--- stop flag
   InterlockedExchange(&m_terminate,TRUE);
//--- close send process
   m_send_process.Close();
//--- wait threads
   if(!m_check_thread.Shutdown())
      m_api->LoggerOutString(MTLogErr,L"check thread is not stopped");
//--- wait threads
   if(!m_send_thread.Shutdown())
      m_api->LoggerOutString(MTLogErr,L"send thread is not stopped");
//--- read parameters
   ParametersRead();
//--- start check thread
   InterlockedExchange(&m_terminate,FALSE);
   if(!m_check_thread.Start(CheckThreadWrapper,this,STACK_SIZE_COMMON))
      m_api->LoggerOutString(MTLogAtt,L"check thread start failed");
  }
//+------------------------------------------------------------------+
//| Parameters                                                       |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::ParametersRead(void)
  {
   MTAPIRES     res    =MT_RET_OK;
   IMTConParam* param  =NULL;
   CMTStr128    tmp;
//--- check pointers
   if(!m_api || !m_config)
      return(MT_RET_ERR_PARAMS);
//--- get current plugin configuration
   if((res=m_api->PluginCurrent(m_config))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"failed to get current plugin configuration [%s (%u)]",
                       SMTFormat::FormatError(res),res);
      return(res);
     }
//--- create plugin parameter object   
   if((param=m_api->PluginParamCreate())==NULL)
     {
      m_api->LoggerOutString(MTLogErr,L"failed to create plugin parameter object");
      return(MT_RET_ERR_MEM);
     }
//--- get parameters
   if((res=m_config->ParameterGet(L"Period (min)",param))!=MT_RET_OK || param->Type()!=IMTConParam::TYPE_INT || param->ValueInt()<=0)
     {
      m_api->LoggerOutString(MTLogErr,L"period is missing");
      param->Release();
      return(MT_RET_ERR_PARAMS);
     }
   m_check_period=param->ValueInt();
//--- groups
   if((res=m_config->ParameterGet(L"Groups",param))!=MT_RET_OK || param->Type()!=IMTConParam::TYPE_GROUPS)
     {
      m_api->LoggerOutString(MTLogErr,L"groups is missing");
      param->Release();
      return(MT_RET_ERR_PARAMS);
     }
   m_group_mask.Assign(param->ValueGroups());
//--- pause
   if((res=m_config->ParameterGet(L"Pause (hours)",param))!=MT_RET_OK || param->Type()!=IMTConParam::TYPE_INT || param->ValueInt()<=0)
     {
      m_api->LoggerOutString(MTLogErr,L"pause is missing");
      param->Release();
      return(MT_RET_ERR_PARAMS);
     }
//--- convert from hours to seconds
   m_notify_pause=param->ValueInt()*3600;
//--- from
   if((res=m_config->ParameterGet(L"From",param))!=MT_RET_OK)
     {
      m_api->LoggerOutString(MTLogErr,L"from is missing");
      param->Release();
      return(MT_RET_ERR_PARAMS);
     }
   m_notify_from.Assign(param->ValueString());
//--- copy to
   if((res=m_config->ParameterGet(L"Copy To",param))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"copy to is missing [%u]",res);
      param->Release();
      return(MT_RET_ERR_PARAMS);
     }
   m_notify_copy.Assign(param->ValueString());
//--- SMTP Server
   if((res=m_config->ParameterGet(L"SMTP Server",param))!=MT_RET_OK || !param->ValueString() || param->ValueString()[0]==0)
     {
      m_api->LoggerOut(MTLogErr,L"smtp server to is missing [%u]",res);
      param->Release();
      return(MT_RET_ERR_PARAMS);
     }
   m_notify_server.Assign(param->ValueString());
//--- SMTP Login
   if((res=m_config->ParameterGet(L"SMTP Login",param))!=MT_RET_OK || !param->ValueString() || param->ValueString()[0]==0)
     {
      m_api->LoggerOut(MTLogErr,L"smtp login to is missing [%u]",res);
      param->Release();
      return(MT_RET_ERR_PARAMS);
     }
   m_notify_login.Assign(param->ValueString());
//--- SMTP Password
   if((res=m_config->ParameterGet(L"SMTP Password",param))!=MT_RET_OK || !param->ValueString() || param->ValueString()[0]==0)
     {
      m_api->LoggerOut(MTLogErr,L"smtp password to is missing [%u]",res);
      param->Release();
      return(MT_RET_ERR_PARAMS);
     }
   m_notify_password.Assign(param->ValueString());
//--- free objects
   param->Release();
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Load template from file or from resource                         |
//+------------------------------------------------------------------+
bool CPluginInstance::TemplatesRead(LPCWSTR filename,const uint32_t id_res,CMTMemPack& pack)
  {
   CMTStrPath path;
   HRSRC      hsrc;
   DWORD      size;
   HGLOBAL    handle;
   LPVOID     data=NULL;
   CMTFile    file;
   bool       res=true;
//--- check
   if(!filename)
      return(false);
//--- path
   path.Format(L"%s\\%s",m_path_data.Str(),filename);
//--- check template in file
   if(file.OpenRead(path.Str()))
     {
      size=DWORD(file.Size());
      if(size && pack.Reallocate(size) && file.Read(pack.Buffer(),size)==size)
        {
         pack.Len(size);
         file.Close();
         return(true);
        }
     }
   if(!(hsrc=FindResourceW(m_module,MAKEINTRESOURCE(id_res),RT_HTML)))
      return(false);
//--- size
   if((size=SizeofResource(m_module,hsrc))==0)
      return(false);
//--- load from resource
   if(!(handle=LoadResource(m_module,hsrc)))
      return(false);
//--- load
   data=LockResource(handle);
//--- add to mempack
   if(data)
      res=pack.Add(data,size);
//--- free allocated resource
   FreeResource(handle);
//--- save template
   if(file.OpenWrite(path.Str()))
     {
      file.Write(pack.Buffer(),pack.Len());
      file.Close();
     }
//--- 
   return(res);
  }
//+------------------------------------------------------------------+
//| SO check thread                                                  |
//+------------------------------------------------------------------+
unsigned __stdcall CPluginInstance::CheckThreadWrapper(void *param)
  {
   if(param)
      ((CPluginInstance*)param)->CheckThread();
   return(0);
  }
//+------------------------------------------------------------------+
//| SO check thread                                                  |
//+------------------------------------------------------------------+
__declspec(noinline) void CPluginInstance::CheckThread(void)
  {
   int64_t check_time=0,ctm;
//--- check
   if(!m_api)
      return;
//--- loop
   while(!InterlockedExchangeAdd(&m_terminate,0))
     {
      ctm=m_api->TimeCurrent();
      if(m_check_period && ctm>check_time)
        {
         CheckStopout();
         //--- next time
         check_time=ctm+m_check_period*SECONDS_IN_MINUTE;
        }
      //--- wait
      Sleep(250);
     }
  }
//+------------------------------------------------------------------+
//| SO check                                                         |
//+------------------------------------------------------------------+
void CPluginInstance::CheckStopout(void)
  {
   uint64_t *logins      =NULL;
   uint32_t    logins_total=0;
//--- check groups
   for(uint32_t i=0;m_api->GroupNext(i,m_group)==MT_RET_OK;i++)
     {
      //--- check group by mask
      if(!CMTStr::CheckGroupMask(m_group_mask.Str(),m_group->Group()))
         continue;
      //--- get users
      if(m_api->UserLogins(m_group->Group(),logins,logins_total)!=MT_RET_OK || logins==NULL)
         continue;
      //--- check one by one
      for(uint32_t user=0;user<logins_total;user++)
         CheckStopoutLogin(m_group,logins[user]);
      if(logins)
        {
         m_api->Free(logins);
         logins=NULL;
        }
     }
//--- send check results
   StopOutSend();
  }
//+------------------------------------------------------------------+
//| User SO check                                                    |
//+------------------------------------------------------------------+
void CPluginInstance::CheckStopoutLogin(const IMTConGroup* group,const uint64_t login)
  {
   StopOutInfo info,*ptr;
//--- check
   if(!group || login<1)
      return;
//--- create record
   ZeroMemory(&info,sizeof(info));
   info.login=login;
   if(m_api->UserAccountGet(login,m_account)!=MT_RET_OK)
      return;
//--- check
   if(m_account->SOActivation()!=IMTAccount::ACTIVATION_NONE && m_account->Margin()!=0.0)
     {
      //--- exist?
      if((ptr=m_stopouts.Search(&login,SearchByLogin))==NULL)
        {
         //--- add new
         if((ptr=m_stopouts.Insert(&info,SortByLogin))==NULL)
            return;
         ptr->datetime=m_api->TimeCurrent();
         ptr->login   =login;
        }
      //---
      ptr->margin  =m_account->Margin();
      ptr->equity  =m_account->Equity();
      ptr->level   =m_account->MarginLevel();
      ptr->value   =(group->MarginSOMode()==IMTConGroup::STOPOUT_PERCENT) ? m_account->MarginLevel() : m_account->Equity();
      //--- Stop Out or Margin Call?
      if(m_account->SOActivation()==IMTAccount::ACTIVATION_STOP_OUT)
        {
         //--- Stop Out
         ptr->mc_flag=false;
         ptr->so_flag=true;
         ptr->limit  =group->MarginStopOut();
        }
      else
        {
         //--- Margin Call
         ptr->mc_flag=true;
         ptr->so_flag=false;
         ptr->limit  =group->MarginCall();
        }
     }
  }
//+------------------------------------------------------------------+
//| Reports send                                                     |
//+------------------------------------------------------------------+
void CPluginInstance::StopOutSend(void)
  {
   StopOutInfo  *info;
   CMTStr2048    path;
   uint32_t      total=0;
   int64_t         ctm;
//--- check
   if(!m_api) return;
//--- check send thread
   if(m_send_thread.IsBusy())
      return;
//--- time
   ctm=m_api->TimeCurrent();
//--- loop
   for(uint32_t i=0;i<m_stopouts.Total();i++)
     {
      info=&m_stopouts[i];
      //--- check notify period
      if((info->mc_send_flag && (ctm-info->datetime)>m_notify_pause) ||
         (info->so_send_flag && (ctm-info->datetime)>m_notify_pause))
        {
         //--- too old, delete it
         m_stopouts.Delete(i);
         i--;
         continue;
        }
      //--- Margin Call notify?
      if(info->mc_flag && !info->mc_send_flag)
        {
         info->mc_send_flag=StopOutGenerate(m_templ_mc,*info);
         total++;
         continue;
        }
      //---  Stop out notify?
      if(info->so_flag && !info->so_send_flag)
        {
         if((info->so_send_flag=StopOutGenerate(m_templ_so,*info))!=false)
            info->mc_send_flag=true;
         total++;
         continue;
        }
     }
//--- any new?
   if(total)
     {
      //--- start send
      if(!m_send_thread.Start(SendThreadWrapper,this,STACK_SIZE_COMMON))
         m_api->LoggerOut(MTLogErr,L"start send thread failed [%u]",GetLastError());
      else
         m_api->LoggerOut(MTLogOK,L"stop-out notification thread started [%u mail(s)]",total);
     }
  }
//+------------------------------------------------------------------+
//| Report Generator                                                 |
//+------------------------------------------------------------------+
bool CPluginInstance::StopOutGenerate(CMTMemPack& templ,StopOutInfo& info)
  {
   CMTStr256      macros,symbol;
   CMTStrPath     path;
   CMTStr1024     tmp;
   double         shortage=0.0;
   uint32_t       digits,report_len=0;
   uint16_t         unicode=0xFEFF;
//--- check
   if(!templ.Len())
      return(false);
//--- create email config file
   if(!m_send_config.IsOpen())
     {
      path.Format(L"%s\\mail.cfg",m_path_data.Str());
      if(!m_send_config.OpenWrite(path.Str()))
        {
         m_api->LoggerOut(MTLogErr,L"StopOut Reporter: config file open error [%s][%u]",path.Str(),GetLastError());
         return(false);
        }
      //--- unicode
      uint16_t unisign=0xFEFF;
      m_send_config.Write(&unisign,sizeof(unisign));
      //--- header
      tmp.Format(L"<group>\r\n"
                 L"name=stopouts\r\n"
                 L"company=stopouts\r\n"
                 L"email=%s\r\n"
                 L"subject=Margin Call Notification\r\n"
                 L"smtp_srv=%s\r\n"
                 L"smtp_login=%s\r\n"
                 L"smtp_pass=%s\r\n\r\n",
                 m_notify_from.Str(),
                 m_notify_server.Str(),
                 m_notify_login.Str(),
                 m_notify_password.Str());
      m_send_config.Write(tmp.Str(),tmp.Len()*sizeof(wchar_t));
     }
//---
   if(m_api->UserGet(info.login,m_user)!=MT_RET_OK)
      return(false);
   if(m_api->GroupGet(m_user->Group(),m_group)!=MT_RET_OK)
      return(false);
//--- digits
   digits=m_group->CurrencyDigits();
//--- check memory
   m_report.Clear();
   if(m_report.Max()<(templ.Len()+64*1024))
      if(!m_report.Reallocate(templ.Len()+64*1024))
        {
         m_api->LoggerOutString(MTLogAtt,L"check thread start failed");
         return(false);
        }
//--- wrap buffer to string
   CMTStr  report((wchar_t*)m_report.Buffer(),m_report.Max());
//--- copy template
   if(templ.Len()>=sizeof(unicode) && *((uint16_t*)templ.Buffer())==unicode)
      report.Assign((wchar_t*)(templ.Buffer()+sizeof(unicode)),(templ.Len()-sizeof(unicode))/sizeof(wchar_t));
   else
      report.Assign((wchar_t*)templ.Buffer(),templ.Len()/sizeof(wchar_t));
//--- replace macro
   report.Replace(L"<!--DATE-->",SMTFormat::FormatDateTime(tmp,info.datetime,false,false));
   report.Replace(L"<!--TIME-->",SMTFormat::FormatTime(tmp,info.datetime,false));
//---
   tmp.Format(L"%I64u",m_user->Login());
   report.Replace(L"<!--LOGIN-->",tmp.Str());
//---
   report.Replace(L"<!--NAME-->",m_user->Name());
   report.Replace(L"<!--GROUP-->",m_user->Group());
   report.Replace(L"<!--COMPANY-->",m_group->Company());
   report.Replace(L"<!--ID-->",m_user->ID());
   report.Replace(L"<!--COUNTRY-->",m_user->Country());
   report.Replace(L"<!--CITY-->",m_user->City());
   report.Replace(L"<!--STATE-->",m_user->State());
   report.Replace(L"<!--ZIPCODE-->",m_user->ZIPCode());
   report.Replace(L"<!--ADDRESS-->",m_user->Address());
   report.Replace(L"<!--PHONE-->",m_user->Phone());
   report.Replace(L"<!--EMAIL-->",m_user->EMail());
   report.Replace(L"<!--STATUS-->",m_user->Status());
   report.Replace(L"<!--CURRENCY-->",m_group->Currency());
//---
   tmp.Format(L"1:%I64u",m_user->Leverage());
   report.Replace(L"<!--LEVERAGE-->",tmp.Str());
//---
   report.Replace(L"<!--BALANCE-->",SMTFormat::FormatMoney(tmp,m_user->Balance(),digits));
   report.Replace(L"<!--CREDIT-->",SMTFormat::FormatMoney(tmp,m_user->Credit(),digits));
   report.Replace(L"<!--EQUITY-->",SMTFormat::FormatMoney(tmp,info.equity,digits));
   report.Replace(L"<!--MARGIN-->",SMTFormat::FormatMoney(tmp,info.margin,digits));
//---
   tmp.Format(L"%.2lf%%",info.level);
   report.Replace(L"<!--MARGIN_LEVEL-->",tmp.Str());
//---
   tmp.Format(L"%s%s",SMTFormat::FormatMoney(tmp,info.limit,digits),(m_group->MarginSOMode()==IMTConGroup::STOPOUT_PERCENT)?"%":"$");
   report.Replace(L"<!--MARGIN_LIMIT-->",tmp.Str());
//---
   tmp.Format(L"%s%s",SMTFormat::FormatMoney(tmp,info.value,digits),(m_group->MarginSOMode()==IMTConGroup::STOPOUT_PERCENT)?"%":"$");
   report.Replace(L"<!--MARGIN_VALUE-->",tmp.Str());
//--- calculate shortage
   if(m_group->MarginSOMode()==IMTConGroup::STOPOUT_MONEY)
      shortage=m_group->MarginCall()-info.equity;
   else
      shortage=m_group->MarginCall()*info.margin/100.0-info.equity;
   report.Replace(L"<!--MARGIN_SHORT-->",SMTFormat::FormatMoney(tmp,shortage,digits));
//--- path
   path.Format(L"%s\\%I64u_mail.htm",m_path_data.Str(),m_user->Login());
//--- save report
   CMTFile report_file;
//--- write
   if(report_file.OpenWrite(path.Str()) &&
      report_file.Write(&unicode,sizeof(unicode))==sizeof(unicode) &&
      report_file.Write(report.Buffer(),report.Len()*sizeof(wchar_t))==(report.Len()*sizeof(wchar_t)))
     {
      //--- specify email
      tmp.Format(L"%I64u\t0\t%s\t%s\r\n",m_user->Login(),m_user->Name(),m_user->EMail());
      m_send_config.Write(tmp.Str(),tmp.Len()*sizeof(wchar_t));
      //--- add copy address
      if(m_notify_copy.Str()[0])
        {
         tmp.Format(L"%I64u\t0\t%s\t%s\r\n",m_user->Login(),m_user->Name(),m_notify_copy.Str());
         m_send_config.Write(tmp.Str(),tmp.Len()*sizeof(wchar_t));
        }
     }
//--- clean
   report_file.Close();
   m_report.Clear();
//--- всё ок
   return(true);
  }
//+------------------------------------------------------------------+
//| Notification send thread                                         |
//+------------------------------------------------------------------+
uint32_t __stdcall CPluginInstance::SendThreadWrapper(LPVOID param)
  {
   if(param)
      ((CPluginInstance*)param)->SendThread();
   return(0);
  }
//+------------------------------------------------------------------+
//| Notification send thread                                         |
//+------------------------------------------------------------------+
__declspec(noinline) void CPluginInstance::SendThread(void)
  {
   CMTStr1024 path;
   uint32_t   i;
//--- open file
   path.Assign("</group>");
   m_send_config.Write(path.Str(),path.Len()*sizeof(wchar_t));
   m_send_config.Close();
//--- start send process
   path.Format(L"%s\\mt5sendmail64.exe /group:\"%s\"",m_path_root.Str(),m_path_data.Str());
   if(m_send_process.Start(path))
     {
      //--- wait results
      for(i=0;i<SEND_PAUSE_TOTAL && !InterlockedExchangeAdd(&m_terminate,0);i++)
         if(m_send_process.Wait(1000)) break;
      //--- timeout?
      if(i>=SEND_PAUSE_TOTAL)
        {
         m_api->LoggerOutString(MTLogErr,L"StopOut Reporter: mail process timeout reached");
         return;
        }
      //--- log
      m_api->LoggerOutString(MTLogOK,L"StopOut Reporter: stop-out notification finished");
     }
   else
      m_api->LoggerOut(MTLogErr,L"StopOut Reporter: mail process creation error [%u]",GetLastError());

//--- mails
   CMTStr16 mask(L"*_mail.htm");
   CMTFile::DirectoryClean(m_path_data,mask);
//--- config
   path.Format(L"%s\\mail.cfg",m_path_data.Str());
   DeleteFileW(path.Str());
  }
//+------------------------------------------------------------------+
//| SO base read                                                     |
//+------------------------------------------------------------------+
void CPluginInstance::BaseRead(void)
  {
   CMTStrPath    path;
   CMTFile       file;
   int32_t           version;
   StopOutInfo   info;
   uint32_t      i,total;
//--- clear
   m_stopouts.Clear();
//--- check path
   if(!m_path_data.Str()[0])
     {
      m_path_root.Clear();
      GetModuleFileNameW(NULL,m_path_root.Buffer(),m_path_root.Max()); m_path_root.Refresh();
      if((i=m_path_root.FindRChar('\\'))>=0) m_path_root.Trim(i);
      //--- create directory
      m_path_data.Format(L"%s\\plugins\\%s",m_path_root.Str(),m_config->Name());
      CMTFile::DirectoryCreate(m_path_data);
     }
//--- path
   path.Format(L"%s\\margin_calls.dat",m_path_data.Str());
//--- read from file
   if(file.Open(path.Str(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,OPEN_EXISTING))
     {
      //--- version
      if(file.Read(&version,sizeof(version))!=sizeof(version) || version!=BASE_VERSION)
        {
         file.Close();
         DeleteFileW(path.Str());
         return;
        }
      //--- total
      if(file.Read(&total,sizeof(total))!=sizeof(total) || total>BASE_MAX)
        {
         file.Close();
         DeleteFileW(path.Str());
         return;
        }
      //--- read
      m_stopouts.Reserve(total);
      for(i=0;i<total;i++)
         if(file.Read(&info,sizeof(info))==sizeof(info))
            m_stopouts.Add(&info);
     }
  }
//+------------------------------------------------------------------+
//| SO base save                                                     |
//+------------------------------------------------------------------+
void CPluginInstance::BaseSave(void)
  {
   CMTStrPath  path;
   CMTFile     file;
   int32_t         version=BASE_VERSION;
   uint32_t    i,total;
//--- check
   if(m_path_data.Str()[0]==0)
      return;
//--- path
   path.Format(L"%s\\margin_calls.dat",m_path_data.Str());
//--- open file
   if(file.OpenWrite(path.Str()))
     {
      //--- version
      if(file.Write(&version,sizeof(version))!=sizeof(version))
        {
         file.Close();
         DeleteFileW(path.Str());
         return;
        }
      //--- data total
      total=m_stopouts.Total();
      if(file.Write(&total,sizeof(total))!=sizeof(total))
        {
         file.Close();
         DeleteFileW(path.Str());
         return;
        }
      //--- data
      for(i=0;i<total;i++)
         file.Write(&m_stopouts[i],sizeof(StopOutInfo));
     }
//--- data saved
   m_path_data.Clear();
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int32_t CPluginInstance::SortByLogin(const void *left,const void *right)
  {
   StopOutInfo *lft=(StopOutInfo*)left;
   StopOutInfo *rgh=(StopOutInfo*)right;
//---
   if(lft->login>rgh->login) return(1);
   if(lft->login<rgh->login) return(-1);
//--- равны
   return(0);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int32_t CPluginInstance::SearchByLogin(const void *left,const void *right)
  {
   uint64_t                 lft=*(uint64_t*)left;
   StopOutInfo*           rgh=(StopOutInfo*)right;
//---
   if(lft>(rgh->login)) return(1);
   if(lft<(rgh->login)) return(-1);
//---
   return(0);
  }
//+------------------------------------------------------------------+