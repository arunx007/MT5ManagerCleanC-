//+------------------------------------------------------------------+
//|                                                   BalanceExample |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "stdafx.h"
//+------------------------------------------------------------------+
//| Manager                                                          |
//+------------------------------------------------------------------+
class CManager
  {
private:
   enum              constants
     {
      MT5_CONNECT_TIMEOUT=30000,       // connect timeout in milliseconds
     };
   IMTManagerAPI    *m_manager;
   CMTManagerAPIFactory m_factory;
   IMTDealArray*     m_deal_array;
   IMTUser*          m_user;
   IMTAccount*       m_account;

public:
                     CManager(void);
                    ~CManager(void);
   //--- initialize, login
   bool              Initialize(void);
   bool              Login(LPCWSTR server,uint64_t login,LPCWSTR password);
   void              Logout(void);
   //--- dealer operation
   bool              DealerBalance(const uint64_t login,const double amount,const uint32_t type,const LPCWSTR comment,bool deposit);
   //--- get info
   bool              GetUserDeal(IMTDealArray*& deals,const uint64_t login,SYSTEMTIME &time_from,SYSTEMTIME &time_to);
   bool              GetUserInfo(uint64_t login,CMTStr &str);
   bool              GetAccountInfo(uint64_t login,CMTStr &str);

private:
   void              Shutdown(void);

  };
//+------------------------------------------------------------------+
