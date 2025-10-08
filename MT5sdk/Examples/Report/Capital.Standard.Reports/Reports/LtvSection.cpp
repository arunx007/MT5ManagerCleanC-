//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "LtvSection.h"
//+------------------------------------------------------------------+
//| Column description LTV                                           |
//+------------------------------------------------------------------+
ReportColumn CLtvSection::s_columns_ltv[]=
  {
   //--- id,name,  type,                           width, width_max,offset,                                             size,digits_column,flags
     { 1, L"Month"  ,IMTDatasetColumn::TYPE_STRING  ,20,0, offsetof(LtvRecord,month)                                     ,MtFieldSize(LtvRecord,month),0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2, L"Value"  ,IMTDatasetColumn::TYPE_MONEY   ,20,0, offsetof(LtvRecord,sections)+offsetof(LtvSectionRecord,value) ,0,0,0 },
   { 3, L"Amount" ,IMTDatasetColumn::TYPE_MONEY   ,20,0, offsetof(LtvRecord,sections)+offsetof(LtvSectionRecord,amount),0,0,0 },
   { 4, L"Count"  ,IMTDatasetColumn::TYPE_UINT64  ,20,0, offsetof(LtvRecord,sections)+offsetof(LtvSectionRecord,count) ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CLtvSection::CLtvSection(CCurrencyConverter &currency) :
   m_currency(currency),m_ltv(true),m_total(true)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CLtvSection::~CLtvSection(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CLtvSection::Clear(void)
  {
//--- clear objects
   m_ltv.Clear();
   m_total.Clear();
   m_top.Clear();
   m_names.Clear();
  }
//+------------------------------------------------------------------+
//| Initialize                                                       |
//+------------------------------------------------------------------+
MTAPIRES CLtvSection::Initialize(IMTReportAPI &api)
  {
//--- report period
   const int64_t period=api.TimeGeneration()-api.ParamFrom();
   if(period<0)
      return(MT_RET_ERR_PARAMS);
//--- initialize ltv matrix
   const uint32_t total=(uint32_t)(period/SECONDS_IN_MONTH)+1;
   if(!m_ltv.Initialize(total,0x100))
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Add deposit                                                      |
//+------------------------------------------------------------------+
MTAPIRES CLtvSection::DepositAdd(const int64_t registration,const int64_t ctm,const uint32_t section,const double value,const uint32_t count)
  {
//--- get record by deal time relative user registration
   Ltv *record=LtvByTimeAndSection(registration,ctm,section);
   if(!record)
      return(MT_RET_ERR_MEM);
//--- calculate deposit
   record->amount=m_currency.MoneyAdd(record->amount,value);
   record->count+=count;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Ltv by time and section                                          |
//+------------------------------------------------------------------+
CLtvSection::Ltv* CLtvSection::LtvByTimeAndSection(const int64_t registration,const int64_t ctm,const uint32_t section)
  {
//--- calculate month index
   uint32_t month=0;
   if(ctm>registration)
      month=(uint32_t)((ctm-registration)/SECONDS_IN_MONTH);
//--- return matrix item
   return(m_ltv.Item(month,section));
  }
//+------------------------------------------------------------------+
//| Calculate total data                                             |
//+------------------------------------------------------------------+
MTAPIRES CLtvSection::CalculateTotal(CReportParameter &params)
  {
//--- calulate total vector
   CLtvOperationAdd operation(m_currency);
   if(!m_ltv.OperationColumns(m_total,operation))
      return(MT_RET_ERR_MEM);
//--- calulate top
   return(m_top.InitializeOther(m_total,SortLtvAmountDesc,operation,params));
  }
//+------------------------------------------------------------------+
//| Calculate period sections                                        |
//+------------------------------------------------------------------+
MTAPIRES CLtvSection::CalculatePeriod(const CReportInterval &interval)
  {
//--- calulate total vector
   CLtvOperationAdd operation(m_currency);
   if(!m_ltv.OperationColumns(m_total,operation))
      return(MT_RET_ERR_MEM);
//--- calulate fake top
   MTAPIRES res=m_top.InitializeNoOrder(m_total);
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
MTAPIRES CLtvSection::FillPeriodTopNames(const CReportInterval &interval,const CReportTopBase &top,CReportDictionary &names)
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
MTAPIRES CLtvSection::ReadTopNames(const CReportCache &cache,const uint32_t dictionary_id)
  {
//--- read top sections names from cache
   return(cache.ReadTopNames(dictionary_id,m_top,m_names));
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CLtvSection::PrepareGraphs(IMTReportAPI &api,LPCWSTR section_type)
  {
//--- check parameter
   if(!section_type)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=api.DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- prepare LTV data
   MTAPIRES res=PrepareData(*data,api.TimeGeneration());
   if(res!=MT_RET_OK)
      return(res);
//--- prepare graph LTV
   CMTStr128 name(L"LTV by ");
   name.Append(section_type);
   if((res=PrepareGraph(api,data,m_currency.Append(name).Str(),IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE,2))!=MT_RET_OK)
      return(res);
//--- prepare graph amount by month
   name.Assign(L"Deposits Amount by ");
   name.Append(section_type);
   if((res=PrepareGraph(api,data,m_currency.Append(name).Str(),IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE,3))!=MT_RET_OK)
      return(res);
//--- prepare graph count by month
   name.Assign(L"Deposits Count by ");
   name.Append(section_type);
   if((res=PrepareGraph(api,data,name.Str(),IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE,4))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare LTV data                                                 |
//+------------------------------------------------------------------+
MTAPIRES CLtvSection::PrepareData(IMTDataset &data,int64_t limit) const
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
   const uint32_t row_size=sizeof(LtvRecord)+(top_total-1)*sizeof(LtvSectionRecord);
   if(!pack.Reallocate(row_size))
      return(MT_RET_ERR_MEM);
   LtvRecord *row=(LtvRecord *)pack.Buffer();
   if(!row)
      return(MT_RET_ERR_MEM);
//--- initialize accumulated vector
   DoubleVector accumulated(true);
   if(!accumulated.InitializeValue(top_total,NAN))
      return(MT_RET_ERR_MEM);
//--- iterate over each month
   for(uint32_t i=0,total=m_ltv.Width();i<total;i++)
     {
      //--- label
      CMTStr32 month;
      CMTStr::FormatStr(row->month,L"%s month",FormatOrdinalNumber(month,i+1).Str());
      //--- fill data row
      const LtvVector *column=m_ltv.Column(i);
      if(column)
         if(!FillRecord(*row,*column,accumulated,limit))
            return(MT_RET_ERROR);
      //--- write data row
      if((res=data.RowWrite(pack.Buffer(),row_size))!=MT_RET_OK)
         return(res);
      //--- shift limit
      limit-=SECONDS_IN_MONTH;
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare LTV data columns                                         |
//+------------------------------------------------------------------+
MTAPIRES CLtvSection::PrepareDataColumns(IMTDataset &data) const
  {
//--- create first dataset column
   MTAPIRES res=ReportColumn::ReportColumnsAdd(data,s_columns_ltv,1);
   if(res!=MT_RET_OK)
      return(res);
//--- initialize dataset section columns
   ReportColumn columns[_countof(s_columns_ltv)-1];
   memcpy(columns,s_columns_ltv+1,sizeof(columns));
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
         columns[j].offset+=sizeof(LtvSectionRecord);
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Graph                                                    |
//+------------------------------------------------------------------+
MTAPIRES CLtvSection::PrepareGraph(IMTReportAPI &api,IMTDataset *data,LPCWSTR title,const uint32_t type,const uint32_t column_id)
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
//--- set stacking for bars
   if(column_id>2)
      if((res=chart->ChartStackType(IMTReportDashboardWidget::CHART_STACK_ACCUMULATION))!=MT_RET_OK)
         return(res);
//--- set title
   if((res=chart->Title(title))!=MT_RET_OK)
      return(res);
//--- set height
   if((res=chart->Height(column_id>2 ? CHART_HEIGHT_SECONDARY : CHART_HEIGHT_PRIMARY))!=MT_RET_OK)
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
bool CLtvSection::FillRecord(LtvRecord &row,const LtvVector &sections,DoubleVector &accumulated,const int64_t limit) const
  {
//--- iterate accumulated sections
   for(uint32_t i=0,total=m_top.Total();i<total;i++)
      if(const uint32_t *pos=m_top.Pos(i))
        {
         //--- accumulated value
         double *accum=accumulated.Item(i);
         if(!accum)
            return(false);
         //--- check position
         if(*pos==LtvTop::TOP_POS_OTHER)
           {
            //--- calculate other
            Ltv other={0};
            CLtvOperationAdd operation(m_currency);
            m_top.CalculateOther(other,sections,operation);
            //--- fill record section
            FillRecordSection(row.sections[i],&other,*accum);
           }
         else
           {
            //--- ltv by section
            const Ltv empty={0};
            const Ltv *ltv=sections.Item(*pos);
            if(!ltv)
               ltv=&empty;
            //--- fill record section
            FillRecordSection(row.sections[i],ltv,*accum);
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
void CLtvSection::FillRecordSection(LtvSectionRecord &record,const Ltv *ltv,double &accum) const
  {
//--- check section
   if(ltv && !_isnan(ltv->amount))
     {
      //--- calc average
      if(ltv->count)
        {
         const double average=ltv->amount/ltv->count;
         accum=_isnan(accum) ? m_currency.MoneyNormalize(average) : m_currency.MoneyAdd(accum,average);
        }
      //--- fill section
      record.value=accum;
      record.amount=ltv->amount;
      record.count=ltv->count;
     }
   else
     {
      //--- zero section
      record.value=NAN;
      record.amount=0;
      record.count=0;
     }
  }
//+------------------------------------------------------------------+
//| Format ordinal number                                            |
//+------------------------------------------------------------------+
const CMTStr& CLtvSection::FormatOrdinalNumber(CMTStr &str,const uint32_t number)
  {
//--- check number
   if(number>=10 && number<=19)
      str.Format(L"%uth",number);
   else
      switch(number%10)
        {
         case 1:  str.Format(L"%ust",number); break;
         case 2:  str.Format(L"%und",number); break;
         case 3:  str.Format(L"%urd",number); break;
         default: str.Format(L"%uth",number); break;
        }
//--- return string
   return(str);
  }
//+------------------------------------------------------------------+
//| sort Ltv amount descending                                       |
//+------------------------------------------------------------------+
int32_t CLtvSection::SortLtvAmountDesc(const void *left,const void *right)
  {
//--- type conversion
   const Ltv *lft=*(const Ltv* const*)left;
   const Ltv *rgh=*(const Ltv* const*)right;
//--- ltv amount comparsion
   if(lft->amount<rgh->amount)
      return(1);
   if(lft->amount>rgh->amount)
      return(-1);
   return(0);
  }
//+------------------------------------------------------------------+
//| sort Ltv descending                                              |
//+------------------------------------------------------------------+
int32_t CLtvSection::SortLtvDesc(const void *left,const void *right)
  {
//--- type conversion
   const Ltv *lft=*(const Ltv* const*)left;
   const Ltv *rgh=*(const Ltv* const*)right;
//--- ltv calculation
   double l_ltv=lft->count ? lft->amount/lft->count : 0;
   double r_ltv=rgh->count ? rgh->amount/rgh->count : 0;
//--- ltv comparsion
   if(l_ltv<r_ltv)
      return(1);
   if(l_ltv>r_ltv)
      return(-1);
   return(0);
  }
//+------------------------------------------------------------------+
//| LTV structure add operation                                      |
//+------------------------------------------------------------------+
void CLtvSection::CLtvOperationAdd::operator()(Ltv &l,const Ltv &r) const
  {
   l.amount=m_currency.MoneyAdd(l.amount,r.amount);
   l.count+=r.count;
  }
//+------------------------------------------------------------------+
