//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Plugin context helper struct                                     |
//+------------------------------------------------------------------+
struct PluginContext
  {
   //--- params
   CMTStr16          fdm_id;
   CMTStr32          server_id;
   CMTStr256         symbols;
   CMTStr256         groups;
   CMTStrPath        base_directory;
   CMTStr256         sftp_address;
   CMTStr256         sftp_port;
   CMTStr256         sftp_login;
   CMTStr256         sftp_password;
   CMTStrPath        sftp_key_path;
   int64_t           report_time;
   int64_t           send_time;
   //--- server context
   CMTStrPath        server_path;
   int32_t           server_timezone;
   //---
                     PluginContext(void) : report_time(0),send_time(0),server_timezone(0) {}
                    ~PluginContext(void) {}
   //--- clear all data
   inline void       Clear()
     {
      fdm_id.Clear();
      server_id.Clear();
      symbols.Clear();
      groups.Clear();
      base_directory.Clear();
      sftp_address.Clear();
      sftp_port.Clear();
      sftp_login.Clear();
      sftp_password.Clear();
      sftp_key_path.Clear();
      report_time=0;
      send_time=0;
      //---
      server_path.Clear();
      server_timezone=0;
     }
  };
//+------------------------------------------------------------------+
