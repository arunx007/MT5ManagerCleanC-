//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "FtdSection.h"
//+------------------------------------------------------------------+
//| Column description FTD                                           |
//+------------------------------------------------------------------+
ReportColumn CFtdSection::s_columns_ftd[]=
  {
   //--- id,name,  type,                           width, width_max,offset,                                             size,digits_column,flags
     { 1, L"Date"   ,IMTDatasetColumn::TYPE_DATE    ,20,0, offsetof(FtdRecord,date)                                      ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2, L"Value"  ,IMTDatasetColumn::TYPE_MONEY   ,20,0, offsetof(FtdRecord,sections)+offsetof(FtdSectionRecord,value) ,0,0,0 },
   { 3, L"Amount" ,IMTDatasetColumn::TYPE_MONEY   ,20,0, offsetof(FtdRecord,sections)+offsetof(FtdSectionRecord,amount),0,0,0 },
   { 4, L"Count"  ,IMTDatasetColumn::TYPE_UINT64  ,20,0, offsetof(FtdRecord,sections)+offsetof(FtdSectionRecord,count) ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CFtdSection::CFtdSection(CCurrencyConverter &currency,const CReportInterval &interval) :
   m_currency(currency),m_interval(interval),m_ftd(true),m_total(true)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CFtdSection::~CFtdSection(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CFtdSection::Clear(void)
  {
//--- clear objects
   m_ftd.Clear();
   m_total.Clear();
   m_top.Clear();
   m_names.Clear();
  }
//+------------------------------------------------------------------+
//| Initialize                                                       |
//+------------------------------------------------------------------+
MTAPIRES CFtdSection::Initialize(void)
  {
//--- initialize ftd matrix
   if(!m_ftd.Initialize(m_interval.PeriodTotal(),0x100))
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Add deposit                                                      |
//+------------------------------------------------------------------+
MTAPIRES CFtdSection::DepositAdd(const int64_t time,const double value,const uint32_t section)
  {
//--- ftd by deal time and section
   Ftd *ftd=FtdByTimeAndSection(time,section);
   if(!ftd)
      return(MT_RET_ERROR);
//--- add deposit
   ftd->DealAdd(value,1,m_currency);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Ftd by time and section                                          |
//+------------------------------------------------------------------+
CFtdSection::Ftd* CFtdSection::FtdByTimeAndSection(const int64_t time,const uint32_t section)
  {
//--- period index by time
   int32_t period=m_interval.PeriodIndex(time);
   if(period<0)
      period=0;
//--- return matrix item
   return(m_ftd.Item(period,section));
  }
//+------------------------------------------------------------------+
//| Calculate total data                                             |
//+------------------------------------------------------------------+
MTAPIRES CFtdSection::CalculateTotal(CReportParameter &params)
  {
//--- calulate total vector
   Ftd::COperationAdd operation(m_currency);
   if(!m_ftd.OperationColumns(m_total,operation))
      return(MT_RET_ERR_MEM);
//--- calulate top
   return(m_top.InitializeOther(m_total,Ftd::SortAmountDesc,operation,params));
  }
//+------------------------------------------------------------------+
//| Read top sections names                                          |
//+------------------------------------------------------------------+
MTAPIRES CFtdSection::ReadTopNames(const CReportCache &cache,const uint32_t dictionary_id)
  {
   return(cache.ReadTopNames(dictionary_id,m_top,m_names));
  }
//+------------------------------------------------------------------+
//| Fill top sections names by money group                           |
//+------------------------------------------------------------------+
MTAPIRES CFtdSection::MoneyGroupTopNames(void)
  {
   return(SReportMoneyGroup::FillTopNames(m_top,m_names,m_currency));
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CFtdSection::PrepareGraphs(IMTReportAPI &api,LPCWSTR section_type,const int32_t y)
  {
//--- check parameter
   if(!section_type)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=api.DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- prepare FTD data
   MTAPIRES res=PrepareData(*data);
   if(res!=MT_RET_OK)
      return(res);
//--- prepare graph FTD
   CMTStr128 name(L"FTD by ");
   if(y<0)
     {
      name.Append(section_type);
      if((res=PrepareGraph(api,data,m_currency.Append(name).Str(),2,y))!=MT_RET_OK)
         return(res);
     }
//--- prepare graph amount by month
   name.Assign(L"FTD Amount by ");
   name.Append(section_type);
   if((res=PrepareGraph(api,data,m_currency.Append(name).Str(),3,y))!=MT_RET_OK)
      return(res);
//--- prepare graph count by month
   if(y<0)
     {
      name.Assign(L"FTD Count by ");
      name.Append(section_type);
      if((res=PrepareGraph(api,data,name.Str(),4,y))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare FTD data                                                 |
//+------------------------------------------------------------------+
MTAPIRES CFtdSection::PrepareData(IMTDataset &data) const
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
   const uint32_t row_size=sizeof(FtdRecord)+(top_total-1)*sizeof(FtdSectionRecord);
   if(!pack.Reallocate(row_size))
      return(MT_RET_ERR_MEM);
   FtdRecord *row=(FtdRecord *)pack.Buffer();
   if(!row)
      return(MT_RET_ERR_MEM);
//--- iterate over each month
   for(uint32_t i=0,total=m_ftd.Width();i<total;i++)
     {
      //--- date
      row->date=m_interval.PeriodDate(i);
      //--- ftd column
      const FtdVector *column=m_ftd.Column(i);
      if(!column)
         return(MT_RET_ERROR);
      //--- fill data row
      if(!FillRecord(*row,*column))
         return(MT_RET_ERROR);
      //--- write data row
      if((res=data.RowWrite(pack.Buffer(),row_size))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare FTD data columns                                         |
//+------------------------------------------------------------------+
MTAPIRES CFtdSection::PrepareDataColumns(IMTDataset &data) const
  {
//--- create first dataset column
   MTAPIRES res=ReportColumn::ReportColumnsAdd(data,s_columns_ftd,1);
   if(res!=MT_RET_OK)
      return(res);
//--- initialize dataset section columns
   ReportColumn columns[_countof(s_columns_ftd)-1];
   memcpy(columns,s_columns_ftd+1,sizeof(columns));
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
         columns[j].offset+=sizeof(FtdSectionRecord);
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Graph                                                    |
//+------------------------------------------------------------------+
MTAPIRES CFtdSection::PrepareGraph(IMTReportAPI &api,IMTDataset *data,LPCWSTR title,const uint32_t column_id,const int32_t y)
  {
//--- checks
   if(!data || !title)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=api.DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   MTAPIRES res=chart->Type(IMTReportDashboardWidget::WIDGET_TYPE_CHART_AREA);
   if(res!=MT_RET_OK)
      return(res);
//--- set stacking
   if((res=chart->ChartStackType(IMTReportDashboardWidget::CHART_STACK_ACCUMULATION))!=MT_RET_OK)
      return(res);
//--- set title
   if((res=chart->Title(title))!=MT_RET_OK)
      return(res);
//--- set top
   if(y>=0)
      if((res=chart->Top(CHART_HEIGHT_PRIMARY*y))!=MT_RET_OK)
         return(res);
//--- set height
   if((res=chart->Height(CHART_HEIGHT_PRIMARY))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- add data column
   for(uint32_t i=0,total=m_top.Total();i<total;i++)
      if((res=chart->DataColumnAdd(column_id+i*3))!=MT_RET_OK)
         return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Fill record                                                      |
//+------------------------------------------------------------------+
bool CFtdSection::FillRecord(FtdRecord &row,const FtdVector &sections) const
  {
//--- iterate accumulated sections
   for(uint32_t i=0,total=m_top.Total();i<total;i++)
      if(const uint32_t *pos=m_top.Pos(i))
        {
         //--- check position
         if(*pos==FtdTop::TOP_POS_OTHER)
           {
            //--- calculate other
            Ftd other={0};
            Ftd::COperationAdd operation(m_currency);
            m_top.CalculateOther(other,sections,operation);
            //--- fill record section
            FillRecordSection(row.sections[i],&other);
           }
         else
           {
            //--- fill record section
            FillRecordSection(row.sections[i],sections.Item(*pos));
           }
        }
      else
         return(false);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Fill record section                                              |
//+------------------------------------------------------------------+
void CFtdSection::FillRecordSection(FtdSectionRecord &record,const Ftd *ftd) const
  {
//--- check ftd
   if(ftd)
     {
      //--- fill section
      record.value   =ftd->Medium(m_currency);
      record.amount  =ftd->amount;
      record.count   =ftd->total;
     }
   else
      ZeroMemory(&record,sizeof(record));
  }
//+------------------------------------------------------------------+
