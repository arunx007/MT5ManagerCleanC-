//+------------------------------------------------------------------+
//|                                                   BalanceExample |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "resource.h"      // main symbols
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
class CBalanceApp : public CWinApp
  {
public:
                     CBalanceApp();
public:
   virtual BOOL      InitInstance(void);
   DECLARE_MESSAGE_MAP()
  };
extern CBalanceApp theApp;
//+------------------------------------------------------------------+
