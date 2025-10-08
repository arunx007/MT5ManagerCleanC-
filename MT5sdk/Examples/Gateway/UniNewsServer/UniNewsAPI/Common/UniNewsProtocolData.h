//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Feeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Protocol version                                                 |
//+------------------------------------------------------------------+
#define UNINEWS_PROTOCOL_VERSION          1
//+------------------------------------------------------------------+
//| Protocl tags                                                     |
//+------------------------------------------------------------------+
enum EnUniNewsTags : uint16_t
  {
   TAG_UNKNOWN             =0,   // unknown tag
//--- messages
   TAG_MSG_PING            =1,   // ping message
   TAG_MSG_LOGIN           =2,   // login message
   TAG_MSG_LOGIN_RESULT    =3,   // login result message
   TAG_MSG_LOGOUT          =4,   // logout message
   TAG_MSG_SUBSCRIBE       =5,   // subscription message
   TAG_MSG_NEWS            =6,   // news message
//--- login message tags
   TAG_LOGIN_HEADER        =8,   // login message header tag
//--- login result message tags
   TAG_LOGIN_RESULT_HEADER =9,   // login result message header tag
//--- logout message tags
   TAG_LOGOUT_HEADER       =10,  // logout message header tag
//--- subscription message tags
   TAG_SUBSCRIBE_HEADER    =11,  // subscription message header tag
   TAG_SUBSCRIBE_KEYWORDS  =12,  // subscription keywords
//--- news message tags
   TAG_NEWS_HEADER         =13,  // news message header tag
   TAG_NEWS_CATEGORY       =14,  // news category
   TAG_NEWS_SUBJECT        =15,  // news subject
   TAG_NEWS_BODY           =16   // news body
  };
//+------------------------------------------------------------------+
//| Ping message time intervals                                      |
//+------------------------------------------------------------------+
#define PING_PERIOD                       60
#define PING_PERIOD_MAX                   300
//+------------------------------------------------------------------+
