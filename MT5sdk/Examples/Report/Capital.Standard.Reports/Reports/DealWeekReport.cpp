//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DealWeekReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CDealWeekReport::s_info=
  {
   100,                                               // report version
   MTReportAPIVersion,                                // report API version
   MTReportInfo::IE_VERSION_9,                        // minimal IE version
   L"Deals Weekly",                                   // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.", // copyright
   L"MetaTrader 5 Report API plug-in",                // description
   MTReportInfo::SNAPSHOT_NONE,                       // snapshot modes
   MTReportInfo::TYPE_DASHBOARD,                      // report types
   L"Trades",                                         // report category
                                                      // request parameters
     {{ MTReportParam::TYPE_GROUPS,    PARAMETER_GROUP_FILTER, DEFAULT_GROUP_FILTER },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_FROM                             },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_TO                               },
     },3,                                          // request parameters total
     {                                             // config parameters
      { MTReportParam::TYPE_STRING,    PARAMETER_CURRENCY,      DEFAULT_CURRENCY    },
      { MTReportParam::TYPE_INT,       PARAMETER_TOP_COUNT,     DEFAULT_TOP_COUNT   },
     },2                                              // config parameters total
  };
//+------------------------------------------------------------------+
//| Column description                                               |
//+------------------------------------------------------------------+
ReportColumn CDealWeekReport::s_columns[12]=
//--- id,              name,                          type,                           width, width_max, offset,                                size,digits_column,flags
  {{ COLUMN_DATE      ,L"Time",                       IMTDatasetColumn::TYPE_DATETIME,20,0,                (uint32_t)offsetof(Row,date)                 ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_VOLUME    ,L"Volume",                     IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,volume)            ,0,0,0 },
   { COLUMN_IN_VOLUME ,L"Profit Deals Volume",        IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,in_volume)         ,0,0,0 },
   { COLUMN_OUT_VOLUME,L"Loss Deals Volume",          IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,out_volume)        ,0,0,0 },
   { COLUMN_IN_AMOUNT ,L"Profit Amount",              IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,in_amount)         ,0,0,0 },
   { COLUMN_OUT_AMOUNT,L"Loss Amount",                IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,out_amount)        ,0,0,0 },
   { COLUMN_IN_COUNT  ,L"Profit Deals Count",         IMTDatasetColumn::TYPE_UINT64  ,10,0,Row::Sections()+(uint32_t)offsetof(Record,in_count)          ,0,0,0 },
   { COLUMN_OUT_COUNT ,L"Loss Deals Count",           IMTDatasetColumn::TYPE_UINT64  ,10,0,Row::Sections()+(uint32_t)offsetof(Record,out_count)         ,0,0,0 },
   { COLUMN_IN_MEDIUM ,L"Profit Deals Volume Average",IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,in_medium)         ,0,0,0 },
   { COLUMN_OUT_MEDIUM,L"Loss Deals Volume Average",  IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,out_medium)        ,0,0,0 },
   { COLUMN_IN_OUT    ,L"Profit-Loss",                IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,in_out_delta)      ,0,0,0 },
   { COLUMN_IN_OUT_ACC,L"Profit-Loss Accumulated",    IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,in_out_delta_accum),0,0,0 },
  };
