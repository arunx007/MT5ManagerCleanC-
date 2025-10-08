//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DealSectionReport.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDealSectionReport::CDealSectionReport(const MTReportInfo &info,const CDealCache::EnDealKeyFields section_field) :
   CDealReport(info,false),m_section_field(section_field),m_table(true),m_total(true)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDealSectionReport::~CDealSectionReport(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CDealSectionReport::Clear(void)
  {
//--- clear containers
   m_table.Clear();
   m_total.Clear();
   m_top.Clear();
   m_top_total.Clear();
//--- base call
   CDealReport::Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CDealSectionReport::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- base call
   MTAPIRES res=CDealReport::Prepare();
   if(res!=MT_RET_OK)
      return(res);
//--- initialize matrix
   if(!m_table.Initialize(m_interval.PeriodTotal(),0x100))
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Request data from server and calculate all statistics            |
//+------------------------------------------------------------------+
MTAPIRES CDealSectionReport::Calculate(void)
  {
//--- base call
   MTAPIRES res=CDealReport::Calculate();
   if(res!=MT_RET_OK)
      return(res);
//--- calulate total vector
   Section::COperationAdd operation(m_currency);
   if(!m_table.OperationColumns(m_total,operation))
      return(MT_RET_ERR_MEM);
//--- calulate tops
   if((res=m_top.InitializeOther(m_total,Section::SortAmountDesc,operation,m_params))!=MT_RET_OK)
      return(res);
   if((res=m_top_total.Initialize(m_total,Section::SortAmountDesc))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| deal hour read handler                                           |
//+------------------------------------------------------------------+
MTAPIRES CDealSectionReport::DealHourRead(const uint64_t time,const CDealCache::DealKey &key,const CDealCache::DealHour &hour,const double *rate)
  {
//--- get record by deal time
   Section *section=RecordByTimeAndSection(time,CDealCache::KeyField(key,m_section_field));
   if(!section)
      return(MT_RET_ERR_MEM);
//--- add deal hour to section
   DealAdd(*section,time,hour,rate);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| record by time and section                                       |
//+------------------------------------------------------------------+
CDealSectionReport::Section* CDealSectionReport::RecordByTimeAndSection(const int64_t ctm,const uint32_t section)
  {
//--- calculate period index
   const int32_t period=m_interval.PeriodIndex(ctm);
   if(period<0)
      return(nullptr);
//--- return matrix item
   return(m_table.Item(period,section));
  }
//+------------------------------------------------------------------+
//| Prepare data                                                     |
//+------------------------------------------------------------------+
MTAPIRES CDealSectionReport::PrepareData(IMTDataset &data)
  {
//--- check
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- prepare dataset columns
   MTAPIRES res=PrepareDataColumns(data);
   if(res!=MT_RET_OK)
      return(res);
//--- check sections
   const uint32_t section_count=m_top.Total();
   if(!section_count)
      return(MT_RET_OK_NONE);
//--- initialize accumulated vector with NAN
   DoubleVector accumulated(true);
   if(!accumulated.InitializeValue(section_count,NAN))
      return(MT_RET_ERR_MEM);
//--- row buffer
   CMTMemPack pack;
   const uint32_t row_size=sizeof(Row)+(section_count-1)*sizeof(Record);
   if(!pack.Reallocate(row_size))
      return(MT_RET_ERR_MEM);
   Row *row=(Row *)pack.Buffer();
   if(!row)
      return(MT_RET_ERR_MEM);
//--- write each date
   for(uint32_t i=0,total=m_table.Width();i<total;i++)
     {
      //--- zero data row
      ZeroMemory(row,row_size);
      //--- get sections
      const SectionVector *sections=m_table.Column(i);
      if(!sections)
         return(MT_RET_ERROR);
      //--- calculate date
      row->date=m_interval.PeriodDate(i);
      //--- fill data row
      FillRow(*row,*sections,accumulated);
      //--- write data row
      if((res=data.RowWrite(pack.Buffer(),row_size))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare data total                                               |
//+------------------------------------------------------------------+
MTAPIRES CDealSectionReport::PrepareDataTotal(IMTDataset &data)
  {
//--- check
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- prepare dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(data,s_columns_total,_countof(s_columns_total));
   if(res!=MT_RET_OK)
      return(res);
//--- fill top names
   CReportDictionary names;
   if((res=FillTopNames(m_section_field,m_top_total,names))!=MT_RET_OK)
      return(res);
//--- iterate sections
   for(uint32_t i=0,total=m_top_total.Total();i<total;i++)
     {
      //--- zero data row
      RowTotal row={};
      //--- get section name
      CMTStr::Copy(row.section,names.String(i));
      //--- item position in vector
      const uint32_t *pos=m_top_total.Pos(i);
      if(pos)
        {
         //--- section
         const Section *section=m_total.Item(*pos);
         //--- fill data total row
         if(section)
            FillRowTotal(row,*section);
        }
      //--- write data row
      if((res=data.RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare dataset columns                                          |
//+------------------------------------------------------------------+
MTAPIRES CDealSectionReport::PrepareDataColumns(IMTDataset &data)
  {
//--- create first dataset column
   MTAPIRES res=ReportColumn::ReportColumnsAdd(data,s_columns,1);
   if(res!=MT_RET_OK)
      return(res);
//---- initialize dataset sections columns
   CMTStr256 section;
   ReportColumn columns[_countof(s_columns)-1];
   for(uint32_t i=0;i<_countof(columns);i++)
     {
      columns[i]=s_columns[i+1];
      columns[i].name=section.Str();
     }
//--- fill top names
   CReportDictionary names;
   if((res=FillTopNames(m_section_field,m_top,names))!=MT_RET_OK)
      return(res);
//--- iterate sections
   for(uint32_t i=0,total=m_top.Total();i<total;i++)
     {
      //--- get section name
      section.Assign(names.String(i));
      //--- add columns to dataset
      if((res=ReportColumn::ReportColumnsAdd(data,columns,_countof(columns)))!=MT_RET_OK)
         break;
      //--- next date
      for(uint32_t i=0;i<_countof(columns);i++)
        {
         columns[i].id+=_countof(s_columns)-1;
         columns[i].offset+=sizeof(Record);
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Fill top names                                                   |
//+------------------------------------------------------------------+
MTAPIRES CDealSectionReport::FillTopNames(const uint32_t dictionary_id,const CReportTopBase &top,CReportDictionary &names)
  {
//--- check deals cache
   if(!m_deals)
      return(MT_RET_ERR_PARAMS);
//--- top count
   if(!top.Total())
      return(MT_RET_OK);
//--- begin read deals
   MTAPIRES res=m_deals->ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
//--- read deals top names
   res=m_deals->ReadTopNames(dictionary_id,top,names);
//--- end read deals
   const MTAPIRES res_deals=m_deals->ReadEnd();
//--- check error
   if(res!=MT_RET_OK)
      return(res);
//--- return error code
   return(res_deals);
  }
//+------------------------------------------------------------------+
//| Fill row                                                         |
//+------------------------------------------------------------------+
void CDealSectionReport::FillRow(Row &row,const SectionVector &sections,DoubleVector &accumulated) const
  {
//--- iterate accumulated sections
   static const Section empty={0};
   for(uint32_t i=0,total=m_top.Total();i<total;i++)
      if(const uint32_t *pos=m_top.Pos(i))
        {
         //--- destination section
         Record &dst=row.sections[i];
         //--- check other section
         if(*pos==SectionTop::TOP_POS_OTHER)
           {
            //--- calculate other
            Section other={0};
            Section::COperationAdd operation(m_currency);
            m_top.CalculateOther(other,sections,operation);
            //--- copy other
            FillRecord(dst,other,accumulated.Item(i));
           }
         else
           {
            //--- fill section row
            const Section *src=sections.Item(*pos);
            if(!src)
               src=&empty;
            FillRecord(dst,*src,accumulated.Item(i));
           }
        }
  }
//+------------------------------------------------------------------+
//| prepare graph                                                    |
//+------------------------------------------------------------------+
MTAPIRES CDealSectionReport::PrepareGraph(IMTDataset *data,const ReportColumn &column,const IMTReportDashboardWidget::EnWidgetType type)
  {
//--- prepare graph
   return(CDealReport::PrepareGraph(data,column,column.id==COLUMN_IN_OUT || column.id==COLUMN_IN_OUT_ACC ? IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE : type));
  }
//+------------------------------------------------------------------+
//| chart columns add                                                |
//+------------------------------------------------------------------+
MTAPIRES CDealSectionReport::ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const
  {
//--- add data columns for top
   MTAPIRES res=MT_RET_OK;
   for(uint32_t i=0,total=m_top.Total();i<total && res==MT_RET_OK;i++)
      res=chart.DataColumnAdd(column.id+i*(_countof(s_columns)-1));
   return(res);
  }
//+------------------------------------------------------------------+
