//+------------------------------------------------------------------+
//|                                          MetaTrader 5 TextFeeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
namespace TextFeeder.NET
  {
   using MetaQuotes.MT5CommonAPI;
   using System;
   using System.Runtime.InteropServices;
   //+------------------------------------------------------------------+
   //| Text feeder data source .NET example                             |
   //+------------------------------------------------------------------+
   static class CTextFeeder
     {
      //+------------------------------------------------------------------+
      //| Copyright                                                        |
      //+------------------------------------------------------------------+
      public const string Copyright         ="Copyright 2000-2025, MetaQuotes Ltd.";
      //+------------------------------------------------------------------+
      //| Version / build / date of the program                            |
      //+------------------------------------------------------------------+
      public const int    ProgramVersion    =500;
      public const int    ProgramBuild      =101;
      public const string ProgramBuildDate  ="5 Apr 2017";
      public const string ProgramName       ="MetaTrader 5 Text Feeder .NET";
      public const string ProgramDescription="This datafeed allows receiving quotes and news from text files.";
      //+------------------------------------------------------------------+
      //| Common constants                                                 |
      //+------------------------------------------------------------------+
      public const int    STACK_SIZE_COMMON =262144; // size of thread stack
      //+------------------------------------------------------------------+
      //| Starting banner                                                  |
      //+------------------------------------------------------------------+
      static void Banner()
        {
         //--- show the starting banner
         Console.WriteLine("{0} {1} bit {2}.{3}  build {4} ({5})\n" +
                           "Copyright 2000-2025, MetaQuotes Ltd.\n",
                           ProgramName,(IntPtr.Size*8),ProgramVersion/100,
                           ProgramVersion%100,ProgramBuild,ProgramBuildDate);
        }
      //+------------------------------------------------------------------+
      //| Entry point                                                      |
      //+------------------------------------------------------------------+
      [STAThread]
      static int Main(string[] args)
        {
         try
           {
            //--- show the banner
            Banner();
            //---
            using(CMTDatafeedApp datafeed=new CMTDatafeedApp())
              {
               //--- initialize the datafeed
               if(!datafeed.Initialize(args))
                  return(-1);
               //--- start the work of the datafeed
               datafeed.Run();
              }
           }
         catch(Exception ex)
           {
            ExtLogger.Out(EnMTLogCode.MTLogErr, "TextFeeder error [{0}]", ex.Message);
            //--- log may be inaccessible
            Console.WriteLine("TextFeeder error [{0}]",ex.Message);
           }
         //--- exit
         return(0);
        }
     }
  }
//+------------------------------------------------------------------+
