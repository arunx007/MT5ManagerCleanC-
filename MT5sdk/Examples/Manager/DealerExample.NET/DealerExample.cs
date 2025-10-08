//+------------------------------------------------------------------+
//|                        MetaTrader 5 API Manager for .NET Example |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
namespace DealerExample.NET
  {
   using System;
   using System.Windows.Forms;
   //+------------------------------------------------------------------+
   //|                                                                  |
   //+------------------------------------------------------------------+
   static class CDealerExampleApp
     {
      //+------------------------------------------------------------------+
      //|                                                                  |
      //+------------------------------------------------------------------+
      [STAThread]
      static void Main()
        {
         try
           {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new CDealerExampleDlg());
           }
         catch(Exception ex)
           {
            MessageBox.Show(ex.Message,"ERROR",MessageBoxButtons.OK,MessageBoxIcon.Error);
           }
        }
     }
  }
//+------------------------------------------------------------------+
