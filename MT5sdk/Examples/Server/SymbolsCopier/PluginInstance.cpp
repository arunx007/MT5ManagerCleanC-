//+------------------------------------------------------------------+
//|                                  MetaTrader 5 API Server Example |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CPluginInstance::CPluginInstance(void) : m_api(NULL), m_config(NULL),
                                         m_from_subgroups(false), m_to_subgroups(false)
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
//| Plugin release                                                   |
//+------------------------------------------------------------------+
void CPluginInstance::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Plugin start                                                     |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Start(IMTServerAPI* api)
  {
//--- check pointer
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- save pointer to Server API interface
   m_api=api;
//--- create plugin config
   m_config=m_api->PluginCreate();
   if(!m_config)
      return(MT_RET_ERR_MEM);
//--- subscribe to plugin config updates
   MTAPIRES res=m_api->PluginSubscribe(this);
   if(res!=MT_RET_OK)
      return(res);
//--- read plugin parameters
   res=ParametersRead();
   if(res!=MT_RET_OK)
      return(res);
//--- subscribe to symbol events
   res=m_api->SymbolSubscribe(this);
//--- primary synchronization
   OnSymbolSync();
//--- ok
   return(res);
  }
//+------------------------------------------------------------------+
//| Stop plugin                                                      |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Stop(void)
  {
//--- clean config object
   if(m_config)
     {
      m_config->Release();
      m_config=NULL;
     }
//--- check pointer
   if(!m_api)
      return(MT_RET_OK);
//--- unsubscribe from symbol events
   MTAPIRES res=m_api->SymbolUnsubscribe(this);

   if((res!=MT_RET_OK) && (res!=MT_RET_ERR_NOTFOUND))
      m_api->LoggerOut(MTLogErr,L"failed to unsubscribe from symbol events [%s (%u)]", SMTFormat::FormatError(res),res);
//--- unsubscribe from plugin config updates & clean API
   res=m_api->PluginUnsubscribe(this);

   if((res!=MT_RET_OK) && (res!=MT_RET_ERR_NOTFOUND))
      m_api->LoggerOut(MTLogErr, L"failed to unsubscribe from plugin config updates [%s (%u)]",SMTFormat::FormatError(res),res);
//--- clear Server API pointer
   m_api=NULL;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin config update notification                                |
//+------------------------------------------------------------------+
void CPluginInstance::OnPluginUpdate(const IMTConPlugin* plugin)
  {
//--- check parameters
   if(!plugin || !m_api || !m_config)
      return;
//--- check config
   if(CMTStr::Compare(plugin->Name(),m_config->Name())!=0 || plugin->Server()!=m_config->Server())
      return;
//--- update config
   MTAPIRES res=ParametersRead();
   if(res!=MT_RET_OK)
      return;
//--- synchronizing symbols
   OnSymbolSync();
  }
//+------------------------------------------------------------------+
//| A handler of the event of symbols synchronization                |
//+------------------------------------------------------------------+
void CPluginInstance::OnSymbolSync(void)
  {
//--- check pointer
   if(!m_api)
      return;
//--- create symbol configuration
   IMTConSymbol *src_symbol=m_api->SymbolCreate();
   if(!src_symbol)
     {
      m_api->LoggerOutString(MTLogErr,L"failed to create symbol");
      return;
     }
//--- synchronize each symbol
   for(uint32_t i=0;m_api->SymbolNext(i,src_symbol)==MT_RET_OK;i++)
      OnSymbolUpdate(src_symbol);
//--- delete symbol configuration
   src_symbol->Release();
  }
//+------------------------------------------------------------------+
//| A handler of the event of adding symbol settings                 |
//+------------------------------------------------------------------+
void CPluginInstance::OnSymbolAdd(const IMTConSymbol* src_symbol)
  {
   OnSymbolUpdate(src_symbol);
  }
//+------------------------------------------------------------------+
//| A handler of the event of updating symbol settings               |
//+------------------------------------------------------------------+
void CPluginInstance::OnSymbolUpdate(const IMTConSymbol* src_symbol)
  {
//--- check parameters and pointers
   if(!src_symbol || !m_api)
      return;
//--- obtaining the symbol path
   CMTStr1024 src_path;
   if(!PathGet(src_symbol,src_path))
     {
      m_api->LoggerOut(MTLogErr,L"failed to get symbol '%s' path",src_symbol->Symbol());
      return;
     }
//--- obtaining the list of nested groups
   CMTStr1024 src_subgroups;
   if(!SubgroupsGet(src_path,m_from_path,src_subgroups))
      return;
//--- completing the work if the symbol is not in the root group
   if(!m_from_subgroups && src_subgroups.Len())
      return;
//--- compiling the path of the symbols receiver group (considering the nested groups hierarchy saving settings)
   CMTStr1024 dst_path;
   if(!DestinationGet(m_to_path,(m_to_subgroups) ? src_subgroups : L"",dst_path))
     {
      m_api->LoggerOut(MTLogErr,L"failed to get symbol '%s' path",src_symbol->Symbol());
      return;
     }
//--- defining a name of the copied symbol
   CMTStr128 dst_name=src_symbol->Symbol();
   dst_name.Append(m_extension.Str());
//--- create symbol configuration
   IMTConSymbol *dst_symbol=m_api->SymbolCreate();
   if(!dst_symbol)
     {
      m_api->LoggerOutString(MTLogErr,L"failed to create symbol");
      return;
     }
//--- get symbol configuration from server
   MTAPIRES res=m_api->SymbolGet(dst_name.Str(),dst_symbol);
   if(res==MT_RET_OK)
     {
      //--- checking the identity of the detected and copied symbols paths
      CMTStr1024 path;
      if(!PathGet(dst_symbol,path))
        {
         m_api->LoggerOut(MTLogErr,L"failed to get symbol '%s' path",dst_name.Str());
         dst_symbol->Release();
         return;
        }
      //--- completing the work if the detected symbol is in another group
      if(path.Compare(dst_path))
        {
         dst_symbol->Release();
         return;
        }
      //--- copy symbol settings
      res=dst_symbol->Assign(src_symbol);
      if(res!=MT_RET_OK)
        {
         m_api->LoggerOut(MTLogErr,L"failed to copy symbol '%s' settings [%s (%u)]",
                          dst_name.Str(),SMTFormat::FormatError(res),res);
         dst_symbol->Release();
         return;
        }
     }
   else if(res==MT_RET_ERR_NOTFOUND)
        {
         //--- if there is no such symbol just append
         //--- copy symbol settings
         res=dst_symbol->Assign(src_symbol);
         if(res!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"failed to copy symbol '%s' settings [%s (%u)]",
                             dst_name.Str(),SMTFormat::FormatError(res),res);
            dst_symbol->Release();
            return;
           }
         //--- defining a path of the symbol
         res=dst_symbol->Path(dst_path.Str());
         if(res!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"failed to define symbol '%s' path [%s (%u)]",
                             dst_name.Str(),SMTFormat::FormatError(res),res);
            dst_symbol->Release();
            return;
           }
         //--- defining a name of the symbol
         res=dst_symbol->Symbol(dst_name.Str());
         if(res!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"failed to define symbol '%s' name [%s (%u)]",
                             dst_name.Str(),SMTFormat::FormatError(res),res);
            dst_symbol->Release();
            return;
           }
        }
      else
        {
         //--- sending the error message to the log file and completing the work
         m_api->LoggerOut(MTLogErr,L"failed to get symbol '%s' configuration [%s (%u)]",
                          dst_name.Str(),SMTFormat::FormatError(res),res);
         dst_symbol->Release();
         return;
        }
