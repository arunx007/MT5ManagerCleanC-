//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ZipFile.h"
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CZipFile::CZipFile(void) : m_buffer(NULL)
  {
   ZeroMemory(&m_info,sizeof(m_info));
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CZipFile::~CZipFile(void)
  {
   if(m_buffer)
     {
      delete[] m_buffer;
      m_buffer=NULL;
     }
   Close();
  }
//+------------------------------------------------------------------+
//| ZIP file open                                                    |
//+------------------------------------------------------------------+
bool CZipFile::Open(LPCWSTR path)
  {
//--- check
   if(!path || *path==0)
      return(false);
   Close();
//---
   m_path.Assign(path);
//--- open file
   if(!m_file.Open(m_path.Str(),GENERIC_READ|GENERIC_WRITE,0,CREATE_ALWAYS))
      return(false);
//--- seek to end
   m_file.Seek(0,FILE_END);
//--- current position
   m_info.begin_pos=(uint32_t)m_file.CurrPos();
//---
   return(true);
  }
//+------------------------------------------------------------------+
//| Write Central Directory                                         |
//+------------------------------------------------------------------+
bool CZipFile::WriteEndOfCentralDirectory(uint64_t& pos_eocd_inzip,const uint64_t pos_centdir_inzip)
  {
   pos_eocd_inzip=0;
//--- check central directory position
   if(pos_centdir_inzip<=UINT_MAX)
      return(false);
//--- get current position
   uint64_t pos_eocd_start=0;
   if((pos_eocd_start=m_file.CurrPos())==CMTFile::INVALID_POSITION)
      return(false);
//--- write header
   if(!PutValue64(SZlib::ZIP64ENDOFCENTDIRMAGIC,4) ||                 // EOCD
      !PutValue64(        SIZEZIP64ENDOFCENTDIR,8) ||                 // size of EOCD
      !PutValue64(                VERSIONMADEBY,2) ||                 // version made by
      !PutValue64(         EXTRACTVERSION_ZIP64,2) ||                 // version needed to extract
      !PutValue64(                            0,4) ||                 // number of this disk
      !PutValue64(                            0,4) ||                 // num disk with this CD
      !PutValue64(          m_info.number_entry,8) ||                 // total number of entries in the CD on this disk
      !PutValue64(          m_info.number_entry,8) ||                 // total number of entries in the CD
      !PutValue64(m_info.ci.size_centralheader+SIZEEXTRAFIELD_CD,8)|| // size of the central directory
      !PutValue64(            pos_centdir_inzip,8))                   // offset of start of CD with respect to the starting disk number
      return(false);
//---
   pos_eocd_inzip=pos_eocd_start;
   return(true);
  }
//+------------------------------------------------------------------+
//| Write Central Directory locator                                  |
//+------------------------------------------------------------------+
bool CZipFile::WriteEndOfCentralDirectoryLocator(const uint64_t pos_eocd_inzip)
  {
   if(!PutValue64(SZlib::ZIP64EOCD_LOCATORMAGIC,4) || // signature                       
      !PutValue64(                            0,4) || // number of the disk with the start of the zip64 end of central directory
      !PutValue64(               pos_eocd_inzip,8) || // relative offset of the zip64 end of central directory record
      !PutValue64(                            1,4))   // total number of disks
      return(false);
   return(true);
  }
//+------------------------------------------------------------------+
//| Close                                                            |
//+------------------------------------------------------------------+
bool CZipFile::Close(void)
  {
   uint64_t centraldir_pos_in_zip;
//--- check
   if(!m_file.IsOpen())
      return(true);
//--- close current file
   if(m_info.in_opened_file_in_zip)
      FileInZipClose();
//--- get current position
   if((centraldir_pos_in_zip=m_file.CurrPos())==CMTFile::INVALID_POSITION)
      return(false);
//--- write catalog structure
   uint32_t size_centraldir=BlockWrite();
//--- if using zip64
   if(centraldir_pos_in_zip>=UINT_MAX)
     {
      uint64_t eocd_pos_in_zip=0;
      //--- add extended headers
      if(!WriteEndOfCentralDirectory(eocd_pos_in_zip,centraldir_pos_in_zip))
         return(false);
      if(!WriteEndOfCentralDirectoryLocator(eocd_pos_in_zip))
         return(false);
     }
//--- add end of central directory
   PutValue((uint32_t)     ENDHEADERMAGIC,4);        // Magic End
   PutValue((uint32_t)                  0,2);        // Number of this disk
   PutValue((uint32_t)                  0,2);        // Number of the disk with the start of the central directory
   PutValue((uint32_t)m_info.number_entry,2);        // Total number of entries in the central dir on this disk
   PutValue((uint32_t)m_info.number_entry,2);        // Total number of entries in the central dir
   PutValue((uint32_t)    size_centraldir,4);        // Size of the central directory
   PutValue((uint32_t)(centraldir_pos_in_zip>=UINT_MAX ? UINT_MAX : centraldir_pos_in_zip),4);
   // Offset of start of central directory with respect to the starting disk number
   PutValue((uint32_t)0,2);                          // Zip file comment length
//--- close file
   m_file.Close();
//--- release buffer
   if(m_info.ci.buffered_data)
     {
      delete[] m_info.ci.buffered_data;
      m_info.ci.buffered_data=NULL;
     }
//--- cleanup
   BlockFreeAll();
//---
   m_info.in_opened_file_in_zip=false;
   m_info.begin_pos            =0;
   m_info.number_entry         =0;
//---
   return(true);
  }
//+------------------------------------------------------------------+
//| Add file to zip                                                  |
//+------------------------------------------------------------------+
bool CZipFile::AddFile(LPCWSTR path,LPCWSTR zip_path)
  {
   WIN32_FILE_ATTRIBUTE_DATA fa;
   ZipFileInfo               info={};
   CMTFile                   file;
   CMTStrPath                str(zip_path);
   uint32_t                  readed;
//--- check
   if(!path || !zip_path)
      return(false);
//--- get file attributes
   if(GetFileAttributesExW(path,GetFileExInfoStandard,&fa)==FALSE)
      return(FALSE);
//--- open file
   if(!file.Open(path,GENERIC_READ,FILE_SHARE_READ,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL))
      return(false);
//--- parse attributes
   FileTimeToSystemTime(&fa.ftLastWriteTime,&info.tmzDate);
   info.dosDate    =0;
   info.external_fa=fa.dwFileAttributes;
   info.internal_fa=0;
   info.file_size  =file.Size();
//--- replace slashes
   str.ReplaceChar(L'\\',L'/');
   CMTStr::Copy(info.file_path,_countof(info.file_path),str.Str());
//--- open new file in ZIP
   if(!FileInZipOpenNew(info))
      return(false);
//--- allocate buffer
   if(!m_buffer)
      if((m_buffer=new(std::nothrow) char[ZIP_READ_CHUNK+128])==NULL)
         return(false);
//--- reading from file and storing to ZIP
   while((readed=file.Read(m_buffer,ZIP_READ_CHUNK))>0)
      if(!FileInZipWrite(m_buffer,readed))
         return(false);
//--- close file in ZIP
   return(FileInZipClose());
  }
//+------------------------------------------------------------------+
//| Add directory to zip                                             |
//+------------------------------------------------------------------+
bool CZipFile::AddDirectory(LPCWSTR path)
  {
   ZipFileInfo info={};
   CMTStrPath  str(path);
//--- check
   if(!path)
      return(false);
//--- get file attributes
   GetSystemTime(&info.tmzDate);
   info.dosDate    =0;
   info.external_fa=FILE_ATTRIBUTE_DIRECTORY;
   info.internal_fa=0;
//--- replace slashes
   str.ReplaceChar(L'\\',L'/');
   CMTStr::Copy(info.file_path,_countof(info.file_path),str.Str());
//--- open new directory in ZIP
   if(!FileInZipOpenNew(info))
      return(false);
//--- close file
   return(FileInZipClose());
  }
//+------------------------------------------------------------------+
//| Add directory to zip                                             |
//+------------------------------------------------------------------+
bool CZipFile::AddDirectory(LPCWSTR zip_path,LPCWSTR full_path,LPCWSTR mask)
  {
//--- check
   if(!zip_path || !full_path || !mask)
      return(false);
//--- check file on disk
   if(GetFileAttributesW(full_path)==INVALID_FILE_ATTRIBUTES)
      return(false);
//--- add directory
   if(zip_path[0]!=0)
      if(!AddDirectory(zip_path))
         return(false);
//---
   WIN32_FIND_DATAW ffd={};
   HANDLE          hFind;
   CMTStrPath      tmp,tmp_zip;
//---
   tmp.Format(L"%s\\%s",full_path,mask);
   if((hFind=FindFirstFileW(tmp.Str(),&ffd))==INVALID_HANDLE_VALUE)
      return(true);
//--- iterate directory
   do
     {
      //--- check directories
      if((ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0)
        {
         //--- check system paths
         if(wcscmp(ffd.cFileName,L".")==0 || wcscmp(ffd.cFileName,L"..")==0)
            continue;
         //--- if mask is *.*
         if(wcscmp(mask,L"*.*")==0)
           {
            tmp.Format(L"%s\\%s",full_path,ffd.cFileName);
            if(zip_path[0]!=0)
               tmp_zip.Format(L"%s\\%s",zip_path,ffd.cFileName);
            else
               tmp_zip.Assign(ffd.cFileName);
            //--- add directory
            if(!AddDirectory(tmp_zip.Str(),tmp.Str(),mask))
              {
               FindClose(hFind);
               return(false);
              }           }
         continue;
        }
      //--- add file
      tmp.Format(L"%s\\%s",full_path,ffd.cFileName);
      if(zip_path[0]!=0)
         tmp_zip.Format(L"%s\\%s",zip_path,ffd.cFileName);
      else
         tmp_zip.Assign(ffd.cFileName);

      if(!AddFile(tmp.Str(),tmp_zip.Str()))
        {
         FindClose(hFind);
         return(false);
        }
     } while(FindNextFileW(hFind,&ffd));

   FindClose(hFind);
//---
   return(true);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void CZipFile::PutValueInMemory(BYTE dest,uint32_t x,int size)
  {
   BYTE *buffer=(BYTE*)m_info.ci.central_header+dest;
//---
   for(int32_t i=0;i<size;i++)
     {
      buffer[i]=(BYTE)(x & 0xff);
      x>>= 8;
     }
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
bool CZipFile::PutValueInMemory(char *buff,const uint32_t buff_size,uint64_t x,const uint32_t size)
  {
//--- check
   if(buff==NULL || buff_size<size)
      return(false);
//--- 
   BYTE *buffer=(BYTE*)buff;
   for(uint32_t i=0;i<size;i++)
     {
      buffer[i]=(BYTE)(x & 0xff);
      x>>= 8;
     }
   return(true);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
bool CZipFile::PutValue(uint32_t x,int size)
  {
   BYTE buffer[4];
//--- prepare buffer
   for(int32_t i=0;i<size;i++)
     {
      buffer[i]=(BYTE)(x&0xff);
      x>>=8;
     }
//---
   return(m_file.Write(buffer,size)==size);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
bool CZipFile::PutValue64(uint64_t x,const int32_t size)
  {
   BYTE buffer[sizeof(x)];
//--- prepare buffer
   for(int32_t i=0;i<size;i++)
     {
      buffer[i]=(BYTE)(x&0xff);
      x>>=8;
     }
//---
   return(m_file.Write(buffer,size)==size);
  }
//+------------------------------------------------------------------+
//| Update file information in zip                                   |
//+------------------------------------------------------------------+
void CZipFile::UpdateZipInfo(ZipFileInfo &info)
  {
   WIN32_FILE_ATTRIBUTE_DATA fa;
   FILETIME                  ft;
   SYSTEMTIME                st;
//--- get file attributes
   if(GetFileAttributesExW(info.file_path,GetFileExInfoStandard,&fa)==FALSE)
      return;
//---
   ft=fa.ftCreationTime;
   FileTimeToSystemTime(&ft,&st);
//--- update
   info.tmzDate    =st;
   info.external_fa=GetFileAttributesW(info.file_path);
  }
//+------------------------------------------------------------------+
//| Open file in ZIP                                                 |
//+------------------------------------------------------------------+
bool CZipFile::FileInZipOpenNew(ZipFileInfo &info)
  {
   char filename[1024]={0};
   int32_t  filename_len;
//--- check
   if(info.file_path[0]==0)
      CMTStr::Copy(info.file_path,L"-");
//--- copy file name
   CMTStr::Copy(filename,info.file_path);
//--- convert to UTF8
   filename_len=WideCharToMultiByte(CP_UTF8,0,info.file_path,CMTStr::Len(info.file_path),NULL,0,NULL,NULL);
   WideCharToMultiByte(CP_UTF8,0,info.file_path,CMTStr::Len(info.file_path),filename,_countof(filename),NULL,NULL);
//--- close previous file in zip
   if(m_info.in_opened_file_in_zip)
      FileInZipClose();
//--- prepare
   m_info.ci.dosDate             =GetDosDate(info);
   m_info.ci.flag                =0x800; // set 11 bit: Language encoding flag (EFS). If this bit is set, the filename and comment fields for this file must be encoded using UTF-8.
   m_info.ci.crc32               =0;
   m_info.ci.method              =SZlib::Z_DEFLATED;
   m_info.ci.stream_initialised  =0;
   m_info.ci.pos_in_buffered_data=0;
   m_info.ci.file_size           =info.file_size;
   m_info.ci.filename_len        =(uint32_t)filename_len;
//--- check position
   uint64_t file_cur_pos=m_file.CurrPos();
   if(file_cur_pos==CMTFile::INVALID_POSITION)
      return(false);
   m_info.ci.pos_local_header=(uint32_t)file_cur_pos;
//--- checks if file above 4Gb (then use Zip64)
   if(info.file_size>=UINT_MAX)
      m_info.ci.using_zip64=true;
//--- reserve for header
   m_info.ci.size_centralheader=(uint32_t)(SIZECENTRALHEADER+filename_len);
//--- allocate memory for header
   if((m_info.ci.central_header=new(std::nothrow) char[m_info.ci.size_centralheader])==NULL)
      return(false);
//--- write header
   PutValueInMemory( 0,(uint32_t)SZlib::CENTRALHEADERMAGIC ,4);
   PutValueInMemory( 4,(uint32_t)VERSIONMADEBY             ,2);
   PutValueInMemory( 6,(uint32_t)(m_info.ci.using_zip64 ? EXTRACTVERSION_ZIP64 : EXTRACTVERSION),2);
   PutValueInMemory( 8,(uint32_t)m_info.ci.flag            ,2);
   PutValueInMemory(10,(uint32_t)m_info.ci.method          ,2);
   PutValueInMemory(12,(uint32_t)m_info.ci.dosDate         ,4);
   PutValueInMemory(16,(uint32_t)0                         ,4); // crc
   PutValueInMemory(20,(uint32_t)UINT_MAX                  ,4); // compr size
   PutValueInMemory(24,(uint32_t)UINT_MAX                  ,4); // uncompr size
   PutValueInMemory(28,(uint32_t)filename_len              ,2);
   PutValueInMemory(30,(uint32_t)0                         ,2);
   PutValueInMemory(32,(uint32_t)0                         ,2);
   PutValueInMemory(34,(uint32_t)0                         ,2); // disk nm start
   PutValueInMemory(36,(uint32_t)info.internal_fa          ,2);
   PutValueInMemory(38,(uint32_t)info.external_fa          ,4);
   PutValueInMemory(42,(uint32_t)m_info.ci.pos_local_header,4);
//--- write file name
   memcpy(m_info.ci.central_header+SIZECENTRALHEADER,filename,filename_len);
//--- write local header
   PutValue((uint32_t)SZlib::LOCALHEADERMAGIC,4);
   PutValue((uint32_t)m_info.ci.using_zip64 ? EXTRACTVERSION_ZIP64 : EXTRACTVERSION,2);
   PutValue((uint32_t)m_info.ci.flag         ,2);
   PutValue((uint32_t)m_info.ci.method       ,2);
   PutValue((uint32_t)m_info.ci.dosDate      ,4);
   PutValue((uint32_t)0                      ,4);   // crc 32, unknown
   PutValue((uint32_t)UINT_MAX               ,4);   // compressed size, unknown
   PutValue((uint32_t)UINT_MAX               ,4);   // uncompressed size, unknown
   PutValue((uint32_t)filename_len           ,2);
   PutValue((uint32_t)m_info.ci.using_zip64 ? 2*sizeof(uint64_t)+4 : 0,2); // extra field length
//--- write file name in file
   if(m_file.Write(filename,(uint32_t)filename_len)!=(uint32_t)filename_len)
      return(false);
//--- reserve extra field
   if(m_info.ci.using_zip64) m_file.Write("reserv_for_extra_fld",20);
//--- allocate memory for buffer
   if(m_info.ci.buffered_data==NULL)
      if((m_info.ci.buffered_data=new(std::nothrow) uint8_t[SZlib::Z_BUFSIZE])==NULL)
         return(false);
//--- setup zip compression
   m_info.ci.stream.avail_in =(uint32_t)0;
   m_info.ci.stream.avail_out=(uint32_t)SZlib::Z_BUFSIZE;
   m_info.ci.stream.next_out =(char*)m_info.ci.buffered_data;
   m_info.ci.stream.total_in =0;
   m_info.ci.stream.total_out=0;
//--- init
   int32_t err=SZlib::deflateInit2(&m_info.ci.stream,
                                 SZlib::Z_DEFAULT_COMPRESSION,
                                 SZlib::Z_DEFLATED,
                                -SZlib::MAX_WBITS,
                                 SZlib::DEF_MEM_LEVEL,
                                 SZlib::Z_DEFAULT_STRATEGY);
//--- check result
   if((err!=SZlib::Z_OK) && (err!=SZlib::Z_NEED_DICT))
      return(false);
//---
   m_info.ci.stream_initialised=1;
   m_info.in_opened_file_in_zip=true;
//---
   return(true);
  }
//+------------------------------------------------------------------+
//| Write file in ZIP                                                |
//+------------------------------------------------------------------+
bool CZipFile::FileInZipWrite(const void *buf,uint32_t len)
  {
   int32_t    err   =SZlib::Z_OK;
   uint64_t before=0;
//--- check
   if(!m_info.in_opened_file_in_zip)
      return(false);
//---
   m_info.ci.stream.next_in =(char*)buf;
   m_info.ci.stream.avail_in=len;
   m_info.ci.crc32          =SZlib::crc32(m_info.ci.crc32,(uint8_t*)buf,len);
//--- deflate
   while(err==SZlib::Z_OK && m_info.ci.stream.avail_in>0)
     {
      //--- check
      if(m_info.ci.stream.avail_out==0)
        {
         //--- flush on disk
         if(m_file.Write(m_info.ci.buffered_data,m_info.ci.pos_in_buffered_data)!=m_info.ci.pos_in_buffered_data)
            return(false);
         //--- flush buffers
         m_info.ci.pos_in_buffered_data=0;
         m_info.ci.stream.avail_out    =(uint32_t)SZlib::Z_BUFSIZE;
         m_info.ci.stream.next_out     =(char*)m_info.ci.buffered_data;
        }
      //--- deflate
      before=m_info.ci.stream.total_out;
      err=SZlib::deflate(&m_info.ci.stream,SZlib::Z_NO_FLUSH);
      m_info.ci.pos_in_buffered_data+=(uint32_t)(m_info.ci.stream.total_out-before);
     }
//--- check result
   if((err!=SZlib::Z_OK) && (err!=SZlib::Z_NEED_DICT))
      return(false);
//--- 
   return(true);
  }
//+------------------------------------------------------------------+
//| Close file in ZIP                                                |
//+------------------------------------------------------------------+
bool CZipFile::FileInZipClose(void)
  {
   int32_t    err=SZlib::Z_OK;
   char   central_header_ex[20];
   uint64_t cur_pos_in_zip;
   uint64_t before;
//--- check
   if(!m_info.in_opened_file_in_zip)
      return(false);
//---
   m_info.ci.stream.avail_in=0;
//---
   while(err==SZlib::Z_OK)
     {
      //--- check
      if(m_info.ci.stream.avail_out==0)
        {
         //--- flush on disk
         if(m_file.Write(m_info.ci.buffered_data,m_info.ci.pos_in_buffered_data)!=m_info.ci.pos_in_buffered_data)
            return(false);
         //---
         m_info.ci.pos_in_buffered_data=0;
         m_info.ci.stream.avail_out    =SZlib::Z_BUFSIZE;
         m_info.ci.stream.next_out     =(char*)m_info.ci.buffered_data;
        }
      //--- deflate
      before=m_info.ci.stream.total_out;
      err=SZlib::deflate(&m_info.ci.stream,SZlib::Z_FINISH);
      m_info.ci.pos_in_buffered_data+=(uint32_t)(m_info.ci.stream.total_out-before);
     }
//--- correct result
   if(err==SZlib::Z_STREAM_END)
      err=SZlib::Z_OK;
//--- check result
   if((err!=SZlib::Z_OK) && (err!=SZlib::Z_NEED_DICT))
      return(false);
//--- final flush
   if(m_info.ci.pos_in_buffered_data>0)
      if(m_file.Write(m_info.ci.buffered_data,m_info.ci.pos_in_buffered_data)!=m_info.ci.pos_in_buffered_data)
         return(false);
//--- end
   err=SZlib::deflateEnd(&m_info.ci.stream);
   m_info.ci.stream_initialised=0;
//--- check result
   if((err!=SZlib::Z_OK) && (err!=SZlib::Z_NEED_DICT))
      return(false);
//--- correct parameters
   PutValueInMemory(16,m_info.ci.crc32,4);            // crc
//--- check sizes
   if(m_info.ci.stream.total_in>=UINT_MAX || m_info.ci.stream.total_out>=UINT_MAX)
     {
      PutValueInMemory(20,(uint32_t)UINT_MAX,4);  // compr size
      PutValueInMemory(24,(uint32_t)UINT_MAX,4);  // uncompr size
      //--- filling
      if(!PutValueInMemory(&central_header_ex[0], 20,(uint64_t)SZlib::EXTRAHEADERID_ZIP64,2) || // header id
         !PutValueInMemory(&central_header_ex[2], 18,(uint64_t)sizeof(uint64_t)*2          ,2) || // size of data-block
         !PutValueInMemory(&central_header_ex[4], 16,(uint64_t)m_info.ci.stream.total_in ,8) || // original size
         !PutValueInMemory(&central_header_ex[12], 8,(uint64_t)m_info.ci.stream.total_out,8))   // compressed size
         return(false);
      //--- change size of extra field
      PutValueInMemory(30,(uint32_t)SIZEEXTRAFIELD_CD,2);
      //--- add central dir header
      BlockAddData(m_info.ci.central_header,(uint32_t)m_info.ci.size_centralheader);
      //--- add extra
      BlockAddData(central_header_ex,20);

     }
   else
     {
      //--- updte sizes
      PutValueInMemory(20,(uint32_t)m_info.ci.stream.total_out,4); // compr size
      PutValueInMemory(24,(uint32_t)m_info.ci.stream.total_in ,4); // uncompr size
      //--- add central dir header
      BlockAddData(m_info.ci.central_header,(uint32_t)m_info.ci.size_centralheader);
     }
//--- free memory
   if(m_info.ci.central_header!=NULL)
     {
      delete[] m_info.ci.central_header;
      m_info.ci.central_header=NULL;
     }
//--- get current position
   if((cur_pos_in_zip=m_file.CurrPos())==CMTFile::INVALID_POSITION)
      return(false);
//--- seek to place of original/compressed size position
   if(m_file.Seek(m_info.ci.pos_local_header+SZlib::OFFSET_LOCAL_COMPSIZE,FILE_BEGIN)==CMTFile::INVALID_POSITION)
      return(false);
//--- write to file
   PutValue(m_info.ci.crc32,4); // crc 32, unknown
//--- if not using zip64
   if(!m_info.ci.using_zip64)
     {
      PutValue((uint32_t)m_info.ci.stream.total_out,4); // compressed size
      PutValue((uint32_t)m_info.ci.stream.total_in ,4); // uncompressed size
     }
   else
     {
      //--- if using zip64
      //--- write invalid values
      PutValue((uint32_t)UINT_MAX,4);
      PutValue((uint32_t)UINT_MAX,4);
      //--- seek to extra field
      if(m_file.Seek(m_info.ci.pos_local_header+SZlib::OFFSET_LOCAL_FILENAME+m_info.ci.filename_len,FILE_BEGIN)==CMTFile::INVALID_POSITION)
         return(false);
      //--- write sizes
      PutValue(  (uint32_t)SZlib::EXTRAHEADERID_ZIP64  ,2); // id of zip64
      PutValue(  (uint32_t)sizeof(uint64_t)*2            ,2); // size of zip64
      PutValue64((uint64_t)m_info.ci.stream.total_in ,8); // 8-bytes original size
      PutValue64((uint64_t)m_info.ci.stream.total_out,8); // 8-bytes compressed size
     }
//--- restore seek position
   if(m_file.Seek(cur_pos_in_zip,FILE_BEGIN)==CMTFile::INVALID_POSITION)
      return(false);
//---
   m_info.number_entry++;
   m_info.in_opened_file_in_zip=false;
//--- 
   return(true);
  }
//+------------------------------------------------------------------+
//| Write block information about files structure                    |
//+------------------------------------------------------------------+
int32_t CZipFile::BlockWrite(void)
  {
   ZipFileDataBlock  *block=m_info.first;
   int32_t                size=0;
//--- iterate blocks
   while(block)
     {
      if(block->filled_in_this_block>0)
         if(m_file.Write(block->data,block->filled_in_this_block)!=block->filled_in_this_block)
            return(0);
      //--- next block
      size+=block->filled_in_this_block;
      block=block->next;
     }
//--- return size
   return(size);
  }
//+------------------------------------------------------------------+
//| Add data                                                         |
//+------------------------------------------------------------------+
void CZipFile::BlockAddData(char *buf,uint32_t len)
  {
   ZipFileDataBlock *block;
   uint32_t          copy_this;
   uint8_t          *to_copy;
//--- allocate block
   if(m_info.last==NULL)
      if((m_info.first=m_info.last=BlockAlloc())==NULL)
         return;
//--- get iterator
   block=m_info.last;
//---
   while(len>0)
     {
      if(block->avail_in_this_block==0)
        {
         if((block->next=BlockAlloc())==NULL)
            return;
         block      =block->next;
         m_info.last=block;
        }
      //---
      if(block->avail_in_this_block<len)
         copy_this=(uint32_t)block->avail_in_this_block;
      else copy_this=(uint32_t)len;
      //---
      to_copy=&(block->data[block->filled_in_this_block]);
      memcpy(to_copy,buf,copy_this);
      //---
      block->filled_in_this_block+=copy_this;
      block->avail_in_this_block -=copy_this;
      //---
      buf+=copy_this;
      len-=copy_this;
     }
  }
//+------------------------------------------------------------------+
//| Free blocks                                                      |
//+------------------------------------------------------------------+
void CZipFile::BlockFreeAll(void)
  {
   ZipFileDataBlock *block=m_info.first,*next;
//--- удаляем все
   while(block!=NULL)
     {
      next=block->next;
      BlockFree(block);
      block=next;
     }
//---
   m_info.first=m_info.last=NULL;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
CZipFile::ZipFileDataBlock * CZipFile::BlockAlloc(void)
  {
//--- allocate block
   ZipFileDataBlock *block=new(std::nothrow) ZipFileDataBlock;
   if(block==NULL)
      return(NULL);
//--- zero
   ZeroMemory(block,sizeof(ZipFileDataBlock));
//--- allocate buffer
   if((block->data=new(std::nothrow) BYTE[SIZEDATA_INDATABLOCK])==NULL)
     {
      delete block;
      return(NULL);
     }
//---
   block->avail_in_this_block=SIZEDATA_INDATABLOCK;
   return(block);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void CZipFile::BlockFree(ZipFileDataBlock *block)
  {
   if(block)
     {
      //--- free buffer
      if(block->data!=NULL)
        {
         delete[] block->data;
         block->data=NULL;
        }
      //--- free block
      delete block;
     }
  }
//+------------------------------------------------------------------+
//| Get DOS date                                                     |
//+------------------------------------------------------------------+
uint32_t CZipFile::GetDosDate(ZipFileInfo &info)
  {
   LONG                  ulDosDate=0;
   WORD                  wDate,wTime,*pWord=(WORD*)&ulDosDate;
   FILETIME              ft;
   SYSTEMTIME            localTime;
   TIME_ZONE_INFORMATION tzInfo;
//---
   if(info.dosDate)
      return(info.dosDate);
//--- получим
   GetTimeZoneInformation(&tzInfo);
   SystemTimeToTzSpecificLocalTime(&tzInfo,&info.tmzDate,&localTime);
   SystemTimeToFileTime(&localTime,&ft);
   FileTimeToDosDateTime(&ft,&wDate,&wTime);
   *(pWord+0)=wTime;
   *(pWord+1)=wDate;
//--- return date
   return(ulDosDate);
  }
//+------------------------------------------------------------------+

