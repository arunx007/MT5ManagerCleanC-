//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Report vector template class                                     |
//+------------------------------------------------------------------+
template<class T>
class TReportVector
  {
public:
   enum constants
     {
      VECTOR_POS_MAX=0xFFFFFF                   // maximum item position
     };

   typedef TMTArray<T> TArray;                  // array type

private:
   const bool        m_size_fixed;              // fixed size flag
   TArray            m_items;                   // items array

public:
   explicit          TReportVector(const bool size_fixed=false) : m_size_fixed(size_fixed) {}
                    ~TReportVector(void) {}
   //--- clear
   void              Clear(void);
   //--- initialization
   bool              Initialize(const uint32_t size);
   bool              InitializeValue(const uint32_t size,const T value);
   bool              InitializeEmpty(const uint32_t size);
   //--- items array
   const TArray&     Items(void) const                { return(m_items); }
   //--- check for object constructed
   bool              Constructed(void) const          { return(m_items.Width()); }
   //--- total items
   uint32_t          Total(void) const                { return(m_items.Total()); }
   //--- item by position
   T*                Item(const uint32_t pos);
   const T*          Item(const uint32_t pos) const       { return(pos<m_items.Total() ? (const T*)m_items.At(pos) : nullptr); }
   //--- position by item
   int32_t           Position(const T *item) const    { return(m_items.Position(item)); }
   //--- vector binary operation
   template<class Func>
   bool              Operation(const TReportVector &r,const Func &func);
  };
//+------------------------------------------------------------------+
//| clear                                                            |
//+------------------------------------------------------------------+
template<class T>
void TReportVector<T>::Clear(void)
  {
   m_items.Clear();
  }
//+------------------------------------------------------------------+
//| initialization with zero                                         |
//+------------------------------------------------------------------+
template<class T>
bool TReportVector<T>::Initialize(const uint32_t size)
  {
//--- initialization with empty values
   if(!InitializeEmpty(size))
      return(false);
//--- zero items
   m_items.Zero();
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| initialization with value                                        |
//+------------------------------------------------------------------+
template<class T>
bool TReportVector<T>::InitializeValue(const uint32_t size,const T value)
  {
//--- initialization with empty values
   if(!InitializeEmpty(size))
      return(false);
//--- initialize items with value
   for(uint32_t i=0;i<m_items.Total();i++)
      m_items[i]=value;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| initialization with empty values                                 |
//+------------------------------------------------------------------+
template<class T>
bool TReportVector<T>::InitializeEmpty(const uint32_t size)
  {
//--- clear
   Clear();
//--- check empty size
   if(!size)
      return(true);
//--- reserve items memory
   if(!m_items.Reserve(size))
      return(false);
//--- check fixed flag size
   if(!m_size_fixed)
      return(true);
//--- resize array
   if(!m_items.Resize(size))
      return(false);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| item by position                                                 |
//+------------------------------------------------------------------+
template<class T>
T* TReportVector<T>::Item(const uint32_t pos)
  {
//--- check existing item
   const uint32_t total=m_items.Total();
   if(pos<total)
      return((T*)m_items.At(pos));
//--- check fixed size flag
   if(m_size_fixed)
      return(nullptr);
//--- check realocation needed
   if(pos>=m_items.Max())
     {
      //--- check position
      if(pos>VECTOR_POS_MAX)
         return(nullptr);
      //--- double size reallocation
      if(!m_items.Reserve(std::max(pos+1,m_items.Max()*2)))
         return(nullptr);
     }
//--- resize array
   if(!m_items.Resize(pos+1))
      return(nullptr);
//--- new items pointer
   T *begin=(T*)m_items.At(total);
   if(!begin)
      return(nullptr);
//--- zero new items
   ZeroMemory((void*)begin,(pos-total+1)*sizeof(T));
//--- return new item pointer
   return((T*)m_items.At(pos));
  }
//+------------------------------------------------------------------+
//| vector binary operation                                          |
//+------------------------------------------------------------------+
template<class T>
template<class Func>
bool TReportVector<T>::Operation(const TReportVector &r,const Func &func)
  {
//--- iterate all items of right vector
   for(uint32_t i=0,total=r.Total();i<total;i++)
      if(const T *r_item=r.Item(i))
        {
         //--- get own item
         T *item=Item(i);
         if(!item)
            return(false);
         //--- operation on items
         func(*item,*r_item);
        }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