//--- update configuration
   res=m_api->SymbolAdd(dst_symbol);
   if(res!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"failed to update symbol '%s' configuration [%s (%u)]",
                       dst_symbol->Symbol(),SMTFormat::FormatError(res),res);
      dst_symbol->Release();
      return;
     }
//--- delete symbol configuration
   dst_symbol->Release();
//--- ok
  }
//+------------------------------------------------------------------+
//| A handler of the event of removing symbol settings               |
//+------------------------------------------------------------------+
void CPluginInstance::OnSymbolDelete(const IMTConSymbol* src_symbol)
  {
//--- check parameters and pointers
   if(!src_symbol || !m_api)
      return;
//--- obtaining the symbol path
   CMTStr1024 src_path;
   if(!PathGet(src_symbol,src_path))
     {
      m_api->LoggerOut(MTLogErr,L"failed to get symbol '%s' path",src_symbol->Symbol());
      return;
     }
//--- obtaining the path with the list of nested groups
   CMTStr1024 src_subgroups;
   if(!SubgroupsGet(src_path,m_from_path,src_subgroups))
      return;
//--- completing the work if the symbol is not in the root group
   if(!m_from_subgroups && src_subgroups.Len())
      return;
//--- defining a name of the deleted symbol
   CMTStr128 dst_name=src_symbol->Symbol();
   dst_name.Append(m_extension.Str());
//--- create symbol configuration
   IMTConSymbol *dst_symbol=m_api->SymbolCreate();
   if(!dst_symbol)
     {
      m_api->LoggerOutString(MTLogErr,L"failed to create symbol");
      return;
     }
//--- compiling the path of the symbols receiver group (considering the nested groups hierarchy saving settings)
   CMTStr1024 dst_path;
   if(!DestinationGet(m_to_path,(m_to_subgroups) ? src_subgroups : L"",dst_path))
     {
      m_api->LoggerOut(MTLogErr,L"failed to get symbol '%s' path",src_symbol->Symbol());
      return;
     }
//--- get symbol configuration from server
   MTAPIRES res=m_api->SymbolGet(dst_name.Str(),dst_symbol);
   if(res!=MT_RET_OK)
     {
      if(res!=MT_RET_ERR_NOTFOUND)
         m_api->LoggerOut(MTLogErr,L"failed to get symbol '%s' configuration [%s (%u)]",
                          dst_name.Str(),SMTFormat::FormatError(res),res);
      dst_symbol->Release();
      return;
     }
//--- checking the identity of the detected and deleted symbols paths
   CMTStr1024 path;
   if(!PathGet(dst_symbol,path))
     {
      m_api->LoggerOut(MTLogErr,L"failed to get symbol '%s' path",dst_name.Str());
      dst_symbol->Release();
      return;
     }
//--- delete symbol configuration
   dst_symbol->Release();
//--- completing the work if the detected symbol is in another group
   if(path.Compare(dst_path))
      return;
//--- update configuration
   res=m_api->SymbolDelete(dst_name.Str());
   if(res!=MT_RET_OK)
      m_api->LoggerOut(MTLogErr,L"failed to update symbol '%s' configuration [%s (%u)]",
                       dst_symbol->Symbol(),SMTFormat::FormatError(res),res);
//--- ok
  }
