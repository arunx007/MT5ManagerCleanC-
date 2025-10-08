<?php
//+------------------------------------------------------------------+
//|                           MetaTrader 5 Web API Extension Example |
//|                   Copyright 2000-2021, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+
  define('PATH_TO_SCRIPTS', 	'../../Examples/PHP/mt5_api/');
  include PATH_TO_SCRIPTS . "mt5_api.php";
  include "web_pageform.php";
  
  define("IS_WRITE_DEBUG_LOG",     false);                    // Write all in logs
  define("PATH_TO_LOGS",           "./logs");                 // Write all in logs
  define("AGENT",                  "WebApiExtensionExample");
  //--- settings of connection
  define("MT5_CONNECTION_TIMEOUT", 30000);
  
  $page = new WebPageForm();
  $page->CreatePage();
?>