//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "TradeAccounts.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CTradeAccounts::CTradeAccounts(void)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CTradeAccounts::~CTradeAccounts(void)
  {
  }
//+------------------------------------------------------------------+
//| Change balance for account                                       |
//+------------------------------------------------------------------+
void CTradeAccounts::BalanceAdd(uint64_t login, double volume)
  {
   bool found=false;
//--- lock
   m_sync.Lock();
//--- look for specified account
   for(uint32_t i=0;i<m_trade_accounts.Total();i++)
      if(m_trade_accounts[i].login==login)
        {
         m_trade_accounts[i].balance+=volume;
         found=true;
         break;
        }
//--- if account wasn't found, add new one
   if(!found)
     {
      TradeAccount account={0};
      //--- set login
      account.login=login;
      //--- set balance
      account.balance=STARTING_BALANCE+volume;
      //--- add account into array
      m_trade_accounts.Add(&account);
     }
//--- unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Get balance of account                                           |
//+------------------------------------------------------------------+
double CTradeAccounts::BalanceGet(uint64_t login)
  {
   double balance=0.0;
   bool   found  =false;
//--- lock
   m_sync.Lock();
//--- look for specified account
   for(uint32_t i=0;i<m_trade_accounts.Total();i++)
      if(m_trade_accounts[i].login==login)
        {
         balance=m_trade_accounts[i].balance;
         found=true;
         break;
        }
//--- if account wasn't found, use starting balance
   if(!found)
      balance=STARTING_BALANCE;
//--- unlock
   m_sync.Unlock();
//--- return result
   return(balance);
  }
//+------------------------------------------------------------------+
