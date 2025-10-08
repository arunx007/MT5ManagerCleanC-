//+------------------------------------------------------------------+
//|                                                   BalanceExample |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "Manager.h"
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
class CBalanceExampleDlg : public CDialog
  {
private:
   HICON             m_hIcon;
   CManager          m_manager;
   //---
   CEdit             m_Password;
   CEdit             m_Loginname;
   CEdit             m_Server;
   CEdit             m_User;
   CEdit             m_Amount;
   CEdit             m_Comment;
   CButton           m_Logout;
   CButton           m_Login;
   CButton           m_Getuser;
   CButton           m_Deposit;
   CButton           m_Withdraw;
   CButton           m_GetDeals;
   CComboBox         m_Operation;
   CListCtrl         m_List;
   CStatic           m_Userinfo;
   CStatic           m_Balanceinfo;
   CDateTimeCtrl     m_From;
   CDateTimeCtrl     m_To;

public:
                     CBalanceExampleDlg(CWnd *pParent=nullptr);
   enum { IDD=IDD_BALANCE_DIALOG };

protected:
   virtual void      DoDataExchange(CDataExchange* pDX);
   virtual BOOL      OnInitDialog(void);
   afx_msg void      OnPaint(void);
   afx_msg HCURSOR   OnQueryDragIcon(void);
   afx_msg void      OnBnClickedLogin(void);
   afx_msg void      OnBnClickedLogout(void);
   afx_msg void      OnBnClickedGetUser(void);
   afx_msg void      OnBnClickedButtonWithdrawal(void);
   afx_msg void      OnBnClickedButtonDeposit(void);
   afx_msg void      OnEnChangeEditComment(void);
   afx_msg void      OnEnChangeEditAmount(void);
   afx_msg void      OnBnClickedButtonGetdeals(void);
   DECLARE_MESSAGE_MAP()

private:
   void              EnableControls(BOOL enable=TRUE);
   void              UpdateButtons(void);
  };
//+------------------------------------------------------------------+
