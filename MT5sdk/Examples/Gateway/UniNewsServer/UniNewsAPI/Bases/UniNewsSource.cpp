//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "UniNewsSource.h"
//+------------------------------------------------------------------+
//| Languages info list                                              |
//+------------------------------------------------------------------+
CUniNewsSource::NewsLangInfo CUniNewsSource::s_languages[]=
  {{  LANG_ARABIC,               L"arabic"               },
   {  LANG_BULGARIAN,            L"bulgarian"            },
   {  LANG_CHINESE_SIMPLIFIED,   L"chinese simplified"   },
   {  LANG_CHINESE_TRADITIONAL,  L"chinese traditional"  },
   {  LANG_CZECH,                L"czech"                },
   {  LANG_ENGLISH,              L"english"              },
   {  LANG_FRENCH,               L"french"               },
   {  LANG_GERMAN,               L"german"               },
   {  LANG_GREEK,                L"greek"                },
   {  LANG_HEBREW,               L"hebrew"               },
   {  LANG_HINDI,                L"hindi"                },
   {  LANG_HUNGARIAN,            L"hungarian"            },
   {  LANG_INDONESIAN,           L"indonesian"           },
   {  LANG_ITALIAN,              L"italian"              },
   {  LANG_JAPANESE,             L"japanise"             },
   {  LANG_KOREAN,               L"korean"               },
   {  LANG_MALAY,                L"malay"                },
   {  LANG_MONGOLIAN,            L"mongolian"            },
   {  LANG_PERSIAN,              L"persian"              },
   {  LANG_POLISH,               L"polish"               },
   {  LANG_PORTUGUESE,           L"portuguese"           },
   {  0x0416,                    L"portuguese brazilian" },
   {  LANG_ROMANIAN,             L"romanian"             },
   {  LANG_RUSSIAN,              L"russian"              },
   {  LANG_SPANISH,              L"spanish"              },
   {  LANG_THAI,                 L"thai"                 },
   {  LANG_TURKISH,              L"turkish"              },
   {  LANG_UKRAINIAN,            L"ukrainian"            },
   {  LANG_URDU,                 L"urdu"                 },
   {  LANG_UZBEK,                L"uzbek"                },
   {  LANG_VIETNAMESE,           L"vietnamese"           }
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CUniNewsSource::CUniNewsSource(void)
   : m_last_news_send(0),m_news_index(0),m_news_sent(0),m_file_last(0)
  {
//--- clear strings
   m_news_path[0]=L'\0';
//--- clear news
   ZeroMemory(m_news,sizeof(m_news));
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CUniNewsSource::~CUniNewsSource(void)
  {
//--- shutdown
   Shutdown();
//--- delete news
   for(uint32_t i=0;i<_countof(m_news);i++)
      if(m_news[i])
         delete m_news[i];
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
bool CUniNewsSource::Initialize(LPCWSTR news_path)
  {
//--- check
   if(!news_path)
      return(false);
//--- reset news send time
   m_last_news_send=_time64(nullptr);
//--- clear buffer
   m_buffer.Clear();
//--- clear news
   ClearNews();
   m_news_sent=0;
//--- reset files list
   m_files.Clear();
   m_file_last=0;
//--- set news path
   CMTStr::Copy(m_news_path,news_path);
//--- extract news
   if(!ExtractNews())
      ExtLogger.Out(MTLogWarn,L"failed to extract news into directory '%s'",m_news_path);
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Shutdown                                                         |
//+------------------------------------------------------------------+
void CUniNewsSource::Shutdown(void)
  {
  }
//+------------------------------------------------------------------+
//| Data processing                                                  |
//+------------------------------------------------------------------+
bool CUniNewsSource::DataProcess(CUniNewsServerPort &server)
  {
   bool res=true;
//--- check time for next news
   if(_time64(nullptr)>m_last_news_send+NEWS_PERIOD)
      res=SendNews(server);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send old news from history to specified context                  |
//+------------------------------------------------------------------+
bool CUniNewsSource::SendNewsHistory(CUniNewsContext &context)
  {
   bool res=true;
//--- lock news
   m_news_sync.Lock();
//--- go through older news and send them
   for(uint32_t i=m_news_index;res && i<_countof(m_news);i++)
      if(m_news[i] && m_news[i]->GetMsgType()==TAG_MSG_NEWS)
         res=context.SendNews(m_news[i],m_news[i]->GetCategory(),true);
//--- go through newer news and send them
   for(uint32_t i=0;res && i<m_news_index;i++)
      if(m_news[i] && m_news[i]->GetMsgType()==TAG_MSG_NEWS)
         res=context.SendNews(m_news[i],m_news[i]->GetCategory(),true);
//--- unlock news
   m_news_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send news                                                        |
//+------------------------------------------------------------------+
void CUniNewsSource::ClearNews(void)
  {
//--- lock news
   m_news_sync.Lock();
//--- go through all news and shutdown them
   for(uint32_t i=0;i<_countof(m_news);i++)
      if(m_news[i])
         m_news[i]->Shutdown();
//--- reset news index
   m_news_index=0;
//--- unlock news
   m_news_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Send news                                                        |
//+------------------------------------------------------------------+
bool CUniNewsSource::SendNews(CUniNewsServerPort &server)
  {
   bool res=true;
//--- get all filenames in directory   
   m_file_last++;
   if(m_file_last>=m_files.Total())
     {
      m_files.Clear();
      ScanFiles(m_news_path,m_files);
      m_file_last=0;
     }
//--- there are no files
   if(m_files.Total()==0)
      return(true);
//--- get next filename
   LPCWSTR filename=m_files[m_file_last];
//--- lock news
   m_news_sync.Lock();
//--- get next news from list
   if(res && m_news[m_news_index]==NULL && (m_news[m_news_index]=new(std::nothrow) CUniNewsMsgNews())==NULL)
     {
      ExtLogger.OutString(MTLogErr,L"failed to allocate memory for news");
      res=false;
     }
//--- reset news for new usage
   if(m_news[m_news_index])
      m_news[m_news_index]->Shutdown();
//--- fill news datetime
   int64_t datetime=_time64(nullptr);
   res=res && m_news[m_news_index]->SetDatetime(datetime);
//--- process news description
   if(res && !FillNewsDescription(filename,m_news[m_news_index]))
     {
      ExtLogger.Out(MTLogWarn,L"failed to fill news description from file '%s'",filename);
      res=false;
     }
//--- read news body
   if(res && !FillNewsBody(filename,m_news[m_news_index]))
     {
      ExtLogger.Out(MTLogWarn,L"failed to read news body from file '%s'",filename);
      res=false;
     }
//--- send news
   if(res && !server.SendNews(m_news[m_news_index],m_news[m_news_index]->GetCategory()))
     {
      ExtLogger.Out(MTLogErr,L"failed to send news #%u",m_news_sent);
      res=false;
     }
//--- write to log and increment index
   if(res)
     {
      ExtLogger.Out(MTLogOK,L"news #%u sent",m_news_sent);
      m_news_index=(m_news_index+1)%_countof(m_news);
     }
   else
     {
      //--- shutdown news
      if(m_news[m_news_index])
         m_news[m_news_index]->Shutdown();
     }
//--- unlock news
   m_news_sync.Unlock();
//--- reset last news send time
   m_last_news_send=datetime;
   m_news_sent++;
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Read news from file                                              |
//+------------------------------------------------------------------+
bool CUniNewsSource::FillNewsDescription(LPCWSTR filename,CUniNewsMsgNews *news)
  {
   LPCWSTR    description=filename;
   CMTStrPath str;
   int32_t        pos=0;
//--- check
   if(!filename || !news)
      return(false);
//--- cut directory from filename
   if((pos=CMTStr::FindChar(description,L'\\'))<0)
      return(true);
   description+=pos+1;
//--- find end of language section
   if((pos=CMTStr::FindChar(description,L'.'))<0)
      return(true);
//--- get news language
   str.Assign(description,pos);
   uint32_t lang=GetLangID(str.Str());
//--- set language
   if(!news->SetLanguage(lang))
      return(false);
//--- move behind language
   description+=pos+1;
//--- find end of subject section
   if((pos=CMTStr::FindRChar(description,L'.'))<0)
      return(false);
//--- get news subject
   CMTStrPath buf;
   buf.Assign(description,pos);
   str.Format(L"%s #%u",buf.Str(),m_news_sent);
//--- set subject
   if(!news->SetSubject(str.Str()))
      return(false);
//--- set priority
   if(!news->SetPriority(false))
      return(false);
//--- set category
   if(!news->SetCategory(DEFAULT_NEWS_CATEGORY))
      return(false);
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Read news from file                                              |
//+------------------------------------------------------------------+
bool CUniNewsSource::FillNewsBody(LPCWSTR filename,CUniNewsMsgNews *news)
  {
//--- check
   if(!filename || !news)
      return(false);
//--- clear buffers
   m_buffer.Clear();
   m_buffer_tmp.Clear();
//--- open file
   CMTFile file;
   if(!file.OpenRead(filename))
      return(false);
//--- reallocate buffer
   m_buffer_tmp.Reallocate(uint32_t(file.Size())+sizeof(wchar_t));
//--- read data
   int32_t size=file.Read(m_buffer_tmp.Buffer(),(DWORD)file.Size());
//--- close file
   file.Close();
//--- check data
   if(size<0)
      return(false);
//--- set buffer size
   m_buffer_tmp.Len(size);
//--- get file extension
   wchar_t ext[16]={0};
   int32_t pos=CMTStr::FindRChar(filename,L'.');
   if(pos>=0)
      CMTStr::Copy(ext,filename+pos+1);
//--- check file extension
   if(CMTStr::CompareNoCase(ext,L"mht")==0)
     {
      //--- calculate unicode text size
      if((size=::MultiByteToWideChar(CP_ACP,0,m_buffer_tmp.Buffer(),m_buffer_tmp.Len(),NULL,0))==0)
         return(false);
      //--- allocate memory
      size*=sizeof(wchar_t);
      if(!m_buffer.Reallocate(size+1))
         return(false);
      //--- convert text
      if(::MultiByteToWideChar(CP_ACP,0,m_buffer_tmp.Buffer(),m_buffer_tmp.Len(),(LPWSTR)m_buffer.Buffer(),size/2)==0)
         return(false);
      //--- set buffer size
      m_buffer.Len(size);
     }
   else
     {
      //--- check html format
      if(CMTStr::CompareNoCase(ext,L"htm")==0 || CMTStr::CompareNoCase(ext,L"html")==0)
        {
         CMIMEConverter::MIMEImageArray images;
         FileNamesArray                 files;
         //--- format images directory from news filename
         wchar_t img_dir[MAX_PATH];
         CMTStr::Copy(img_dir,filename,pos);
         CMTStr::Append(img_dir,L"_files");
         //--- load images
         if(!LoadImages(img_dir,images))
            ExtLogger.Out(MTLogWarn,L"failed to load all images for news file '%s'",filename);
         //--- convert news to mime
         if(!m_mime_converter.Process(CP_UTF8,m_buffer_tmp.Buffer(),m_buffer_tmp.Len(),images,m_buffer))
            return(false);
        }
      else
        {
         ExtLogger.Out(MTLogErr,L"unknown news file '%s' extension",filename);
         return(false);
        }
     }
//--- terminate buffer
   m_buffer.Add(L"",sizeof(L""));
//--- set news body
   if(!news->SetBody((LPCWSTR)m_buffer.Buffer()))
      return(false);
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Load images from directory                                       |
//+------------------------------------------------------------------+
bool CUniNewsSource::LoadImages(LPCWSTR path,CMIMEConverter::MIMEImageArray &images)
  {
   TMTArray<uint32_t> img_data_offsets;
   uint32_t       buff_len=0;
   uint32_t       path_len;
//--- check
   if(!path || CMTStr::CompareNoCase(path,m_news_path,CMTStr::Len(m_news_path))!=0)
      return(false);
//--- save path len
   path_len=CMTStr::Len(path)+1;
//--- get files list
   FileNamesArray files;
   if(!ScanFiles(path,files))
      return(false);
//--- clear images
   images.Clear();
   m_buffer_img.Clear();
//--- load all images
   int32_t     pos     =0;
   LPCWSTR filename=NULL;
   wchar_t ext[16] ={0};
   CMTFile file;
   for(uint32_t i=0;i<files.Total();i++)
     {
      filename=files[i];
      //--- get file extension
      ext[0]=L'\0';
      if((pos=CMTStr::FindRChar(filename,L'.'))>=0)
         CMTStr::Copy(ext,filename+pos+1);
      //--- check extension
      if(CMTStr::FindNoCase(L"png,gif,jpg,jpeg",ext)<0)
         continue;
      //--- open file
      if(!file.OpenRead(filename))
        {
         ExtLogger.Out(MTLogWarn,L"failed to open image '%s'",filename);
         continue;
        }
      //--- allocate buffer
      if(!m_buffer_img.Reallocate(uint32_t(m_buffer_img.Len()+file.Size()+1)))
        {
         ExtLogger.Out(MTLogWarn,L"failed to allocate memory for image '%s'",filename);
         continue;
        }
      //--- read data
      int32_t size=file.Read(m_buffer_img.Buffer()+m_buffer_img.Len(),(DWORD)file.Size());
      //--- close file
      file.Close();
      //--- check data
      if(size<0)
        {
         ExtLogger.Out(MTLogWarn,L"failed to read image '%s'",filename);
         continue;
        }
      //--- format image name
      CMTStrPath img_name;
      img_name.Assign(filename+path_len);
      img_name.ReplaceChar(L'\\',L'/');
      //--- set up image
      CMIMEConverter::MIMEImage image={0};
      CMTStr::Copy(image.name,img_name.Str());
      //--- set image size
      image.data_size=(uint32_t)size;
      //--- add image and it's data offset into arrays
      buff_len=m_buffer_img.Len();
      if(!images.Add(&image) || !img_data_offsets.Add(&buff_len))
        {
         ExtLogger.Out(MTLogWarn,L"failed to add image '%s' to list",filename);
         images.Clear();
         img_data_offsets.Clear();
         break;
        }
      //--- set buffer size
      m_buffer_img.Len(m_buffer_img.Len()+size);
     }
//--- calculate images data pointers from offsets
   for(uint32_t i=0;i<images.Total();i++)
     {
      if(i<img_data_offsets.Total())
         images[i].data=m_buffer_img.Buffer()+img_data_offsets[i];
      else
        {
         images[i].data=NULL;
         images[i].data_size=0;
        }
     }
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Get locale id for language                                       |
//+------------------------------------------------------------------+
uint32_t CUniNewsSource::GetLangID(LPCWSTR language)
  {
//--- check
   if(!language)
      return(LANG_NEUTRAL);
//--- find language
   for(uint32_t i=0;i<_countof(s_languages);i++)
      if(CMTStr::CompareNoCase(s_languages[i].lang_name,language)==0)
         return(s_languages[i].lang_id);
//--- failed to find language
   return(LANG_NEUTRAL);
  }
//+------------------------------------------------------------------+
//| Scan directory for files                                         |
//+------------------------------------------------------------------+
bool CUniNewsSource::ScanFiles(LPCWSTR path,FileNamesArray& files)
  {
   CMTStrPath                tmp;
   HANDLE                    handle;
   WIN32_FIND_DATAW           find={0};
   WIN32_FILE_ATTRIBUTE_DATA attr={0};
   wchar_t                   filename[MAX_PATH];
//--- check
   if(!path)
      return(false);
//--- format mask
   tmp.Format(L"%s\\*.*",path);
//--- find first file in directory
   if((handle=FindFirstFileW(tmp.Str(),&find))!=INVALID_HANDLE_VALUE)
     {
      //--- go through all files
      do
        {
         //--- this is subdirectory
         if(find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;
         //--- format filename with full path
         CMTStr::FormatStr(filename,L"%s\\%s",path,find.cFileName);
         //--- get file attributes
         if(GetFileAttributesExW(filename,GetFileExInfoStandard,&attr)==FALSE)
            ZeroMemory(&attr,sizeof(attr));
         //--- add filename into the list
         if(!files.Add(&filename))
           {
            FindClose(handle);
            return(false);
           }
        }
      while(FindNextFileW(handle,&find));
      //--- close find
      FindClose(handle);
     }
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Extract news files from resources                                |
//+------------------------------------------------------------------+
bool CUniNewsSource::ExtractNews(void)
  {
   CMTStrPath path;
//--- scan for files in news directory
   m_files.Clear();
   ScanFiles(m_news_path,m_files);
//--- if there is exists any file in directory, exit
   if(m_files.Total()>0)
     {
      ExtLogger.OutString(MTLogOK,L"news directory is not empty, news extraction skipped");
      return(true);
     }
//--- extract english news file
   path.Format(L"%s\\%s",m_news_path,L"english.Welcome!.mht");
   bool res=ExtractFile(IDR_NEWS_ENGLISH,path.Str());
//--- extract russian news file
   path.Format(L"%s\\%s",m_news_path,L"russian.Добро пожаловать!.mht");
   res=res && ExtractFile(IDR_NEWS_RUSSIAN,path.Str());
//--- extract italian news file
   path.Format(L"%s\\%s",m_news_path,L"italian.Ciao!.htm");
   res=res && ExtractFile(IDR_NEWS_ITALIAN,path.Str());
//--- extract italian news image
   path.Format(L"%s\\%s",m_news_path,L"italian.Ciao!_files\\welcome-en.jpg");
   res=res && ExtractFile(IDR_NEWS_ITALIAN_IMG,path.Str());
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Extract file from resource                                       |
//+------------------------------------------------------------------+
bool CUniNewsSource::ExtractFile(uint32_t id,LPCWSTR filepath)
  {
   CMTFile    file;
   CMTStrPath path;
   HRSRC      hRes    =NULL;
   HGLOBAL    hGlobal =NULL;
   DWORD      filesize=0;
   int32_t        pos     =0;
   char      *filedata=NULL;
   bool       res     =false;
//--- check
   if(!id || !filepath || filepath[0]==L'\0')
      return(false);
//--- save path
   path.Assign(filepath);
//--- get file directory
   if((pos=path.FindRChar(L'\\'))>0)
      path.Trim((uint32_t)pos);
//--- create directory
   if(::GetFileAttributesW(path.Str())==INVALID_FILE_ATTRIBUTES)
      CMTFile::DirectoryCreate(path);
//--- search for resource
   if(((hRes=::FindResourceW(NULL,MAKEINTRESOURCE(id),L"FILES")))!=NULL)
     {
      //--- get size of resource
      filesize=::SizeofResource(NULL,hRes);
      //--- load resource
      hGlobal =::LoadResource(NULL,hRes);
      //--- get data pointer
      if(hGlobal && (filedata=(char*)::LockResource(hGlobal)))
        {
         //--- remove old file
         if(::GetFileAttributesW(filepath)!=INVALID_FILE_ATTRIBUTES)
            ::DeleteFileW(filepath);
         //--- open the file
         res=file.Open(filepath,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,CREATE_NEW);
         //--- write data to the file
         res=res && (file.Write(filedata,filesize)==filesize);
         //--- free resource
         ::FreeResource(hGlobal);
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
