//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Feeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Tag header structure                                             |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct TagHeader
  {
   uint16_t          type;             // tag type
   uint32_t          size;             // tag size
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Base class of message                                            |
//+------------------------------------------------------------------+
class CUniNewsMsg
  {

public:
   //--- constants
   enum EnConstants
     {
      MAX_MSG_SIZE   =32*1024*1024,    // maximum message size
      BUFFER_STEP    =64*1024          // buffer reallocation step
     };

protected:
   TagHeader        *m_headtag;        // message tag
   char             *m_buffer;         // message data buffer
   uint32_t          m_buffer_max;     // length of data buffer
   bool              m_buffer_parsed;  // indication that buffer is parsed

public:
   //--- constructor/destructor
                     CUniNewsMsg();
   virtual          ~CUniNewsMsg();
   //--- initialization/shutdown
   virtual bool      Initialize(void);
   virtual void      Shutdown(void);
   //--- set raw message data
   bool              SetRaw(const char* buffer,const uint32_t buffer_size);
   //--- get message type and size
   uint16_t          GetMsgType();
   uint32_t          GetMsgSize(void);
   //--- get message raw data
   const char*       GetRaw();

protected:
   //--- add new tag
   bool              TagAdd(const uint16_t type,const char *data,const uint32_t size);
   //--- parse message
   virtual void      ParseClear(void);
   virtual bool      ParseTag(TagHeader *tag);
   bool              ParseBuffer(void);

private:
   //--- reallocate buffer with new size
   bool              ReallocateBuffer(const uint32_t new_size);
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
inline CUniNewsMsg::CUniNewsMsg(void)
 : m_headtag(NULL),m_buffer(NULL),m_buffer_max(0),m_buffer_parsed(false)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
inline CUniNewsMsg::~CUniNewsMsg(void)
  {
//--- shutdown message
   Shutdown();
//--- free buffer
   if(m_buffer)
      delete[] m_buffer;
  }
//+------------------------------------------------------------------+
//| Set raw message data                                             |
//+------------------------------------------------------------------+
inline bool CUniNewsMsg::SetRaw(const char* buffer,const uint32_t buffer_size)
  {
//--- check buffer
   if(!buffer || buffer_size<sizeof(TagHeader))
      return(false);
//--- clear older parsing results
   ParseClear();
//--- allocate new-sized buffer
   if(!ReallocateBuffer(buffer_size))
      return(false);
//--- copy data
   memcpy(m_buffer,buffer,buffer_size);
//--- message tag is at the beginning of buffer
   m_headtag=(TagHeader*)m_buffer;
//--- parse buffer
   if(!ParseBuffer())
     {
      ParseClear();
      return(false);
     }
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Get message type                                                 |
//+------------------------------------------------------------------+
inline uint16_t CUniNewsMsg::GetMsgType(void)
  {
//--- check message header
   if(!m_buffer || !m_headtag)
      return(TAG_UNKNOWN);
//--- return message type
   return(m_headtag->type);
  }
//+------------------------------------------------------------------+
//| Get message size                                                 |
//+------------------------------------------------------------------+
inline uint32_t CUniNewsMsg::GetMsgSize(void)
  {
//--- check message header
   if(!m_buffer || !m_headtag)
      return(0);
//--- return message size
   return(m_headtag->size);
  }
//+------------------------------------------------------------------+
//| Get message raw data                                             |
//+------------------------------------------------------------------+
inline const char* CUniNewsMsg::GetRaw(void)
  {
//--- initialization
   if(!Initialize())
      return(NULL);
//--- return buffer
   return(m_buffer);
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
inline bool CUniNewsMsg::Initialize(void)
  {
//--- check if message already initialized
   if(m_buffer && m_headtag && m_headtag->size)
      return(true);
//--- clear older parsing results
   ParseClear();
//--- allocate buffer for message header
   if(!ReallocateBuffer(sizeof(TagHeader)))
      return(false);
//--- message tag is at the beginning of buffer
   m_headtag=(TagHeader*)m_buffer;
//--- message contains only header
   m_headtag->size=sizeof(TagHeader);
//--- message type
   m_headtag->type=TAG_UNKNOWN;
//--- empty message, no need to parse it
   m_buffer_parsed=true;
//--- succesed
   return(true);
  }
//+------------------------------------------------------------------+
//| Shutdown                                                |
//+------------------------------------------------------------------+
inline void CUniNewsMsg::Shutdown(void)
  {
//--- clear older parsing results
   ParseClear();
//--- clear header
   m_headtag=NULL;
  }
//+------------------------------------------------------------------+
//| Add new tag                                                      |
//+------------------------------------------------------------------+
inline bool CUniNewsMsg::TagAdd(const uint16_t type,const char *data,const uint32_t size)
  {
   TagHeader *tag;
   uint32_t   offset,new_size;
//--- check
   if(!data || !size)
      return(false);
//--- initialization
   if(!Initialize() || !m_buffer || !m_headtag)
      return(false);
//--- delete tag data if it already exists
   for(offset=sizeof(TagHeader),tag=NULL;offset<m_headtag->size;offset+=tag->size)
     {
      //--- get tag pointer
      tag=(TagHeader*)(m_buffer+offset);
      //--- search for the right tag
      if(tag->type==type)
        {
         //--- save tag length
         uint32_t tag_size=tag->size;
         //--- shift data
         memmove(m_buffer+offset,m_buffer+offset+tag_size,m_headtag->size-offset-tag_size);
         //--- correct message length
         m_headtag->size-=tag_size;
         //--- clear older parsing results
         ParseClear();
         break;
        }
     }
//--- calculate new message size (old size + header for new data + new data)
   new_size=m_headtag->size+sizeof(TagHeader)+size;
//--- allocate new-sized buffer
   if(!ReallocateBuffer(new_size))
      return(false);
//--- set up new tag
   tag      =(TagHeader*)(m_buffer+m_headtag->size);
   tag->type=type;
   tag->size=size+sizeof(TagHeader);
//--- copy tag data
   memcpy(m_buffer+m_headtag->size+sizeof(TagHeader),data,size);
//--- correct message size
   m_headtag->size=new_size;
//--- if buffer already parsed, parse new tag too
   if(m_buffer_parsed)
      ParseTag(tag);
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Clear message parsing data                            |
//+------------------------------------------------------------------+
inline void CUniNewsMsg::ParseClear(void)
  {
//--- resed flag
   m_buffer_parsed=false;
  }
//+------------------------------------------------------------------+
//| Parse message tag                                            |
//+------------------------------------------------------------------+
inline bool CUniNewsMsg::ParseTag(TagHeader *tag)
  {
//--- check tag
   if(tag->size<sizeof(TagHeader))
      return(false);
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Parse message                                                 |
//+------------------------------------------------------------------+
inline bool CUniNewsMsg::ParseBuffer(void)
  {
   TagHeader *tag;
   uint32_t   offset,check_size=0;
//--- message alerady parsed
   if(m_buffer_parsed)
      return(true);
//--- check buffer
   if(!m_buffer || !m_headtag || m_buffer_max<sizeof(TagHeader) || m_buffer_max<m_headtag->size)
      return(false);
//--- clear older parsing results
   ParseClear();
//--- add message header size to check size
   check_size+=sizeof(TagHeader);
//--- go through all tags
   for(offset=sizeof(TagHeader),tag=NULL;offset<m_headtag->size;offset+=tag->size)
     {
      //--- get tag pointer
      tag=(TagHeader*)(m_buffer+offset);
      //--- calculate message length for control
      check_size+=tag->size;
      //--- check tag size
      if(check_size>m_headtag->size || tag->size<sizeof(TagHeader))
         return(false);
      //--- parse tag
      if(!ParseTag(tag))
         return(false);
     }
//--- check message size
   if(m_headtag->size==check_size)
      m_buffer_parsed=true;
//--- return result
   return(m_buffer_parsed);
  }
//+------------------------------------------------------------------+
//| Reallocate buffer with new size                                  |
//+------------------------------------------------------------------+
inline bool CUniNewsMsg::ReallocateBuffer(const uint32_t new_size)
  {
//--- check news size
   if(new_size>MAX_MSG_SIZE)
      return(false);
//--- allocate news buffer if it is necessary
   if(!m_buffer || new_size>m_buffer_max)
     {
      char *buffer;
      //--- allocate memory
      if((buffer=new(std::nothrow) char[new_size+BUFFER_STEP])==NULL)
         return(false);
      //--- copy old data
      if(m_buffer && m_headtag && m_headtag->size>0)
         memcpy(buffer,m_buffer,m_headtag->size);
      //--- free old buffer
      if(m_buffer)
         delete[] m_buffer;
      //--- set new buffer and it's size
      m_buffer    =buffer;
      m_buffer_max=new_size+BUFFER_STEP;
      //--- set message tag in new buffer
      m_headtag=(TagHeader*)m_buffer;
      //--- clear older parsing results
      ParseClear();
     }
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
