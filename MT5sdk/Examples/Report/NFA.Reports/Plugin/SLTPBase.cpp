//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "SLTPBase.h"
//+------------------------------------------------------------------+
//| Read POD                                                         |
//+------------------------------------------------------------------+
bool SLTPRecord::Read(CMTFile &file)
  {
   return(file.Read(this,sizeof(*this))==sizeof(*this));
  }
//+------------------------------------------------------------------+
//| Read POD                                                         |
//+------------------------------------------------------------------+
bool SLTPRecord::Read(CMTFile& file,const uint32_t version,bool &invalid_version)
  {
   invalid_version=true;
   return(false);
  }
//+------------------------------------------------------------------+
//| Write POD                                                        |
//+------------------------------------------------------------------+
bool SLTPRecord::Write(CMTFile & file) const
  {
   return(file.Write(this,sizeof(*this))==sizeof(*this));
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CSLTPBase::CSLTPBase() : m_api(NULL)
  {
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CSLTPBase::~CSLTPBase(void)
  {
   Shutdown();
  }
//+------------------------------------------------------------------+
//| Reopen database file and init in-memory cache                    |
//+------------------------------------------------------------------+
bool CSLTPBase::Initialize(IMTServerAPI *api,const PluginContext &context)
  {
   Shutdown();
//--- setup api
   m_api=api;
//--- check directory
   CMTStrPath path;
   path.Format(L"%s\\Reports\\NFA.Reports\\%s\\",context.server_path.Str(),context.base_directory.Str());
   if(!CMTFile::DirectoryCreate(path))
     {
      Out(MTLogErr,L"create directory error [%s][%u]",path.Str(),GetLastError());
      return(false);
     }
//--- make database path
   path.Append(SLTPBaseFile);
   if(!BaseInitialize(path.Str(),SLTPBaseHeaderName,SLTPBaseHeaderVersion))
      return(false);
//--- build index
   return(IndexRebuild());
  }
//+------------------------------------------------------------------+
//| Index rebuild                                                    |
//+------------------------------------------------------------------+
bool CSLTPBase::IndexRebuild(void)
  {
   m_index_position.Clear();
   if(!m_index_position.Reserve(m_index_total))
      return(false);
   SLTPRecord *rec=First();
   while(rec)
     {
      m_index_position.Add(&rec);
      rec=Next(rec);
     }
   m_index_position.Sort(SortByPosition);
   return(true);
  }
//+------------------------------------------------------------------+
//| Release database file and clear in-memory cache                  |
//+------------------------------------------------------------------+
void CSLTPBase::Shutdown(void)
  {
   BaseShutdown();
   m_index_position.Shutdown();
   m_api=NULL;
  }
//+------------------------------------------------------------------+
//| Check positions                                                  |
//+------------------------------------------------------------------+
void CSLTPBase::Compact(void)
  {
//--- checks
   if(!m_api)
      return;
   IMTPosition *position=m_api->PositionCreate();
   if(position!=NULL)
     {
      Out(MTLogOK,L"compact started");
      //---
      int64_t trade_time=m_api->TimeCurrent();
      uint32_t    deleted=0;
      SLTPRecord *next=First();
      while(next)
        {
         SLTPRecord *died=next;
         next=Next(next);
         //--- check time and position exist
         if(trade_time>(died->time_create+COMPACT_CHECK_START_TIME))
            if(died->volume_ext==0 || m_api->PositionGetByTicket(died->position_id,position)==MT_RET_ERR_NOTFOUND)
              {
               Delete(died);
               deleted++;
              }
        }
      //--- compact base on disk and rebuild index
      if(deleted>0)
        {
         BaseCompact();
         IndexRebuild();
        }
      //---
      Out(MTLogOK,L"compact finished");
     }
//--- cleanup
   if(position)
      position->Release();
  }
//+------------------------------------------------------------------+
//| Record get                                                       |
//+------------------------------------------------------------------+
bool CSLTPBase::RecordGet(const uint64_t position,SLTPRecord& record)
  {
//--- find record
   SLTPRecord **ptr=(SLTPRecord**)m_index_position.Search(&position,SearchByPosition);
   if(ptr && *ptr)
     {
      record=*(*ptr);
      return(true);
     }
//---
   return(false);
  }
//+------------------------------------------------------------------+
//| Record update                                                    |
//+------------------------------------------------------------------+
bool CSLTPBase::RecordUpdate(SLTPRecord& record,const bool allow_add/*=true*/)
  {
//--- find record
   SLTPRecord **ptr=(SLTPRecord**)m_index_position.Search(&record.position_id,SearchByPosition),*rec;
   if(ptr && *ptr)
     {
      rec=*ptr;
      *rec=record;
      return(Update(rec));
     }
//--- add new record
   if(allow_add)
     {
      SLTPRecord *added=NULL;
      if(Add(&record,&added))
        {
         m_index_position.Insert(&added,SortByPosition);
         return(true);
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Record delete                                                    |
//+------------------------------------------------------------------+
bool CSLTPBase::RecordDelete(const uint64_t position)
  {
//--- find record
   SLTPRecord **ptr=(SLTPRecord**)m_index_position.Search(&position,SearchByPosition);
   if(ptr)
     {
      //--- delete record
      if(!Delete(*ptr))
         return(false);
      m_index_position.Delete(ptr);
     }
   return(true);
  }
//+------------------------------------------------------------------+
//| Logger for RecordDatabase                                        |
//+------------------------------------------------------------------+
void CSLTPBase::Out(uint32_t code,LPCWSTR msg,...)
  {
   if(!m_api)
      return;
//---
   wchar_t buffer[1024];
   va_list args;

   va_start(args,msg);
   _vsnwprintf_s(buffer,_countof(buffer),_TRUNCATE,msg,args);
   va_end(args);
//--- out
   m_api->LoggerOut(code,L"SLTP: %s",buffer);
  }
//+------------------------------------------------------------------+
//| Sort function                                                    |
//+------------------------------------------------------------------+
int32_t CSLTPBase::SortByPosition(const void *left,const void *right)
  {
   SLTPRecord *lft=*(SLTPRecord**)left;
   SLTPRecord *rgh=*(SLTPRecord**)right;
//---
   if(lft->position_id<rgh->position_id) return(1);
   if(lft->position_id>rgh->position_id) return(-1);
   return(0);
  }
//+------------------------------------------------------------------+
//| Search function                                                  |
//+------------------------------------------------------------------+
int32_t CSLTPBase::SearchByPosition(const void *left,const void *right)
  {
   uint64_t      lft=*(uint64_t*)left;
   SLTPRecord *rgh=*(SLTPRecord**)right;
//---
   if(lft<rgh->position_id) return(1);
   if(lft>rgh->position_id) return(-1);
   return(0);
  }
//+------------------------------------------------------------------+
