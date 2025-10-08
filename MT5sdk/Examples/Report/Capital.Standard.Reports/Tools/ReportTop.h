//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportVector.h"
#include "ReportParameter.h"
//+------------------------------------------------------------------+
//| Default values                                                   |
//+------------------------------------------------------------------+
#define DEFAULT_TOP_COUNT                 L"10"       // default: top items to show
//+------------------------------------------------------------------+
//| Parameters                                                       |
//+------------------------------------------------------------------+
#define PARAMETER_TOP_COUNT               L"Top Count"
//+------------------------------------------------------------------+
//| Report top base class                                            |
//+------------------------------------------------------------------+
class CReportTopBase
  {
public:
   enum constatnts
     {
      TOP_COUNT_MAX=0x10000,                          // top count max
      TOP_POS_OTHER=UINT_MAX                          // other position
     };

protected:
   MTUIntArray       m_index;                         // top index
   MTUIntArray       m_index_sorted;                  // top index sorted

public:
                     CReportTopBase(void) {}
                    ~CReportTopBase(void) {}
   //--- clear
   void              Clear(void);
   //--- total items
   uint32_t          Total(void) const             { return(m_index.Total()); }
   //--- item position in vector
   const uint32_t*   Pos(const uint32_t pos) const     { return(pos<m_index.Total() ? (const uint32_t*)m_index.At(pos) : nullptr); }
   //--- check other item exists
   bool              OtherExists(void) const       { return(m_index.Total()>m_index_sorted.Total()); }

protected:
   //--- get top count report parameter value
   MTAPIRES          TopCount(CReportParameter &params,uint32_t &top_count,LPCWSTR param_name=PARAMETER_TOP_COUNT,LPCWSTR top_count_default=DEFAULT_TOP_COUNT);
   //--- sort index static method
   static int32_t    SortIndex(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
//| Report top template class                                        |
//+------------------------------------------------------------------+
template<class T>
class TReportTop : public CReportTopBase
  {
public:
   typedef TReportVector<T> TVector;                  // vector type

public:
                     TReportTop(void) {}
                    ~TReportTop(void) {}
   //--- initialization without order
   MTAPIRES          InitializeNoOrder(const TVector &vector);
   //--- initialization
   MTAPIRES          Initialize(const TVector &vector,SMTSearch::SortFunctionPtr sort_function);
   //--- initialization with other calculation
   template<class Func>
   MTAPIRES          InitializeOther(const TVector &vector,SMTSearch::SortFunctionPtr sort_function,const Func &func_other,CReportParameter &params);
   //--- calculate other item
   template<class Func>
   void              CalculateOther(T &other,const TVector &vector,const Func &func) const;
  };
//+------------------------------------------------------------------+
//| initialization without order                                     |
//+------------------------------------------------------------------+
template<class T>
MTAPIRES TReportTop<T>::InitializeNoOrder(const TVector &vector)
  {
//--- clear
   Clear();
//--- vector size
   uint32_t total=vector.Total();
   if(!total)
      return(MT_RET_OK);
//--- reserve top indexes memory
   if(!m_index.Reserve(total))
      return(MT_RET_ERR_MEM);
//--- fill top index array
   for(uint32_t i=0;i<total;i++)
      if(!m_index.Add((uint32_t*)&i))
         return(MT_RET_ERROR);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| initialization                                                   |
//+------------------------------------------------------------------+
template<class T>
MTAPIRES TReportTop<T>::Initialize(const TVector &vector,SMTSearch::SortFunctionPtr sort_function)
  {
//--- check function pointer
   if(!sort_function)
      return(MT_RET_ERR_PARAMS);
//--- clear
   Clear();
//--- vector size
   uint32_t total=vector.Total();
   if(!total)
      return(MT_RET_OK);
//--- reserve top indexes memory
   if(!m_index.Reserve(total))
      return(MT_RET_ERR_MEM);
//--- reserve vector index
   TMTArray<const T*> index;
   if(!index.Reserve(total))
      return(MT_RET_ERR_MEM);
//--- fill vector index
   for(uint32_t i=0;i<total;i++)
      if(const T *item=vector.Item(i))
         if(!index.Add(&item))
            return(MT_RET_ERROR);
//--- sort vector index
   index.Sort(sort_function);
//--- fill top index array
   for(uint32_t i=0;i<total;i++)
     {
      const int32_t pos=vector.Position(index[i]);
      if(pos>=0)
         if(!m_index.Add((uint32_t*)&pos))
            return(MT_RET_ERROR);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| initialization with ohter calculation                            |
//+------------------------------------------------------------------+
template<class T>
template<class Func>
MTAPIRES TReportTop<T>::InitializeOther(const TVector &vector,SMTSearch::SortFunctionPtr sort_function,const Func &func_other,CReportParameter &params)
  {
//--- check function pointer
   if(!sort_function)
      return(MT_RET_ERR_PARAMS);
//--- clear
   Clear();
//--- get top count report parameter value
   uint32_t count=0;
   const MTAPIRES res=TopCount(params,count);
   if(res!=MT_RET_OK)
      return(res);
//--- check for empty
   if(!count)
      return(MT_RET_OK);
//--- reserve top indexes memory
   if(!m_index.Reserve(count+1) || !m_index_sorted.Reserve(count))
      return(MT_RET_ERR_MEM);
//--- vector size
   uint32_t total=vector.Total();
   if(!total)
      return(MT_RET_OK);
//--- reserve vector index
   TMTArray<const T*> index;
   if(!index.Reserve(total+1))
      return(MT_RET_ERR_MEM);
//--- fill vector index
   for(uint32_t i=0;i<total;i++)
      if(const T *item=vector.Item(i))
         if(!index.Add(&item))
            return(MT_RET_ERROR);
//--- sort vector index
   index.Sort(sort_function);
//--- fill top index array
   const T empty={0};
   total=std::min(count,index.Total());
   for(uint32_t i=0;i<total;i++)
      if(const T *item=index[i])
         if(memcmp(item,&empty,sizeof(empty)))
           {
            const int32_t pos=vector.Position(item);
            if(pos>=0)
               if(!m_index.Add((uint32_t*)&pos))
                  return(MT_RET_ERROR);
           }
//--- check empty
   if(!m_index.Total())
      return(MT_RET_OK);
//--- make sorted index
   if(!m_index_sorted.Assign(m_index))
      return(MT_RET_ERROR);
   m_index_sorted.Sort(SortIndex);
//--- calculate other
   T other={0};
   CalculateOther(other,vector,func_other);
//--- check other empty
   if(!memcmp(&other,&empty,sizeof(empty)))
      return(MT_RET_OK);
//--- insert other to index
   const T *ptr_other=&other;
   const T **index_other=index.Insert(&ptr_other,sort_function);
//--- search other in index if no insert
   if(!index_other)
     {
      index_other=index.Search(&ptr_other,sort_function);
      if(index_other)
        {
         //--- insert other to index before same element
         const int32_t pos_other=index.Position(index_other);
         if(pos_other>=0)
            if(!index.Insert(pos_other,&ptr_other,1))
               return(MT_RET_ERROR);
        }
     }

   if(!index_other || !*index_other)
      return(MT_RET_ERROR);
//--- get other postition in index
   const int32_t pos_other=index.Position(index_other);
   if(pos_other<0)
      return(MT_RET_ERROR);
//--- add other index to top index
   const uint32_t other_pos=TOP_POS_OTHER;
   if((uint32_t)pos_other<m_index.Total())
     {
      //--- insert other index to top index
      if(!m_index.Insert((uint32_t)pos_other,&other_pos,1))
         return(MT_RET_ERROR);
      //--- ok
      return(MT_RET_OK);
     }
//--- append other index to top index
   if(!m_index.Add(&other_pos))
      return(MT_RET_ERROR);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| calculate other item                                             |
//+------------------------------------------------------------------+
template<class T>
template<class Func>
void TReportTop<T>::CalculateOther(T &other,const TVector &vector,const Func &func) const
  {
//--- iterate each vector item except top items from index
   for(uint32_t i=0,j=0,i_total=vector.Total(),j_total=m_index_sorted.Total();i<i_total;i++)
      if(j<j_total && i==m_index_sorted[j])
         j++;
      else
         if(const T *item=vector.Item(i))
            func(other,*item);
  }
//+------------------------------------------------------------------+
