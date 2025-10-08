//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "FtdTotalSection.h"
//+------------------------------------------------------------------+
//| Column description FTD                                           |
//+------------------------------------------------------------------+
ReportColumn CFtdTotalSection::s_columns_ftd[]=
   //--- id,name,  type,                           width, width_max,offset,                                             size,digits_column,flags
  {{ 1, L"Channel",IMTDatasetColumn::TYPE_STRING  ,20,0, offsetof(FtdSectionRecord,section)  ,MtFieldSize(FtdSectionRecord,section),0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2, L"Value"  ,IMTDatasetColumn::TYPE_MONEY   ,20,0, offsetof(FtdSectionRecord,value)    ,0,0,0 },
   { 3, L"Amount" ,IMTDatasetColumn::TYPE_MONEY   ,20,0, offsetof(FtdSectionRecord,amount)   ,0,0,0 },
   { 4, L"Count"  ,IMTDatasetColumn::TYPE_UINT64  ,20,0, offsetof(FtdSectionRecord,count)    ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CFtdTotalSection::CFtdTotalSection(CCurrencyConverter &currency,const bool geo) :
   m_currency(currency),m_geo(geo),m_total(false)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CFtdTotalSection::~CFtdTotalSection(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CFtdTotalSection::Clear(void)
  {
//--- clear objects
   m_total.Clear();
   m_top.Clear();
   m_names.Clear();
  }
//+------------------------------------------------------------------+
//| Initialize                                                       |
//+------------------------------------------------------------------+
MTAPIRES CFtdTotalSection::Initialize(void)
  {
//--- initialize ftd vector
   if(!m_total.Initialize(0x100))
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Add deposit                                                      |
//+------------------------------------------------------------------+
MTAPIRES CFtdTotalSection::DepositAdd(const double value,const uint32_t section)
  {
//--- ftd by section
   Ftd *ftd=m_total.Item(section);
   if(!ftd)
      return(MT_RET_ERROR);
//--- add deposit
   ftd->DealAdd(value,1,m_currency);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Calculate top data                                               |
//+------------------------------------------------------------------+
MTAPIRES CFtdTotalSection::CalculateTop(CReportParameter &params)
  {
//--- calculate unlimited top for geo section
   if(m_geo)
      return(m_top.Initialize(m_total,Ftd::SortAmountDesc));
//--- calculate top
   Ftd::COperationAdd operation(m_currency);
   return(m_top.InitializeOther(m_total,Ftd::SortAmountDesc,operation,params));
  }
//+------------------------------------------------------------------+
//| Read top sections names                                          |
//+------------------------------------------------------------------+
MTAPIRES CFtdTotalSection::ReadTopNames(const CReportCache &cache,const uint32_t dictionary_id)
  {
   return(cache.ReadTopNames(dictionary_id,m_top,m_names));
  }
//+------------------------------------------------------------------+
//| Fill top sections names by money group                           |
//+------------------------------------------------------------------+
MTAPIRES CFtdTotalSection::MoneyGroupTopNames(void)
  {
   return(SReportMoneyGroup::FillTopNames(m_top,m_names,m_currency));
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CFtdTotalSection::PrepareGraphs(IMTReportAPI &api,LPCWSTR section_type,const uint32_t y,const bool amount_only)
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
   if(!amount_only)
     {
      name.Append(section_type);
      if((res=PrepareGraph(api,data,m_currency.Append(name).Str(),2,0,CHART_HEIGHT_SECONDARY*y))!=MT_RET_OK)
         return(res);
     }
//--- prepare graph amount by month
   name.Assign(L"FTD Amount by ");
   name.Append(section_type);
   if((res=PrepareGraph(api,data,m_currency.Append(name).Str(),3,amount_only ? y : 1,amount_only ? CHART_HEIGHT_PRIMARY*3 : (CHART_HEIGHT_SECONDARY*y)))!=MT_RET_OK)
      return(res);
//--- prepare graph count by month
   if(!amount_only)
     {
      name.Assign(L"FTD Count by ");
      name.Append(section_type);
      if((res=PrepareGraph(api,data,name.Str(),4,2,CHART_HEIGHT_SECONDARY*y))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare FTD data                                                 |
//+------------------------------------------------------------------+
MTAPIRES CFtdTotalSection::PrepareData(IMTDataset &data) const
  {
//--- create first dataset column
   MTAPIRES res=ReportColumn::ReportColumnsAdd(data,s_columns_ftd,_countof(s_columns_ftd));
   if(res!=MT_RET_OK)
      return(res);
//--- iterate top sections
   for(uint32_t i=0,total=m_top.Total();i<total;i++)
     {
      //--- top section pos
      const uint32_t *pos=m_top.Pos(i);
      if(!pos)
         return(MT_RET_ERROR);
      //--- get section name
      LPCWSTR name=m_names.String(i);
      if(!name)
         return(MT_RET_ERROR);
      //--- fill section name
      FtdSectionRecord row;
      CMTStr::Copy(row.section,name);
      //--- check position
      if(*pos==FtdTop::TOP_POS_OTHER)
        {
         //--- calculate other
         Ftd other={0};
         Ftd::COperationAdd operation(m_currency);
         m_top.CalculateOther(other,m_total,operation);
         //--- fill record section
         FillRecordSection(row,other);
        }
      else
        {
         //--- ftd section
         const Ftd *ftd=m_total.Item(*pos);
         if(!ftd)
            return(MT_RET_ERROR);
         //--- fill record section
         FillRecordSection(row,*ftd);
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
MTAPIRES CFtdTotalSection::PrepareGraph(IMTReportAPI &api,IMTDataset *data,LPCWSTR title,const uint32_t column_id,const uint32_t x,const uint32_t y)
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
   if((res=chart->Left(21*x))!=MT_RET_OK)
      return(res);
//--- set top position
   if((res=chart->Top(y))!=MT_RET_OK)
      return(res);
//--- set width
   if((res=chart->Width(21+x/2))!=MT_RET_OK)
      return(res);
//--- set height
   if((res=chart->Height(CHART_HEIGHT_SECONDARY))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- add data column
   if((res=chart->DataColumnAdd(column_id))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Fill record section                                              |
//+------------------------------------------------------------------+
void CFtdTotalSection::FillRecordSection(FtdSectionRecord &record,const Ftd &ftd) const
  {
//--- fill section
   record.value   =ftd.Medium(m_currency);
   record.amount  =ftd.amount;
   record.count   =ftd.total;
  }
//+------------------------------------------------------------------+
