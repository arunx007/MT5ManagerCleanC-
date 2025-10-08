//+------------------------------------------------------------------+
//|                                                    DealerExample |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
class CDealerExampleApp : public CWinApp
  {
public:
                     CDealerExampleApp();

public:
   virtual BOOL      InitInstance(void);

   DECLARE_MESSAGE_MAP()
  };
extern CDealerExampleApp theApp;
//+------------------------------------------------------------------+
