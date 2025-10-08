//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ActivitySection.h"
//+------------------------------------------------------------------+
//| Column description inactive clients                              |
//+------------------------------------------------------------------+
ReportColumn CActivitySection::s_columns_inactive[]=
   //--- id,name,  type,                           width, width_max,offset,               size,digits_column,flags
  {{ 1, L"Channel",IMTDatasetColumn::TYPE_STRING  ,20,0, offsetof(InactiveRecord,section),MtFieldSize(InactiveRecord,section),0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2, L"Count"  ,IMTDatasetColumn::TYPE_UINT32  ,20,0, offsetof(InactiveRecord,count)  ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CActivitySection::CActivitySection(const bool geo) :
   m_geo(geo),m_inactive(false)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CActivitySection::~CActivitySection(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CActivitySection::Clear(void)
  {
//--- clear objects
   m_inactive.Clear();
   m_inactive_top.Clear();
   m_inactive_names.Clear();
  }
//+------------------------------------------------------------------+
//| Initialize                                                       |
//+------------------------------------------------------------------+
MTAPIRES CActivitySection::Initialize(void)
  {
//--- initialize inactive clients vector
   if(!m_inactive.Initialize(0x100))
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Add inactive client                                              |
//+------------------------------------------------------------------+
MTAPIRES CActivitySection::InactiveAdd(const uint32_t section)
  {
//--- get counter by section
   uint32_t *counter=m_inactive.Item(section);
   if(!counter)
      return(MT_RET_ERR_MEM);
//--- count client
   (*counter)++;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Calculate total data                                             |
//+------------------------------------------------------------------+
MTAPIRES CActivitySection::CalculateTotal(CReportParameter &params)
  {
//--- calulate inactive clients unlimited top for geo section
   if(m_geo)
      return(m_inactive_top.Initialize(m_inactive,SortUIntDesc));
//--- calulate inactive clients top
   CUIntOperationAdd operation_uint;
   return(m_inactive_top.InitializeOther(m_inactive,SortUIntDesc,operation_uint,params));
  }
//+------------------------------------------------------------------+
//| Read top sections names                                          |
//+------------------------------------------------------------------+
MTAPIRES CActivitySection::ReadTopNames(const CReportCache &cache,const uint32_t dictionary_id)
  {
//--- read inactive clients top sections names from cache
   return(cache.ReadTopNames(dictionary_id,m_inactive_top,m_inactive_names));
  }
//+------------------------------------------------------------------+
//| set section name                                                 |
//+------------------------------------------------------------------+
MTAPIRES CActivitySection::SetSectionName(const uint32_t section,LPCWSTR name)
  {
   if(!name)
      return(MT_RET_ERR_PARAMS);
//--- search section
   for(uint32_t i=0,total=m_inactive_top.Total();i<total;i++)
     {
      //--- compare section
      const uint32_t *pos=m_inactive_top.Pos(i);
      if(pos && *pos==section)
        {
         //--- set section name
         if(!m_inactive_names.String(i,name))
            return(MT_RET_ERR_MEM);
         break;
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CActivitySection::PrepareGraphs(IMTReportAPI &api,LPCWSTR section_type,const uint32_t x,const uint32_t y)
  {
//--- check parameter
   if(!section_type)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=api.DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- prepare inactive clients data
   MTAPIRES res=PrepareDataInactive(*data);
   if(res!=MT_RET_OK)
      return(res);
//--- prepare graph count by month
   CMTStr128 name(*section_type==L' ' ? L"Inactive Clients Count by" : L"");
   name.Append(section_type);
   if((res=PrepareGraph(api,data,name.Str(),x,y))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare inactive clients data                                    |
//+------------------------------------------------------------------+
MTAPIRES CActivitySection::PrepareDataInactive(IMTDataset &data) const
  {
//--- create first dataset column
   MTAPIRES res=ReportColumn::ReportColumnsAdd(data,s_columns_inactive,_countof(s_columns_inactive));
   if(res!=MT_RET_OK)
      return(res);
//--- iterate top sections
   for(uint32_t i=0,total=m_inactive_top.Total();i<total;i++)
     {
      //--- top section pos
      const uint32_t *pos=m_inactive_top.Pos(i);
      if(!pos)
         return(MT_RET_ERROR);
      //--- get section name
      LPCWSTR name=m_inactive_names.String(i);
      if(!name)
         return(MT_RET_ERROR);
      //--- fill section name
      InactiveRecord row;
      CMTStr::Copy(row.section,name);
      //--- check position
      if(*pos==CReportTopBase::TOP_POS_OTHER)
        {
         //--- calculate other
         row.count=0;
         CUIntOperationAdd operation;
         m_inactive_top.CalculateOther(row.count,m_inactive,operation);
        }
      else
        {
         //--- inactive section
         const uint32_t *inactive=m_inactive.Item(*pos);
         if(!inactive)
            return(MT_RET_ERROR);
         //--- fill record section
         row.count=*inactive;
        }
      //--- write data row
      if((res=data.RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Graph                                                    |
//+------------------------------------------------------------------+
MTAPIRES CActivitySection::PrepareGraph(IMTReportAPI &api,IMTDataset *data,LPCWSTR title,const uint32_t x,const uint32_t y)
  {
//--- checks
   if(!data || !title)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=api.DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   MTAPIRES res=chart->Type(m_geo ? IMTReportDashboardWidget::WIDGET_TYPE_CHART_GEO : IMTReportDashboardWidget::WIDGET_TYPE_CHART_PIE);
   if(res!=MT_RET_OK)
      return(res);
//--- set title
   if((res=chart->Title(title))!=MT_RET_OK)
      return(res);
//--- set left position
   if((res=chart->Left(32*x))!=MT_RET_OK)
      return(res);
//--- set top position
   if((res=chart->Top(CHART_HEIGHT_PRIMARY*y))!=MT_RET_OK)
      return(res);
//--- set width
   if((res=chart->Width(32))!=MT_RET_OK)
      return(res);
//--- set height
   if((res=chart->Height(CHART_HEIGHT_PRIMARY))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| sort uint descending                                             |
//+------------------------------------------------------------------+
int32_t CActivitySection::SortUIntDesc(const void *left,const void *right)
  {
//--- type conversion
   const uint32_t lft=**(const uint32_t* const*)left;
   const uint32_t rgh=**(const uint32_t* const*)right;
//--- ltv comparsion
   if(lft<rgh)
      return(1);
   if(lft>rgh)
      return(-1);
   return(0);
  }
//+------------------------------------------------------------------+
