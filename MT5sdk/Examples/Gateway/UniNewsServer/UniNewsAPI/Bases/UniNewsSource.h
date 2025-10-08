//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Server |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Default parameters                                               |
//+------------------------------------------------------------------+
#define DEFAULT_NEWS_DIRECTORY   L"news"                          // news directory
#define DEFAULT_NEWS_CATEGORY    L"Universal News Server"         // news directory
//+------------------------------------------------------------------+
//| Preliminary declarations                                         |
//+------------------------------------------------------------------+
class CUniNewsServerPort;
class CUniNewsContext;
//+------------------------------------------------------------------+
//| News source                                                      |
//+------------------------------------------------------------------+
class CUniNewsSource
  {

private:
   //---  contants
   enum EnConstants
     {
      NEWS_PERIOD    =2          // news send period, sec
     };
   //--- news language info
   struct NewsLangInfo
     {
      uint32_t          lang_id;
      wchar_t           lang_name[64];
     };
   //--- filenames array
   typedef TMTArray<wchar_t[MAX_PATH]> FileNamesArray;
   //--- list of languages
   static NewsLangInfo s_languages[];

private:
   //--- last time news sent
   int64_t           m_last_news_send;
   //--- buffers
   CMTMemPack        m_buffer;
   CMTMemPack        m_buffer_tmp;
   CMTMemPack        m_buffer_img;
   //--- news
   CUniNewsMsgNews*  m_news[32];
   CMTSync           m_news_sync;
   uint32_t          m_news_index;
   uint32_t          m_news_sent;
   //--- mime converter
   CMIMEConverter    m_mime_converter;
   //--- files
   FileNamesArray    m_files;
   uint32_t          m_file_last;
   wchar_t           m_news_path[MAX_PATH];

public:
   //--- constructor/destructor
                     CUniNewsSource();
                    ~CUniNewsSource(void);
   //--- initialize/shutdown
   bool              Initialize(LPCWSTR news_path);
   void              Shutdown(void);
   //--- process data
   bool              DataProcess(CUniNewsServerPort &server);
   //--- send old news from history to specified context
   bool              SendNewsHistory(CUniNewsContext &context);

private:
   //--- clear news
   void              ClearNews(void);
   //--- send news
   bool              SendNews(CUniNewsServerPort &server);
   //--- fill news description
   bool              FillNewsDescription(LPCWSTR filename,CUniNewsMsgNews *news);
   //--- read news from file
   bool              FillNewsBody(LPCWSTR filename,CUniNewsMsgNews *news);
   //--- load images from directory
   bool              LoadImages(LPCWSTR path,CMIMEConverter::MIMEImageArray &images);
   //--- get language locale id
   uint32_t          GetLangID(LPCWSTR language);
   //--- scan directory for files
   bool              ScanFiles(LPCWSTR path,FileNamesArray& files);
   //--- extract from resources
   bool              ExtractNews(void);
   bool              ExtractFile(uint32_t id,LPCWSTR filepath);
  };
//+------------------------------------------------------------------+
