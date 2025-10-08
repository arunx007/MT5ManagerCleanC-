<?php
//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                   Copyright 2000-2021, MetaQuotes Software Corp. |
//|                                        http://www.metaquotes.net |
//+------------------------------------------------------------------+
  define("PATH_TO_SCRIPTS", "../mt5_api/");
  include PATH_TO_SCRIPTS . "mt5_api.php";
  //---
  include "mysql.php";
  include "tools.php";
  include "web_pageform.php";
  //---
  define("IS_WRITE_DEBUG_LOG", true);                 // Write all in logs
  define("PATH_TO_LOGS", "c:\\temp\\web_registration\\");                 // Write all in logs
  define("AGENT", "WebRegistration");
  //--- settings of connection
  define("MT5_SERVER_IP", "192.168.9.150");           // MetaTrader: ip MetaTrader 5 server
  define("MT5_SERVER_PORT", 1951);                    // MetaTrader: port MetaTrader 5 server
  define("MT5_SERVER_WEB_LOGIN", 1000);               // MetaTrader: login
  define("MT5_SERVER_WEB_PASSWORD", 'Password1');       // MetaTrader: password
  //---
  define('PAGE_ENCODING', 'utf-8');                   // Web: Page encoding
  //---
  define('MYSQL_SERVER',  'localhost');               // MySQL: Server
  define('MYSQL_LOGIN',   'mysql_login');             // MySQL: Login
  define('MYSQL_PASSWORD','mysql_password');          // MySQL: Password
  define('MYSQL_DB_NAME', 'mysql_db_name');           // MySQL: Database
  //---
  define('SMTP_SERVER',   'mail.mycompany.com');      // SMTP: Server
  define('SMTP_LOGIN',    'login');                   // SMTP: Login
  define('SMTP_PASSWORD', 'password');                // SMTP: Password
  define('SMTP_FROM',     'hello@mycompany.com');     // SMTP: From
  define('SMTP_FROM_NAME','Mr. Hello');               // SMTP: From Name

  //---
  $page = new WebPageForm();
  //---
  $page->CreatePage();
?>