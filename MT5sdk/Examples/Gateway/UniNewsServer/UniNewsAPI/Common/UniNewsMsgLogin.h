//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Feeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Login message header structure                                   |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct UniNewsLoginHeader
  {
   uint32_t          version;          // client protocol version
   wchar_t           login[64];        // user login
   wchar_t           password_hash[65];// hash of salted password
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Password container for hash calculation                          |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct UniNewsPasswordContainer
  {
   wchar_t           login[64];
   BYTE              salt[32];
   wchar_t           password[64];
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Login message                                                    |
//+------------------------------------------------------------------+
class CUniNewsMsgLogin : public CUniNewsMsg
  {

private:
   //--- login message header
   UniNewsLoginHeader *m_header;

public:
   //--- construcor/destructor
                     CUniNewsMsgLogin();
   virtual          ~CUniNewsMsgLogin();
   //--- initialization
   virtual bool      Initialize(void);
   //--- get values
   uint32_t          GetVersion(void);
   LPCWSTR           GetLogin();
   LPCWSTR           GetPasswordHash();
   //--- set values
   bool              SetVersion(const uint32_t version);
   bool              SetLogin(LPCWSTR login);
   bool              SetPasswordHash(LPCWSTR password_hash);

protected:
   //--- parse message
   virtual void      ParseClear(void);
   virtual bool      ParseTag(TagHeader *tag);

private:
   //--- check message header
   bool              CheckHeader(void);
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
inline CUniNewsMsgLogin::CUniNewsMsgLogin(void)
 : CUniNewsMsg(),
   m_header(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
inline CUniNewsMsgLogin::~CUniNewsMsgLogin(void)
  {
  }
//+------------------------------------------------------------------+
//| Get protocol version                                             |
//+------------------------------------------------------------------+
inline uint32_t CUniNewsMsgLogin::GetVersion(void)
  {
//--- parse buffer, check header
   if(!ParseBuffer() || !m_header)
      return(0);
//--- return message version
   return(m_header->version);
  }
//+------------------------------------------------------------------+
//| Get login                                                        |
//+------------------------------------------------------------------+
inline LPCWSTR CUniNewsMsgLogin::GetLogin(void)
  {
//--- parse buffer, check header
   if(!ParseBuffer() || !m_header)
      return(NULL);
//--- return login
   return(m_header->login);
  }
//+------------------------------------------------------------------+
//| Get password                                                     |
//+------------------------------------------------------------------+
inline LPCWSTR CUniNewsMsgLogin::GetPasswordHash(void)
  {
//--- parse buffer, check header
   if(!ParseBuffer() || !m_header)
      return(NULL);
//--- return password
   return(m_header->password_hash);
  }
//+------------------------------------------------------------------+
//| Set protocol version                                             |
//+------------------------------------------------------------------+
inline bool CUniNewsMsgLogin::SetVersion(const uint32_t version)
  {
//--- check header
   if(!CheckHeader())
      return(false);
//--- set protocol version
   m_header->version=version;
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Set login                                                        |
//+------------------------------------------------------------------+
inline bool CUniNewsMsgLogin::SetLogin(LPCWSTR login)
  {
//--- check
   if(!login || !CheckHeader())
      return(false);
//--- copy login
   wcsncpy_s(m_header->login,login,_TRUNCATE);
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Set password                                                     |
//+------------------------------------------------------------------+
inline bool CUniNewsMsgLogin::SetPasswordHash(LPCWSTR password_hash)
  {
//--- check
   if(!password_hash || !CheckHeader())
      return(false);
//--- copy password
   wcsncpy_s(m_header->password_hash,password_hash,_TRUNCATE);
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
inline bool CUniNewsMsgLogin::Initialize(void)
  {
//--- already initialized
   if(m_buffer && m_headtag && m_headtag->size)
      return(true);
//--- basic call
   if(!CUniNewsMsg::Initialize())
      return(false);
//--- set message type
   if(m_headtag)
      m_headtag->type=TAG_MSG_LOGIN;
//--- login header
   UniNewsLoginHeader header={0};
//--- add header tag
   if(!TagAdd(TAG_LOGIN_HEADER,(char*)&header,sizeof(header)))
      return(false);
//--- if there is no header in message, failed
   if(!m_header)
      return(false);
//--- fill header with default values
   m_header->version=UNINEWS_PROTOCOL_VERSION;
   m_header->login[0]=L'\0';
   m_header->password_hash[0]=L'\0';
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Clear message parsing data                            |
//+------------------------------------------------------------------+
inline void CUniNewsMsgLogin::ParseClear(void)
  {
//--- basic call
   CUniNewsMsg::ParseClear();
//--- reset pointers
   m_header=NULL;
  }
//+------------------------------------------------------------------+
//| Parse message tag                                                |
//+------------------------------------------------------------------+
inline bool CUniNewsMsgLogin::ParseTag(TagHeader *tag)
  {
//--- check tag
   if(!tag || tag->size<sizeof(TagHeader))
      return(false);
//--- tag data is located behind tag header
   char *data     =(char*)tag+sizeof(TagHeader);
   uint32_t  data_size=tag->size-sizeof(TagHeader);
//--- check tag type
   switch(tag->type)
     {
      //--- login header      
      case TAG_LOGIN_HEADER:
         if(data_size==sizeof(UniNewsLoginHeader))
            m_header=(UniNewsLoginHeader*)data;
         break;
     }
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Check message header                                             |
//+------------------------------------------------------------------+
inline bool CUniNewsMsgLogin::CheckHeader(void)
  {
//--- parse message, check header
   if(ParseBuffer() && m_header)
      return(true);
//--- initialize message
   return(Initialize());
  }
//+------------------------------------------------------------------+
