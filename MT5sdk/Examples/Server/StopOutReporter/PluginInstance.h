//+------------------------------------------------------------------+
//|                                          StopOut Reporter Plugin |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Plugin instance class                                            |
//+------------------------------------------------------------------+
class CPluginInstance : public IMTServerPlugin,IMTConPluginSink
  {
private:
   //--- константы
   enum constants
     {
      STOP_OUT_PERIOD  =60*60*12,     // период устаревания уведомления (12 часов)
      BASE_VERSION     =100,          // версия базы
      BASE_MAX         =65536,        // максимальный размер базы
      SEND_PAUSE       =1000,         // промежуток проверки завершённости посылки
      SEND_PAUSE_TOTAL =120,          // число интервалов проверки 
      STACK_SIZE_COMMON=262144,       // для треда
     };
   //--- данные клиента
   struct StopOutInfo
     {
      uint64_t          login;        // логин
      int64_t           datetime;     // время SO или MC
      double            margin;       // маржа
      double            equity;       // евкити
      double            level;        // уровень маржи
      double            value;        // значение в процентах или в деньгах
      double            limit;        // критическое значение для SO или MC
      bool              mc_flag;      // флаг присутствия MC
      bool              mc_send_flag; // флаг отправки MC
      bool              so_flag;      // флаг присутствия SO
      bool              so_send_flag; // флаг отправки SO
     };
   //--- список данных
   typedef TMTArray<StopOutInfo> StopOutInfoArray;

private:
   HMODULE           m_module;
   IMTServerAPI*     m_api;
   IMTConPlugin*     m_config;
   //--- база стоп-аутов
   StopOutInfoArray  m_stopouts;
   //--- шаблоны
   CMTMemPack        m_templ_mc;       // шаблон для маржин-колла
   CMTMemPack        m_templ_so;       // шаблон для маржин-колла
   //--- потоки
   volatile LONG     m_terminate;
   CMTThread         m_check_thread;
   CMTThread         m_send_thread;    // поток отсылки писем
   CMTProcess        m_send_process;   // процесс посылки писем
   CMTFile           m_send_config;    // файл конфигурации посылки
   //--- параметры
   CMTStr256         m_path_data;
   CMTStr256         m_path_root;
   CMTStr256         m_group_mask;
   int64_t           m_check_period;
   int64_t           m_notify_pause;
   CMTStr256         m_notify_from;
   CMTStr256         m_notify_copy;
   CMTStr256         m_notify_server;
   CMTStr256         m_notify_login;
   CMTStr256         m_notify_password;
   //--- буфер отчёта
   CMTMemPack        m_report;
   //--- вспомогательные интерфейсы
   IMTConGroup*      m_group;
   IMTAccount*       m_account;
   IMTUser*          m_user;

public:
                     CPluginInstance(HMODULE hmodule);
                    ~CPluginInstance(void);
   //--- IMTServerPlugin interface implementation
   virtual void      Release(void);
   virtual MTAPIRES  Start(IMTServerAPI* server);
   virtual MTAPIRES  Stop(void);
   //--- IMTConPluginSink interface implementation
   virtual void      OnPluginUpdate(const IMTConPlugin* plugin);

private:
   MTAPIRES          ParametersRead(void);
   bool              TemplatesRead(LPCWSTR filename,const uint32_t id_res,CMTMemPack& pack);
   //--- база стоп-аутов
   void              BaseRead(void);
   void              BaseSave(void);
   //--- проверка уровня
   void              CheckStopout(void);
   void              CheckStopoutLogin(const IMTConGroup* group,const uint64_t login);
   //--- проверка и отсылка уведомлений
   void              StopOutSend(void);
   bool              StopOutGenerate(CMTMemPack& templ,StopOutInfo& info);
   //--- тред
   static uint32_t __stdcall CheckThreadWrapper(void *param);
   void              CheckThread(void);
   //--- поток отсылки
   static uint32_t __stdcall SendThreadWrapper(LPVOID param);
   void              SendThread(void);
   //--- сортировка
   static int32_t    SortByLogin(const void *left,const void *right);
   static int32_t    SearchByLogin(const void *left,const void *right);
  };
//+------------------------------------------------------------------+