//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "RetentionSection.h"
//+------------------------------------------------------------------+
//| Column description retention                                     |
//+------------------------------------------------------------------+
ReportColumn CRetentionSection::s_columns_retention[]=
  {
   //--- id,name,  type,                           width, width_max,offset,                 size,digits_column,flags
     { 1, L"Day"    ,IMTDatasetColumn::TYPE_STRING  ,20,0, offsetof(RetentionRecord,day)     ,MtFieldSize(RetentionRecord,day),0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2, L"Count"  ,IMTDatasetColumn::TYPE_DOUBLE  ,20,0, offsetof(RetentionRecord,sections),0,0,0 },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CRetentionSection::CRetentionSection(void) :
   m_retention(false)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CRetentionSection::~CRetentionSection(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CRetentionSection::Clear(void)
  {
//--- clear objects
   m_retention.Clear();
   m_top.Clear();
   m_names.Clear();
  }
//+------------------------------------------------------------------+
//| Initialize                                                       |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSection::Initialize(void)
  {
//--- initialize retention matrix
   if(!m_retention.Initialize(STEP_MAX,0x100))
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Add lifetime                                                     |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSection::LifetimeAdd(const int64_t lifetime,const uint32_t section)
  {
//--- get record by deal time relative user registration
   uint32_t *count=RetentionByTimeAndSection(lifetime,section);
   if(!count)
      return(MT_RET_ERR_MEM);
//--- count lifetime
   (*count)++;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Retention by time and section                                    |
//+------------------------------------------------------------------+
UINT* CRetentionSection::RetentionByTimeAndSection(const int64_t lifetime,const uint32_t section)
  {
//--- calculate week index
   uint32_t week=0;
   if(lifetime)
      week=(uint32_t)(lifetime/STEP_DURATION);
//--- return matrix item
   return(m_retention.Item(std::min(week,uint32_t(STEP_MAX-1)),section));
  }
//+------------------------------------------------------------------+
//| Calculate total data                                             |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSection::CalculateTotal(CReportParameter &params)
  {
//--- accumulate columns values and calulate total vector
   CUIntOperationAdd operation;
   const RetentionVector *total=m_retention.OperationColumnsReverse(operation);
   if(!total)
      return(MT_RET_OK);
//--- calulate top
   return(m_top.InitializeOther(*total,SortRetentionDesc,operation,params));
  }
//+------------------------------------------------------------------+
//| Calculate period sections                                        |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSection::CalculatePeriod(const CReportInterval &interval)
  {
//--- accumulate columns values and calulate total vector
   CUIntOperationAdd operation;
   const RetentionVector *total=m_retention.OperationColumnsReverse(operation);
   if(!total)
      return(MT_RET_OK);
//--- calulate fake top
   MTAPIRES res=m_top.InitializeNoOrder(*total);
   if(res!=MT_RET_OK)
      return(res);
//--- initialize top names
   if((res=FillPeriodTopNames(interval,m_top,m_names))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| fill period top names                                            |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSection::FillPeriodTopNames(const CReportInterval &interval,const CReportTopBase &top,CReportDictionary &names)
  {
//--- initialize top names
   if(!names.Initialize(top.Total()))
      return(MT_RET_ERR_MEM);
//--- initialize period names
   for(uint32_t i=0,total=top.Total();i<total;i++)
     {
      //--- format period
      CMTStr64 period;
      if(!names.String(i,interval.FormatPeriod(period,i).Str()))
         return(MT_RET_ERR_MEM);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Read top sections names                                          |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSection::ReadTopNames(const CReportCache &cache,const uint32_t dictionary_id)
  {
//--- read top sections names from cache
   return(cache.ReadTopNames(dictionary_id,m_top,m_names));
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSection::PrepareGraphs(IMTReportAPI &api,LPCWSTR section_type,LPCWSTR description)
  {
//--- check parameter
   if(!section_type)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=api.DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- prepare retention data
   MTAPIRES res=PrepareData(*data);
   if(res!=MT_RET_OK)
      return(res);
//--- prepare graph retention
   CMTStr128 name(L"Retention by ");
   name.Append(section_type);
   name.Append(L", %");
   if((res=PrepareGraph(api,data,name.Str(),description,IMTReportDashboardWidget::WIDGET_TYPE_CHART_SPLINE,2))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare retention data                                           |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSection::PrepareData(IMTDataset &data) const
  {
//--- create first dataset column
   MTAPIRES res=PrepareDataColumns(data);
   if(res!=MT_RET_OK)
      return(res);
//--- check section
   const uint32_t top_total=m_top.Total();
   if(!top_total)
      return(MT_RET_OK);
//--- row buffer
   CMTMemPack pack;
   const uint32_t row_size=sizeof(RetentionRecord)+(top_total-1)*sizeof(RetentionRecord::sections[0]);
   if(!pack.Reallocate(row_size))
      return(MT_RET_ERR_MEM);
   RetentionRecord *row=(RetentionRecord *)pack.Buffer();
   if(!row)
      return(MT_RET_ERR_MEM);
//--- total days
   const uint32_t total=m_retention.Width();
   if(!total)
      return(MT_RET_OK);
//--- first day
   const RetentionVector *first=m_retention.Column(0);
   if(!first)
      return(MT_RET_OK);
//--- calculate other for first day
   uint32_t other_first=0;
   CUIntOperationAdd operation;
   m_top.CalculateOther(other_first,*first,operation);
//--- iterate over each day
   for(uint32_t i=0;i<total;i++)
     {
      //--- label
      CMTStr32 day;
      CMTStr::FormatStr(row->day,L"%u",i+1);
      //--- fill data row
      const RetentionVector *column=m_retention.Column(i);
      if(column)
         if(!FillRecord(*row,*column,*first,other_first))
            return(MT_RET_ERROR);
      //--- write data row
      if((res=data.RowWrite(pack.Buffer(),row_size))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare retention data columns                                   |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSection::PrepareDataColumns(IMTDataset &data) const
  {
//--- create first dataset column
   MTAPIRES res=ReportColumn::ReportColumnsAdd(data,s_columns_retention,1);
   if(res!=MT_RET_OK)
      return(res);
//--- initialize dataset section columns
   ReportColumn columns[_countof(s_columns_retention)-1];
   memcpy(columns,s_columns_retention+1,sizeof(columns));
//--- iterate top
   for(uint32_t i=0,total=m_top.Total();i<total;i++)
     {
      //--- get section name
      LPCWSTR name=m_names.String(i);
      if(!name)
         return(MT_RET_ERROR);
      //--- initialize dataset section columns name
      for(uint32_t j=0;j<_countof(columns);j++)
         columns[j].name=name;
      //--- add columns to dataset
      if((res=ReportColumn::ReportColumnsAdd(data,columns,_countof(columns)))!=MT_RET_OK)
         return(res);
      //--- next section
      for(uint32_t j=0;j<_countof(columns);j++)
        {
         columns[j].id+=_countof(columns);
         columns[j].offset+=sizeof(RetentionRecord::sections[0]);
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Graph                                                    |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSection::PrepareGraph(IMTReportAPI &api,IMTDataset *data,LPCWSTR title,LPCWSTR description,const uint32_t type,const uint32_t column_id)
  {
//--- checks
   if(!data || !title)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=api.DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   MTAPIRES res=chart->Type(type);
   if(res!=MT_RET_OK)
      return(res);
//--- set title
   if((res=chart->Title(title))!=MT_RET_OK)
      return(res);
//--- set description
   if(description)
      if((res=chart->Description(description))!=MT_RET_OK)
         return(res);
//--- set height
   if((res=chart->Height(CHART_HEIGHT_PRIMARY))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- add data column
   for(uint32_t i=0,total=m_top.Total();i<total;i++)
      if((res=chart->DataColumnAdd(column_id+i))!=MT_RET_OK)
         return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Fill record                                                      |
//+------------------------------------------------------------------+
bool CRetentionSection::FillRecord(RetentionRecord &row,const RetentionVector &sections,const RetentionVector &first,const uint32_t other_first) const
  {
//--- iterate accumulated sections
   for(uint32_t i=0,total=m_top.Total();i<total;i++)
      if(const uint32_t *pos=m_top.Pos(i))
        {
         //--- check position
         if(*pos==RetentionTop::TOP_POS_OTHER)
           {
            //--- calculate other
            uint32_t other=0;
            CUIntOperationAdd operation;
            m_top.CalculateOther(other,sections,operation);
            //--- fill record section
            row.sections[i]=(other_first && other) ? other*100.0/other_first : NAN;
           }
         else
           {
            //--- retention by section
            const uint32_t *retention_first=first.Item(*pos);
            const uint32_t *retention=sections.Item(*pos);
            row.sections[i]=(retention_first && *retention_first && retention && *retention) ? *retention*100.0/(*retention_first) : NAN;
           }
        }
      else
         return(false);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| sort retention count descending                                  |
//+------------------------------------------------------------------+
int32_t CRetentionSection::SortRetentionDesc(const void *left,const void *right)
  {
//--- type conversion
   const uint32_t *lft=*(const uint32_t* const*)left;
   const uint32_t *rgh=*(const uint32_t* const*)right;
//--- retention count comparsion
   if(*lft<*rgh)
      return(1);
   if(*lft>*rgh)
      return(-1);
   return(0);
  }
//+------------------------------------------------------------------+