//+------------------------------------------------------------------+
//| Column total description                                         |
//+------------------------------------------------------------------+
ReportColumn CDealWeekReport::s_columns_total[4]=
//--- id,              name,                          type,                           width, width_max, offset,          size,                         digits_column,flags
  {{ COLUMN_GROUP     ,L"Group",                      IMTDatasetColumn::TYPE_STRING  ,20,0,offsetof(RowTotal,group)     ,MtFieldSize(RowTotal,group),0,IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_VOLUME    ,L"Volume",                     IMTDatasetColumn::TYPE_MONEY   ,20,0,offsetof(RowTotal,volume)    ,0,0,0 },
   { COLUMN_IN_VOLUME ,L"Profit Deals Volume",        IMTDatasetColumn::TYPE_MONEY   ,20,0,offsetof(RowTotal,in_volume) ,0,0,0 },
   { COLUMN_OUT_VOLUME,L"Loss Deals Volume",          IMTDatasetColumn::TYPE_MONEY   ,20,0,offsetof(RowTotal,out_volume),0,0,0 },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDealWeekReport::CDealWeekReport(void) :
   CCurrencyReport(s_info),m_interval(0,UINT_MAX),m_users(NULL),m_deals(NULL),m_table(true),m_total(true)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDealWeekReport::~CDealWeekReport(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CDealWeekReport::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Factory method                                                   |
//+------------------------------------------------------------------+
IMTReportContext* CDealWeekReport::Create(void)
  {
   return(new (std::nothrow) CDealWeekReport());
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CDealWeekReport::Clear(void)
  {
//--- release user cache
   if(m_users)
     {
      m_users->Release();
      m_users=NULL;
     }
//--- release balances
   if(m_deals)
     {
      m_deals->Release();
      m_deals=NULL;
     }
//--- clear interval
   m_interval.Clear();
//--- clear containers
   m_table.Clear();
   m_total.Clear();
   m_top.Clear();
   m_top_total.Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekReport::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- initalize interval
   MTAPIRES res=m_interval.Initialize(m_api);
   if(res!=MT_RET_OK)
      return(res);
//--- user group cache
   m_users=CUserGroupCache::Create(*m_api,m_params,m_currency,res);
   if(!m_users)
      return(res);
//--- deals week aggregates
   m_deals=CDealWeekCache::Create(*m_api,*m_users,m_params,m_currency,res);
   if(!m_deals)
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
MTAPIRES CDealWeekReport::Calculate(void)
  {
//--- checks
   if(!m_api || !m_deals)
      return(MT_RET_ERR_PARAMS);
//--- update deals week aggregates
   MTAPIRES res=m_deals->Update(m_interval.From(),m_interval.To());
   if(res!=MT_RET_OK)
      return(LogError(res,L"Update deals week aggregates"));
//--- initialize group filter
   if((res=m_groups.Initialize(*m_api,*m_deals))!=MT_RET_OK)
      return(LogError(res,L"Initialize group filter"));
//--- calculate deals
   if((res=CalculateDeals())!=MT_RET_OK)
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
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Calculate deals                                                  |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekReport::CalculateDeals(void)
  {
//--- checks
   if(!m_api || !m_deals)
      return(MT_RET_ERR_PARAMS);
//--- read all deals weeks
   return(m_deals->ReadAll(*this,&CDealWeekReport::DealRead));
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekReport::DealRead(const uint64_t key,const CDealWeekCache::DealWeek &week)
  {
//--- check deal week
   const CDealWeekCache::DealKey &deal=CDealWeekCache::Key(key);
   if(!m_interval.WeekContains(deal.week))
      return(MT_RET_OK);
//--- match group with filter
   if(!m_groups.GroupMatch(deal.group))
      return(MT_RET_OK);
//--- currency conversion
   double rate=0.0;
   const bool convert=m_currency.NeedConversion(deal.currency);
   if(convert)
     {
      //--- get currency conversion rate
      MTAPIRES res=m_currency.CurrencyRate(rate,deal.currency);
      if(res!=MT_RET_OK)
         return(res);
     }
//--- call deal week read handler
   return(DealWeekRead(deal.week*SECONDS_IN_WEEK,deal,week,convert ? &rate : nullptr));
  }
//+------------------------------------------------------------------+
//| deal week read handler                                           |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekReport::DealWeekRead(const uint64_t time,const CDealWeekCache::DealKey &key,const CDealWeekCache::DealWeek &week,const double *rate)
  {
//--- get record by deal time
   Section *section=RecordByTimeAndSection(time,key.group);
   if(!section)
      return(MT_RET_ERR_MEM);
//--- add deal week to section
   DealAdd(*section,time,week,rate);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| record by time and section                                       |
//+------------------------------------------------------------------+
CDealWeekReport::Section* CDealWeekReport::RecordByTimeAndSection(const int64_t ctm,const uint32_t section)
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
MTAPIRES CDealWeekReport::PrepareData(IMTDataset &data)
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
MTAPIRES CDealWeekReport::PrepareDataTotal(IMTDataset &data)
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
   if((res=FillTopNames(CDealWeekCache::DEAL_KEY_FIELD_GROUP,m_top_total,names))!=MT_RET_OK)
      return(res);
//--- iterate sections
   for(uint32_t i=0,total=m_top_total.Total();i<total;i++)
     {
      //--- init data row
      RowTotal row={};
      CMTStr::Copy(row.group,names.String(i));
      //--- item position in vector
      const uint32_t *pos=m_top_total.Pos(i);
      if(pos)
        {
         //--- fill data total row
         const Section *section=m_total.Item(*pos);
         if(section && !FillRowTotal(row,*section))
            continue;
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
MTAPIRES CDealWeekReport::PrepareDataColumns(IMTDataset &data)
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
   if((res=FillTopNames(CDealWeekCache::DEAL_KEY_FIELD_GROUP,m_top,names))!=MT_RET_OK)
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
MTAPIRES CDealWeekReport::FillTopNames(const uint32_t dictionary_id,const CReportTopBase &top,CReportDictionary &names)
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
void CDealWeekReport::FillRow(Row &row,const SectionVector &sections,DoubleVector &accumulated) const
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
//| Add deal week to report section                                  |
//+------------------------------------------------------------------+
void CDealWeekReport::DealAdd(Section &section,const uint64_t time,const CDealWeekCache::DealWeek &week,const double *rate)
  {
//--- profit deals
   if(week.profit.count)
      section.in.DealProfitAdd(week.profit.count,
         rate ? m_currency.Convert(week.profit.volume,*rate) : week.profit.volume,
         rate ? m_currency.Convert(week.profit.value,*rate) : week.profit.value,m_currency);
//--- loss deals
   if(week.loss.count)
      section.out.DealProfitAdd(week.loss.count,
         rate ? m_currency.Convert(week.loss.volume,*rate) : week.loss.volume,
         rate ? m_currency.Convert(week.loss.value,*rate) : week.loss.value,m_currency);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekReport::PrepareGraphs(void)
  {
//--- double check
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- prepare total graphs
   MTAPIRES res=PrepareGraphsTotal();
   if(res!=MT_RET_OK)
      return(res);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- prepare data
   if((res=PrepareData(*data))!=MT_RET_OK)
      return(res);
//--- prepare graphs
   for(uint32_t i=1;i<_countof(s_columns);i++)
      if((res=PrepareGraph(data,s_columns[i],
            s_columns[i].id==COLUMN_IN_OUT ? IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE : IMTReportDashboardWidget::WIDGET_TYPE_CHART_SPLINE))!=MT_RET_OK)
         return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekReport::PrepareGraphsTotal(void)
  {
//--- double check
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- prepare total data
   MTAPIRES res=PrepareDataTotal(*data);
   if(res==MT_RET_OK_NONE)
      return(MT_RET_OK);
   if(res!=MT_RET_OK)
      return(res);
//--- prepare graphs
   for(uint32_t i=1;i<std::min(2U,(uint32_t)_countof(s_columns_total));i++)
      if((res=PrepareGraph(data,s_columns_total[i],IMTReportDashboardWidget::WIDGET_TYPE_CHART_PIE))!=MT_RET_OK)
         return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Graph                                                    |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekReport::PrepareGraph(IMTDataset *data,const ReportColumn &column,IMTReportDashboardWidget::EnWidgetType type)
  {
//--- checks
   if(!m_api || !data)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=m_api->DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- widget height
   MTAPIRES res=chart->Height(CHART_HEIGHT_PRIMARY);
   if(res!=MT_RET_OK)
      return(res);
//--- hide zero values
   if((type==IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE || type==IMTReportDashboardWidget::WIDGET_TYPE_CHART_SPLINE) &&
         column.id!=COLUMN_IN_OUT && column.id!=COLUMN_IN_OUT_ACC)
      if((res=chart->Flags(chart->Flags()|IMTReportDashboardWidget::WIDGET_FLAG_HIDE_ZEROES))!=MT_RET_OK)
         return(res);
//--- chart type
   if((res=chart->Type(type))!=MT_RET_OK)
      return(res);
//--- stacked area chart
   if(type==IMTReportDashboardWidget::WIDGET_TYPE_CHART_AREA)
      if((res=chart->ChartStackType(IMTReportDashboardWidget::CHART_STACK_ACCUMULATION))!=MT_RET_OK)
         return(res);
//--- append currency to column name
   CMTStr256 str(column.name);
   if(column.type==IMTDatasetColumn::TYPE_MONEY)
      m_currency.Append(str);
//--- set title
   if((res=chart->Title(str.Str()))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- add data column
   return(ChartColumnsAdd(*chart,column));
  }
//+------------------------------------------------------------------+
//| chart columns add                                                |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekReport::ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const
  {
//--- add data columns for top
   MTAPIRES res=MT_RET_OK;
   for(uint32_t i=0,total=m_top.Total();i<total && res==MT_RET_OK;i++)
      res=chart.DataColumnAdd(column.id+i*(_countof(s_columns)-1));
   return(res);
  }
//+------------------------------------------------------------------+
//| Fill record                                                      |
//+------------------------------------------------------------------+
void CDealWeekReport::FillRecord(Record &record,const Section &section,double *in_out_delta_accum) const
  {
//--- fill record by section
   record.volume      =section.in.amount+section.out.amount;
   record.in_volume   =section.in.amount;
   record.out_volume  =section.out.amount;
   record.in_amount   =section.in.profit;
   record.in_count    =section.in.total;
   record.in_medium   =section.in.Medium(m_currency);
   record.out_amount  =-section.out.profit;
   record.out_count   =section.out.total;
   record.out_medium  =section.out.Medium(m_currency);
   record.in_out_delta=section.InOutProfitDelta(in_out_delta_accum);
   record.in_out_delta_accum=in_out_delta_accum ? *in_out_delta_accum : NAN;
  }
//+------------------------------------------------------------------+
//| Fill row total                                                   |
//+------------------------------------------------------------------+
bool CDealWeekReport::FillRowTotal(RowTotal &row,const Section &section) const
  {
//--- check for emptiness
   if(!section.in.total && !section.out.total)
      return(false);
//--- fill record by section
   row.volume     =section.in.amount+section.out.amount;
   row.in_volume  =section.in.amount;
   row.out_volume =section.out.amount;
   return(true);
  }
//+------------------------------------------------------------------+
