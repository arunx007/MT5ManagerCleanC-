//+------------------------------------------------------------------+
//|                                                    DealerExample |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "Dealer.h"
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
class CDealerExampleDlg : public CDialog
  {
private:
   CDealer           m_dealer;
   //---
   CEdit             m_Password;
   CEdit             m_Loginname;
   CEdit             m_Server;
   CButton           m_Logout;
   CButton           m_Login;
   CButton           m_Confirm;
   CButton           m_Reject;
   CEdit             m_Request;
   HICON             m_hIcon;
   IMTRequest       *m_request;
   IMTConfirm       *m_confirm;

public:
                     CDealerExampleDlg(CWnd *pParent=nullptr);
                    ~CDealerExampleDlg(void);

   enum { IDD=IDD_DEALER_DIALOG };

protected:
   void              EnableButtons(BOOL enable=TRUE);
   virtual void      DoDataExchange(CDataExchange* pDX);
   DECLARE_MESSAGE_MAP()
   virtual BOOL      OnInitDialog(void);
   afx_msg void      OnPaint(void);
   afx_msg void      OnBnClickedLogin(void);
   afx_msg void      OnBnClickedLogout(void);
   afx_msg HCURSOR   OnQueryDragIcon(void);
   afx_msg void      OnBnClickedConfirm(void);
   afx_msg void      OnBnClickedReject(void);
   afx_msg LRESULT   OnRequestReady(WPARAM wp,LPARAM lp);
  };
//+------------------------------------------------------------------+
