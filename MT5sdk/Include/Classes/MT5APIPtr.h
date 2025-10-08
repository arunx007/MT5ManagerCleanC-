//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Interface unique pointer template class                          |
//+------------------------------------------------------------------+
template<typename T>
class TMTInterfacePtr final
  {
   T                *m_ptr=nullptr; // interface pointer

public:
                     TMTInterfacePtr(void)=default;
                     TMTInterfacePtr(T *pntr) : m_ptr{pntr} {}
                     TMTInterfacePtr(TMTInterfacePtr &&r) : m_ptr{r.Detach()} {}
                    ~TMTInterfacePtr(void);
   //--- access operators
   operator          T *(void)               { return m_ptr; }
   operator    const T *(void) const         { return m_ptr; }
   operator          bool(void) const        { return m_ptr; }
   T*                operator->(void)        { return m_ptr; }
   const T*          operator->(void) const  { return m_ptr; }
   //--- move operator
   TMTInterfacePtr&  operator=(TMTInterfacePtr &&r);
   //--- detach interface pointer
   T*                Detach(void);
  };
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
template<typename T>
TMTInterfacePtr<T>::~TMTInterfacePtr(void)
  {
   if(m_ptr)
      m_ptr->Release();
  }
//+------------------------------------------------------------------+
//| Move operator                                                    |
//+------------------------------------------------------------------+
template<typename T>
TMTInterfacePtr<T>& TMTInterfacePtr<T>::operator=(TMTInterfacePtr &&r)
  {
   T *ptr=m_ptr;
   m_ptr=r.m_ptr;
   r.m_ptr=ptr;
   return *this;
  }
//+------------------------------------------------------------------+
//| Detach interface pointer                                         |
//+------------------------------------------------------------------+
template<typename T>
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
T* TMTInterfacePtr<T>::Detach(void)
  {
   T *ptr=m_ptr;
   m_ptr=nullptr;
   return ptr;
  }
//+------------------------------------------------------------------+
