//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Feeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Ping message                                                     |
//+------------------------------------------------------------------+
class CUniNewsMsgPing : public CUniNewsMsg
  {

public:
   //--- constructor/destructor
                     CUniNewsMsgPing();
   virtual          ~CUniNewsMsgPing();
   //--- initialization
   virtual bool      Initialize(void);
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
inline CUniNewsMsgPing::CUniNewsMsgPing() : CUniNewsMsg()
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
inline CUniNewsMsgPing::~CUniNewsMsgPing(void)
  {
  }
//+------------------------------------------------------------------+
//| Initialization                                                    |
//+------------------------------------------------------------------+
inline bool CUniNewsMsgPing::Initialize(void)
  {
//--- already initialized
   if(m_buffer && m_headtag && m_headtag->size)
      return(true);
//---basic call
   if(!CUniNewsMsg::Initialize())
      return(false);
//--- set message type
   if(m_headtag)
      m_headtag->type=TAG_MSG_PING;
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