//+------------------------------------------------------------------+
//| Obtaining string parameters                                      |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::StringGet(IMTConPlugin* config,IMTConParam* param,LPCWSTR name)
  {
//--- check pointers
   if(!config || !param || !name)
      return(MT_RET_ERR_PARAMS);
//--- get parameters
   MTAPIRES res=config->ParameterGet(name, param);
   if(res!=MT_RET_OK)
      return(res);
//--- check parameters
   if((param->Type()!=IMTConParam::TYPE_STRING) || (param->ValueString()[0]==L'\0'))
      return(MT_RET_ERR_PARAMS);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Defining the symbol path considering the subgroups               |
//+------------------------------------------------------------------+
bool CPluginInstance::PathDefine(LPCWSTR value,CMTStr& path,bool& subfolders)
  {
//--- reset parameters
   path.Clear();
   subfolders=false;
//--- check pointers
   if(!value || (*value==L'\0'))
      return(false);
//--- verifying the path length
   uint32_t len=CMTStr::Len(value);
//--- verifying the length of the result
   if(len>=path.Max())
      return(false);
//--- checking the path for the subgroups flag
   if(value[len-1]==L'*')
     {
      //--- verifying the symbol path format
      if((len<3) || (value[len-2]!=L'\\'))
         return(false);
      //--- copying the symbol path
      path.Assign(value,--len);
      subfolders=true;
     }
   else
     {
      //--- copying the symbol path
      path.Assign(value,len);
      if(path[len-1]!=L'\\')
         path.Append(L"\\");
     }
//--- checking for service characters in the obtained result
   if((path.FindChar(L'*')>=0) || (path.FindChar(L'!')>=0))
      return(false);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Obtaining the symbol path                                        |
//+------------------------------------------------------------------+
bool CPluginInstance::PathGet(const IMTConSymbol* symbol,CMTStr& path)
  {
//--- reset parameters
   path.Clear();
//--- check pointers
   if(!symbol)
      return(false);
//--- obtaining the absolute symbol path
   LPCWSTR symbol_path=symbol->Path();
   if(!symbol_path || (*symbol_path==L'\0'))
      return(false);
//--- obtaining the symbol name
   LPCWSTR symbol_name=symbol->Symbol();
   if(!symbol_name || (*symbol_name==L'\0'))
      return(false);
//--- copying the symbol path
   path.Assign(symbol_path,CMTStr::Len(symbol_path)-CMTStr::Len(symbol_name));
//--- checking for the finishing '\' character (groups separation)
   return((path.Len()>0) ? (path[path.Len()-1]==L'\\') : false);
  }
//+------------------------------------------------------------------+
//| Obtaining the path with the list of nested groups                |
//+------------------------------------------------------------------+
bool CPluginInstance::SubgroupsGet(const CMTStr& path,const CMTStr& root,CMTStr& subgroups)
  {
//--- reset parameters
   subgroups.Clear();
//--- check exact matching path
   if(!root.Len() || (root.Len()>path.Len()) || root.Compare(path.Str(),root.Len()))
      return(false);
//--- copying the list of nested groups
   subgroups.Assign(path.Str()+root.Len());
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Obtaining the path of the symbols receiver group                 |
//+------------------------------------------------------------------+
bool CPluginInstance::DestinationGet(const CMTStr& root,const CMTStr& subgroups,CMTStr& path)
  {
//--- reset parameters
   path.Clear();
//--- check parameters
   if(!root.Len())
      return(false);
//--- copying the path of the root group
   path.Assign(root.Str());
   if(path[path.Len()-1]!=L'\\')
      path.Append(L"\\");
//--- adding the list of nested groups
   if(subgroups.Len())
     {
      path.Append(subgroups.Str()+((subgroups[0]==L'\\') ? 1 : 0));
      if(path[path.Len()-1]!=L'\\')
         path.Append(L"\\");
     }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Plugin parameters read function                                  |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::ParametersRead(void)
  {
//--- check pointers
   if(!m_api || !m_config)
      return(MT_RET_ERR_PARAMS);
//--- get current plugin configuration
   MTAPIRES res=m_api->PluginCurrent(m_config);
   if(res!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr, L"failed to get current plugin configuration [%s (%u)]",SMTFormat::FormatError(res),res);
      return(res);
     }
//--- create plugin parameter object   
   IMTConParam* param=m_api->PluginParamCreate();
   if(!param)
     {
      m_api->LoggerOutString(MTLogErr, L"failed to create plugin parameter object");
      return(MT_RET_ERR_MEM);
     }
//--- get parameters
//--- the source group symbols are copied from
   res=StringGet(m_config,param,L"Symbol group from");
   if(res!=MT_RET_OK)
     {
      m_api->LoggerOutString(MTLogErr, L"group is missing");
      param->Release();
      return(res);
     }
   if(!PathDefine(param->ValueString(),m_from_path,m_from_subgroups))
     {
      m_api->LoggerOutString(MTLogErr,L"invalid source group value");
      param->Release();
      return(MT_RET_ERR_PARAMS);
     }
//--- the receiver group symbols are copied to
   res=StringGet(m_config,param,L"Symbol group to");
   if(res!=MT_RET_OK)
     {
      m_api->LoggerOutString(MTLogErr, L"group is missing");
      param->Release();
      return(res);
     }
   if(!PathDefine(param->ValueString(),m_to_path,m_to_subgroups))
     {
      m_api->LoggerOutString(MTLogErr,L"invalid receiver group value");
      param->Release();
      return(MT_RET_ERR_PARAMS);
     }
//--- making sure the receiver folder is not within the source one to avoid recursive copying
   CMTStr1024 subgroups;
   if(SubgroupsGet(m_to_path,m_from_path,subgroups) && subgroups.Len())
     {
      m_api->LoggerOutString(MTLogErr,L"invalid receiver group value");
      param->Release();
      return(MT_RET_ERR_PARAMS);
     }
//--- the copied symbol name extension
   res=StringGet(m_config,param,L"Symbol extension");
   if(res!=MT_RET_OK)
     {
      m_api->LoggerOutString(MTLogErr, L"group is missing");
      param->Release();
      return(res);
     }
   m_extension.Assign(param->ValueString());
//--- free objects
   param->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+