//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Class of memory block                                            |
//+------------------------------------------------------------------+
class CMTMemPack
  {
private:
   //--- constants
   enum
     {
      REALLOC_STEP  =1024                  // buffer reallocation step
     };

private:
   //--- data
   char             *m_buffer;             // data
   uint32_t          m_buffer_len;         // data length
   uint32_t          m_buffer_max;         // maximum number of bytes in buffer

public:
   //--- constructor/destructor
                     CMTMemPack();
                    ~CMTMemPack(void);
   //--- correct the length in block
   void              Clear()         { m_buffer_len=0; }
   void              Shutdown(void);
   bool              Reallocate(uint32_t growsize);
   //--- add unformatted data
   bool              Add(const void *buf,uint32_t len);
   //--- swap array contents
   void              Swap(CMTMemPack &pack);
   //--- buffer access
   char*             Buffer()        { return(m_buffer);     }
   const uint32_t    Len(void) const     { return(m_buffer_len); }
   void              Len(uint32_t val)   { if(val<=m_buffer_max) m_buffer_len=val; }
   uint32_t          Max(void) const     { return(m_buffer_max); }
  };
//+------------------------------------------------------------------+
//| Constructor with memory pre-allocation                           |
//+------------------------------------------------------------------+
inline CMTMemPack::CMTMemPack(void)
   : m_buffer(NULL),
     m_buffer_len(0),
     m_buffer_max(0)
  {
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
inline CMTMemPack::~CMTMemPack(void)
  {
   Shutdown();
  }
//+------------------------------------------------------------------+
//| Add data to buffer                                               |
//+------------------------------------------------------------------+
inline bool CMTMemPack::Add(const void *buf,uint32_t len)
  {
//--- check
   if(!buf)
      return(false);
//--- check for exceeding buffer size
   if(m_buffer==NULL || (len+m_buffer_len)>m_buffer_max)
      if(!Reallocate(len+m_buffer_len+REALLOC_STEP))
         return(false);
//--- append data to buffer
   memcpy(&m_buffer[m_buffer_len],buf,len);
   m_buffer_len+=len;
//---
   return(true);
  }
//+------------------------------------------------------------------+
//| Swap buffer contents                                             |
//+------------------------------------------------------------------+
inline void CMTMemPack::Swap(CMTMemPack &pack)
  {
   char *buffer;             // buffer
   uint32_t  buffer_len;         // data length
   uint32_t  buffer_max;         // maximum number of bytes in buffer
//--- check
   if(this==&pack)
      return;
//--- swap, remember own buffer
   buffer    =m_buffer;
   buffer_len=m_buffer_len;
   buffer_max=m_buffer_max;
//--- replace buffer with received one
   m_buffer    =pack.m_buffer;
   m_buffer_len=pack.m_buffer_len;
   m_buffer_max=pack.m_buffer_max;
//--- return own buffer
   pack.m_buffer    =buffer;
   pack.m_buffer_len=buffer_len;
   pack.m_buffer_max=buffer_max;
  }
//+------------------------------------------------------------------+
//|  Deallocate memory in MemPack                                    |
//+------------------------------------------------------------------+
inline void CMTMemPack::Shutdown(void)
  {
   if(m_buffer) { delete[] m_buffer; m_buffer=NULL; }
   m_buffer_max=0;
   m_buffer_len=0;
  }
//+------------------------------------------------------------------+
//| Reallocate data block                                            |
//+------------------------------------------------------------------+
inline bool CMTMemPack::Reallocate(uint32_t growsize)
  {
   char *newbuf;
//--- check sizes
   if(growsize<=m_buffer_max) return(true);
//--- try to allocate block of greater size
   if((newbuf=new(std::nothrow) char[growsize])==NULL)
      return(false);
//--- copy values from old buffer
   if(m_buffer)
     {
      if(m_buffer_len>0) memcpy(newbuf,m_buffer,m_buffer_len);
      delete[] m_buffer;
     }
   m_buffer    =newbuf;
   m_buffer_max=growsize;
//---
   return(true);
  }
//+------------------------------------------------------------------+
